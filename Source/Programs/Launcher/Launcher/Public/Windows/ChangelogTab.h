#pragma once

#include "Core.h"

#include "LauncherWindow.h"

namespace Bloodshot::Launcher
{
	class FChangelogTab final : public ILauncherWindow
	{
		friend class FMainWindow;

	public:
		NODISCARD static FChangelogTab& GetInstance();

	private:
		FORCEINLINE FChangelogTab()
		{
			Setup();
		}

		virtual void Draw() override;

		void Setup();
	};
}