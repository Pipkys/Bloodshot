#pragma once

#include "Core.h"

#include "LauncherWindow.h"

namespace Bloodshot::Launcher
{
	class FOpenProjectWindow final : public ILauncherWindow
	{
	public:
		NODISCARD static FOpenProjectWindow& GetInstance();

	private:
		FORCEINLINE FOpenProjectWindow()
		{
			Setup();
		}

		virtual void Draw() override;

		void Setup();
	};
}
