std::vector<IVehicle*> aCatchupCheatCars;

auto GetCatchupCheatOrig = (float(__thiscall*)(ICheater*))0x409390;
float __thiscall GetCatchupCheatHooked(ICheater* pThis) {
	auto value = GetCatchupCheatOrig(pThis);
	auto iveh = pThis->mCOMObject->Find<IVehicle>();
	for (auto& car : aCatchupCheatCars) {
		if (iveh == car) {
			//AddLogPopup(std::format("making {:X} ({}) go fast", (uintptr_t)iveh, iveh->GetVehicleName()));
			value = 2;
		}
	}
	return value;
}

ChloeHook Hook_CatchupCheat([](){
	NyaHookLib::Patch(0x8925C8, &GetCatchupCheatHooked);
});