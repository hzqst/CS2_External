#include "globalvars.h"

bool globalvars::UpdateGlobalvars()
{
	DWORD64 m_DglobalVars = 0;
	if (!ProcessMgr.ReadMemory<DWORD64>(gGame.GetGlobalVarsAddress(), m_DglobalVars))
		return false;

	this->address = m_DglobalVars;

	if (!this->GetRealTime())
		return false;
	if (!this->GetFrameCount())
		return false;
	if (!this->GetMaxClients())
		return false;
	if (!this->GetTickCount())
		return false;
	if (!this->GetIntervalPerTick())
		return false;
	if (!this->GetcurrentTime())
		return false;
	if (!this->GetCurrentMapName())
		return false;

	return true;
}

bool globalvars::GetRealTime()
{
	return GetDataAddressWithOffset<float>(this->address, Offset::GlobalVar.RealTime, this->g_fRealTime);
}

bool globalvars::GetFrameCount()
{
	return GetDataAddressWithOffset<int>(this->address, Offset::GlobalVar.FrameCount, this->g_iFrameCount);
}

bool globalvars::GetMaxClients()
{
	return GetDataAddressWithOffset<int>(this->address, Offset::GlobalVar.MaxClients, this->g_iMaxClients);
}

bool globalvars::GetTickCount()
{
	return GetDataAddressWithOffset<int>(this->address, Offset::GlobalVar.TickCount, this->g_iTickCount);
}

bool globalvars::GetIntervalPerTick()
{
	return GetDataAddressWithOffset<float>(this->address, Offset::GlobalVar.IntervalPerTick, this->g_fIntervalPerTick);
}

bool globalvars::GetcurrentTime()
{
	return GetDataAddressWithOffset<float>(this->address, Offset::GlobalVar.CurrentTime, this->g_fCurrentTime);
}

bool globalvars::GetCurrentMapName()
{
	return GetDataAddressWithOffset<char*>(this->address, Offset::GlobalVar.CurrentMapName, this->g_cCurrentMapName);
}