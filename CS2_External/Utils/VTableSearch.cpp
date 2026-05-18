#include "ProcessManager.hpp"
#include <vector>
#include <cstring>
#include <algorithm>
#include <Windows.h>
#include <winnt.h>

namespace
{
	constexpr std::size_t kChunkSize = 0x10000;

	struct SectionInfo
	{
		DWORD64 Base;
		DWORD   Size;
		DWORD   Characteristics;
		char    Name[9];
	};

	std::vector<SectionInfo> GetSections(DWORD64 ModuleBase)
	{
		std::vector<SectionInfo> Sections;
		if (ModuleBase == 0)
			return Sections;

		IMAGE_DOS_HEADER Dos{};
		if (!ProcessMgr.ReadMemory(ModuleBase, Dos) || Dos.e_magic != IMAGE_DOS_SIGNATURE)
			return Sections;

		IMAGE_NT_HEADERS64 Nt{};
		if (!ProcessMgr.ReadMemory(ModuleBase + Dos.e_lfanew, Nt) || Nt.Signature != IMAGE_NT_SIGNATURE)
			return Sections;

		const DWORD64 First = ModuleBase + Dos.e_lfanew
			+ offsetof(IMAGE_NT_HEADERS64, OptionalHeader)
			+ Nt.FileHeader.SizeOfOptionalHeader;

		Sections.reserve(Nt.FileHeader.NumberOfSections);
		for (WORD i = 0; i < Nt.FileHeader.NumberOfSections; ++i)
		{
			IMAGE_SECTION_HEADER Hdr{};
			if (!ProcessMgr.ReadMemory(First + i * sizeof(IMAGE_SECTION_HEADER), Hdr))
				continue;

			SectionInfo Sec{};
			Sec.Base = ModuleBase + Hdr.VirtualAddress;
			Sec.Size = Hdr.Misc.VirtualSize;
			Sec.Characteristics = Hdr.Characteristics;
			std::memcpy(Sec.Name, Hdr.Name, 8);
			Sec.Name[8] = '\0';
			Sections.push_back(Sec);
		}

		return Sections;
	}

	template <typename Matcher>
	DWORD64 ScanChunked(DWORD64 Base, DWORD Size, std::size_t Overlap, Matcher&& MatchFn)
	{
		std::vector<BYTE> Buffer(kChunkSize + Overlap);

		for (std::size_t Offset = 0; Offset < Size; Offset += kChunkSize)
		{
			const std::size_t ReadSize = std::min<std::size_t>(kChunkSize + Overlap, Size - Offset);
			if (!ProcessMgr.ReadMemory(Base + Offset, *Buffer.data(), static_cast<int>(ReadSize)))
				continue;

			const std::size_t MatchOffset = MatchFn(Buffer.data(), ReadSize);
			if (MatchOffset != std::size_t(-1))
				return Base + Offset + MatchOffset;
		}

		return 0;
	}
}

DWORD64 ProcessManager::GetModuleSize(DWORD64 ModuleBase)
{
	if (ModuleBase == 0)
		return 0;

	HANDLE Snap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, ProcessID);
	if (Snap == INVALID_HANDLE_VALUE)
		return 0;

	MODULEENTRY32 Entry{};
	Entry.dwSize = sizeof(MODULEENTRY32);
	DWORD64 Result = 0;

	if (Module32First(Snap, &Entry))
	{
		do
		{
			if (reinterpret_cast<DWORD64>(Entry.modBaseAddr) == ModuleBase)
			{
				Result = Entry.modBaseSize;
				break;
			}
		} while (Module32Next(Snap, &Entry));
	}

	CloseHandle(Snap);
	return Result;
}

DWORD64 ProcessManager::FindQWordInSections(DWORD64 ModuleBase, DWORD64 Value, DWORD RequiredCharacteristics)
{
	for (const auto& Sec : GetSections(ModuleBase))
	{
		if ((Sec.Characteristics & RequiredCharacteristics) != RequiredCharacteristics || Sec.Size < 8)
			continue;

		DWORD64 Result = ScanChunked(Sec.Base, Sec.Size, 8, [&](const BYTE* Data, std::size_t SizeBytes) -> std::size_t
			{
				for (std::size_t i = 0; i + 8 <= SizeBytes; i += 8)
				{
					if (*reinterpret_cast<const DWORD64*>(Data + i) == Value)
						return i;
				}
				return std::size_t(-1);
			});

		if (Result)
			return Result;
	}

	return 0;
}

DWORD64 ProcessManager::FindVTable(DWORD64 ModuleBase, const std::string& ClassName)
{
	const std::string DescriptorName = ".?AV" + ClassName + "@@";
	DWORD64 TypeDescriptor = 0;

	for (const auto& Sec : GetSections(ModuleBase))
	{
		constexpr DWORD Required = IMAGE_SCN_CNT_INITIALIZED_DATA | IMAGE_SCN_MEM_READ;
		if ((Sec.Characteristics & Required) != Required || Sec.Size <= DescriptorName.size())
			continue;

		DWORD64 Result = ScanChunked(Sec.Base, Sec.Size, DescriptorName.size(), [&](const BYTE* Data, std::size_t SizeBytes) -> std::size_t
			{
				for (std::size_t i = 0; i + DescriptorName.size() < SizeBytes; ++i)
				{
					if (std::memcmp(Data + i, DescriptorName.data(), DescriptorName.size() + 1) == 0)
						return i;
				}
				return std::size_t(-1);
			});

		if (Result)
		{
			TypeDescriptor = Result - 0x10;
			break;
		}
	}

	if (!TypeDescriptor)
		return 0;

	const DWORD DescriptorRva = static_cast<DWORD>(TypeDescriptor - ModuleBase);
	DWORD64 ColAddress = 0;

	for (const auto& Sec : GetSections(ModuleBase))
	{
		if (std::strstr(Sec.Name, ".rdata") == nullptr || Sec.Size < 0x30)
			continue;

		DWORD64 Result = ScanChunked(Sec.Base, Sec.Size, 0x30, [&](const BYTE* Data, std::size_t SizeBytes) -> std::size_t
			{
				for (std::size_t i = 0; i + 0x30 <= SizeBytes; i += 8)
				{
					if (reinterpret_cast<const DWORD*>(Data + i)[3] == DescriptorRva)
						return i;
				}
				return std::size_t(-1);
			});

		if (Result)
		{
			ColAddress = Result;
			break;
		}
	}

	if (!ColAddress)
		return 0;

	const DWORD64 ColRef = FindQWordInSections(ModuleBase, ColAddress, IMAGE_SCN_MEM_READ);
	return ColRef ? ColRef + 8 : 0;
}

DWORD64 ProcessManager::FindVTableInstance(DWORD64 ModuleBase, const std::string& ClassName)
{
	const DWORD64 VTable = FindVTable(ModuleBase, ClassName);
	if (!VTable)
		return 0;

	return FindQWordInSections(ModuleBase, VTable, IMAGE_SCN_MEM_READ | IMAGE_SCN_MEM_WRITE);
}
