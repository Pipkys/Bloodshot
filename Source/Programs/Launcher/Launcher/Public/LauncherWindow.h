#pragma once

namespace Bloodshot::Launcher
{
	class ILauncherWindow
	{
		friend class FLauncher;

	public:
		ILauncherWindow() {}
		ILauncherWindow(const ILauncherWindow&) = delete;
		ILauncherWindow(ILauncherWindow&&) = delete;
		ILauncherWindow& operator=(const ILauncherWindow&) = delete;
		ILauncherWindow& operator=(ILauncherWindow&&) = delete;
		virtual ~ILauncherWindow() {}

	protected:
		virtual void Draw() = 0;
	};
}
