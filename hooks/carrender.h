bool CarRender_DontRenderPlayer = false;

float TruncateFloat(float in, int accuracy) {
	in *= accuracy;
	in = (int)in;
	in /= accuracy;
	return in;
}

void ModifyCarRenderMatrix(NyaMat4x4* carMatrix, bool skipPlayerCheck) {
	if (!skipPlayerCheck && CarRender_DontRenderPlayer && TheGameFlowManager.CurrentGameFlowState == GAMEFLOW_STATE_RACING && !IsInLoadingScreen()) {
		if (GetClosestActiveVehicle(RenderToWorldCoords(carMatrix->p)) == GetLocalPlayerVehicle()) {
			// hacky solution!! it works but checking some CarRenderInfo ptr against the player and disabling DrawCars would be way better
			carMatrix->p = {0,0,0};
			return;
		}
	}
}

float fCarWorldX = 90.0;
float fCarWorldY = 0.0;
float fCarWorldZ = 90.0;
void ModifyCarWorldMatrix(NyaMat4x4& mat) {
	UMath::Matrix4 rotate_temp;
	rotate_temp.Rotate({fCarWorldX * 0.01745329, fCarWorldY * 0.01745329, fCarWorldZ * 0.01745329});
	mat = (UMath::Matrix4)(mat * rotate_temp);

	mat = WorldToRenderMatrix(mat);
	ModifyCarRenderMatrix(&mat, true);
	mat.x *= CarScaleMatrix.x.x;
	mat.y *= CarScaleMatrix.y.y;
	mat.z *= CarScaleMatrix.z.z;
	mat = RenderToWorldMatrix(mat);

	rotate_temp = rotate_temp.Invert();
	mat = (UMath::Matrix4)(mat * rotate_temp);
}

auto CarGetVisibleStateOrig = (int(__thiscall*)(eView*, const bVector3*, const bVector3*, bMatrix4*))nullptr;
int __thiscall CarGetVisibleStateHooked(eView* a1, const bVector3* a2, const bVector3* a3, bMatrix4* a4) {
	ModifyCarRenderMatrix((NyaMat4x4*)a4, false);
	return CarGetVisibleStateOrig(a1, a2, a3, a4);
}

ChloeHook Hook_CarRender([]() {
	CarGetVisibleStateOrig = (int(__thiscall*)(eView*, const bVector3*, const bVector3*, bMatrix4*))NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x74E346, &CarGetVisibleStateHooked);
	NyaHookLib::PatchRelative(NyaHookLib::JMP, 0x73786A, 0x73790A); // don't reset CarScaleMatrix when exiting to menu
	NyaHooks::LateInitHook::aFunctions.push_back([]() { CarScaleMatrix = UMath::Matrix4::kIdentity; });
});