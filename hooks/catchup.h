std::vector<IVehicle*> aCatchupCheatCars;

auto GetCatchupCheatOrig = (float(__thiscall*)(ICheater*))0x409390;
float __thiscall GetCatchupCheatHooked(ICheater* pThis) {
	auto value = GetCatchupCheatOrig(pThis);
	for (auto& car : aCatchupCheatCars) {
		if (pThis->mCOMObject->Find<IVehicle>() == car) {
			value = 10;
		}
	}
	return value;
}

ChloeHook Hook_CatchupCheat([](){
	NyaHookLib::Patch(0x8925C8, &GetCatchupCheatHooked);
});