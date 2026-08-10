bool bForcePlayerNOS = false;
double fForcePlayerNoNOS = 0;

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

ChloeHook Hook_Input([]() {
	NyaHookLib::Patch(0x8AC678, &GetControlsHooked); // race
	NyaHookLib::Patch(0x8AC710, &GetControlsHooked); // drag
});