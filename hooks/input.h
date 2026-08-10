bool bForcePlayerNOS = false;
double fForcePlayerNoNOS = 0;
std::vector<IVehicle*> aForceNOSCars;

auto GetControls_orig = (InputControls*(__thiscall*)(IInputPlayer*))0x69CBB0;
InputControls* __thiscall GetControlsHooked(IInputPlayer* pThis) {
	static CNyaTimer gTimer;
	gTimer.Process();

	auto orig = GetControls_orig(pThis);
	static auto tmp = *orig;
	tmp = *orig;
	if (bForcePlayerNOS) {
		tmp.fNOS = true;
	}
	if (fForcePlayerNoNOS > 0.0) {
		fForcePlayerNoNOS -= gTimer.fDeltaTime;
		tmp.fNOS = false;
	}
	return &tmp;
}

InputControls* __thiscall GetOpponentControlsHooked(IInputPlayer* pThis) {
	static CNyaTimer gTimer;
	gTimer.Process();

	auto orig = GetControls_orig(pThis);
	static auto tmp = *orig;
	tmp = *orig;
	auto iveh = pThis->mCOMObject->Find<IVehicle>();
	for (auto& car : aForceNOSCars) {
		if (iveh == car) {
			AddLogPopup(std::format("making {:X} ({}) nos", (uintptr_t)iveh, iveh->GetVehicleName()));
			tmp.fNOS = true;
		}
	}
	return &tmp;
}

ChloeHook Hook_Input([]() {
	NyaHookLib::Patch(0x8AC678, &GetControlsHooked); // race
	NyaHookLib::Patch(0x8AC710, &GetControlsHooked); // drag
	NyaHookLib::Patch(0x8AB5D0, &GetOpponentControlsHooked); // racer
});