#pragma once

#include "Core.h"

namespace Bloodshot
{
	class IEditorWindow
	{
		friend class FEditor;

	public:
		IEditorWindow() {}
		IEditorWindow(const IEditorWindow&) = delete;
		IEditorWindow(IEditorWindow&&) = delete;
		IEditorWindow& operator=(const IEditorWindow&) = delete;
		IEditorWindow& operator=(IEditorWindow&&) = delete;
		virtual ~IEditorWindow() {}

	protected:
		bool bOpen = false;
		bool bFocused = false;

		NODISCARD FORCEINLINE bool IsOpen() const
		{
			return bOpen;
		}

		NODISCARD FORCEINLINE bool IsInputAllowed() const
		{
			// BSTODO: Implement ?
			return true;
		}

		FORCEINLINE void Open()
		{
			bOpen = true;
		}

		FORCEINLINE void Close()
		{
			bOpen = false;
		}

		virtual void Input() = 0;
		virtual void Tick() = 0;
		virtual void Render() = 0;
	};
}
