#pragma once

#include "Core.h"

#include "KeyCode.h"
#include "KeyState.h"
#include "RHI.h"

namespace Bloodshot
{
	class FInput final
	{
	public:
		NODISCARD static FInput& GetInstance();

		NODISCARD FORCEINLINE static bool GetKeyDown(const EKeyCode KeyCode)
		{
			return CheckKeyState(KeyCode, EKeyState::Press);
		}

		NODISCARD FORCEINLINE static bool GetKeyUp(const EKeyCode KeyCode)
		{
			return CheckKeyState(KeyCode, EKeyState::Release);
		}

		NODISCARD FORCEINLINE static bool GetMouseButtonDown(const EMouseButton MouseButton)
		{
			return CheckMouseButtonState(MouseButton, EMouseButtonState::Press);
		}

		NODISCARD FORCEINLINE static bool GetMouseButtonUp(const EMouseButton MouseButton)
		{
			return CheckMouseButtonState(MouseButton, EMouseButtonState::Release);
		}

		NODISCARD FORCEINLINE static glm::vec2 GetMousePosition()
		{
			double PositionX;
			double PositionY;
			FRHI::GetMousePosition(PositionX, PositionY);
			return glm::vec2(PositionX, PositionY);
		}

		NODISCARD FORCEINLINE static float GetMousePositionX()
		{
			return GetMousePosition().x;
		}

		NODISCARD FORCEINLINE static float GetMousePositionY()
		{
			return GetMousePosition().y;
		}

		NODISCARD FORCEINLINE static bool IsCursorVisible()
		{
			return GetInstance().bCursorVisible;
		}

		NODISCARD FORCEINLINE static void ShowCursor()
		{
			GetInstance().bCursorVisible = true;
			FRHI::SetMouseVisibility(true);
		}

		NODISCARD FORCEINLINE static void HideCursor()
		{
			GetInstance().bCursorVisible = false;
			FRHI::SetMouseVisibility(false);
		}

	private:
		bool bCursorVisible = true;

		NODISCARD FORCEINLINE static bool CheckKeyState(const EKeyCode KeyCode, const EKeyState KeyState)
		{
			return FRHI::GetKeyState(static_cast<int>(KeyCode)) == static_cast<int>(KeyState);
		}

		NODISCARD FORCEINLINE static bool CheckMouseButtonState(const EMouseButton MouseButton, const EMouseButtonState MouseButtonState)
		{
			return FRHI::GetMouseButtonState(static_cast<int>(MouseButton)) == static_cast<int>(MouseButtonState);
		}
	};
}
