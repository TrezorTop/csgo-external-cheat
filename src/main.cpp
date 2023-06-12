#include <iostream>
#include <thread>

#include "memory/memory.h"
#include "offset.h"

struct Vector2 {
	float x;
	float y;
};

int main() {
	Memory memory = Memory("csgo.exe");

	const uintptr_t client = memory.GetModuleAddress("client.dll");
	const uintptr_t engine = memory.GetModuleAddress("engine.dll");
	const uintptr_t clientState = memory.Read<uintptr_t>(engine + offset::dwClientState);
	const uintptr_t localPlayer = memory.Read<uintptr_t>(client + offset::dwLocalPlayer);

	Vector2 oldAimPunch = Vector2{
		.x = .0f,
		.y = .0f,
	};

	while (true) {
		const uintptr_t shotsFired = memory.Read<int32_t>(localPlayer + offset::m_iShotsFired);
		const Vector2 viewAngles = memory.Read<Vector2>(clientState + offset::dwClientState_ViewAngles);

		if (shotsFired) {
			const Vector2 aimPunch = memory.Read<Vector2>(localPlayer + offset::m_aimPunchAngle);

			Vector2 newAngles = Vector2{
				.x = viewAngles.x + oldAimPunch.x - aimPunch.x * 2.0f,
				.y = viewAngles.y + oldAimPunch.y - aimPunch.y * 2.0f,
			};

			while (newAngles.y > 180.0f) {
				newAngles.y -= 360.0f;
			}

			while (newAngles.y < 180.0f) {
				newAngles.y += 360.0f;
			}

			if (newAngles.x > 89.0f) {
				newAngles.x = 89.0f;
			}

			if (newAngles.x < -89.0f) {
				newAngles.x = -89.0f;
			}


			if (shotsFired > 1) {
				memory.Write<Vector2>(clientState + offset::dwClientState_ViewAngles, newAngles);
			}

			oldAimPunch.x = aimPunch.x * 2.0f;
			oldAimPunch.y = aimPunch.y * 2.0f;
		}
		else {
			oldAimPunch.x = .0f;
			oldAimPunch.y = .0f;
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}

	return 0;
}