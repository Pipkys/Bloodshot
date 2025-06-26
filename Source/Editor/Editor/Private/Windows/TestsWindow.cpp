#include "Windows/TestsWindow.h"
#include "EditorConfig.h"
#include "RHI.h"
#include "Translator.h"

namespace Bloodshot
{
	FTestsWindow::FTestsWindow()
		: TestImage(FRHI::CreateTexture(BuildResourcePath("Editor/TestsWindow/Icons/TestIcon.png")))
	{
	}

	FTestsWindow::~FTestsWindow()
	{
		FRHI::DestroyTexture(TestImage);
	}

	void FTestsWindow::Input()
	{
	}

	void FTestsWindow::Tick()
	{
	}

	void FTestsWindow::Render()
	{
#ifdef BS_WITH_AUTOMATION_TESTS
		if (ImGui::Ext::Begin(TRANSLATE("Automation Tests"), bFocused))
		{
			TMap<FStringView, IAutomationTest*>& AutomationTests = FAutomationTestFramework::GetTests();
			for (TPair<FStringView, IAutomationTest*>& TestPair : AutomationTests)
			{
				FStringView TestName = TestPair.First;
				IAutomationTest* const Test = TestPair.Second;
				const size_t ErrorCount = Test->GetErrorCount();
				const bool bRunnedOnce = Test->IsRunnedOnce();

				const ImVec4& ImageColor = bRunnedOnce
					? ErrorCount ? ImVec4(0.9f, 0.0f, 0.1f, 0.9f) : ImVec4(0.f, 0.9f, 0.1f, 0.9f)
					: ImVec4(1.f, 1.f, 1.f, 0.9f);

				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.f, 0.f, 0.f, 0.f));
				if (ImGui::Ext::ImageTextButton(TestImage->GetUniqueID(),
					TestName.GetData(),
					ImVec2(16.f, 16.f),
					ImVec2(0.f, 0.f),
					ImVec2(1.f, 1.f),
					-1,
					ImVec4(0.f, 0.f, 0.f, 0.f),
					ImageColor))
				{
					FAutomationTestFramework::RunTest(TestName);
				}
				ImGui::PopStyleColor();

				if (bRunnedOnce)
				{
					const float Duration = Test->GetDuration();
					if (ErrorCount)
					{
						FString Formatted = String::Format("Result: {} Errors\nDuration: {} ms\tErrors:\n", ErrorCount, Duration);
						for (const FString& Error : Test->GetErrors())
						{
							Formatted += "\t";
							Formatted += Error;
							Formatted += "\n";
						}
						Formatted.RemoveLast();
						ImGui::SetItemTooltip(Formatted.GetData());
					}
					else
					{
						const FString& Formatted = String::Format("Result: Success\nDuration: {} ms", Duration);
						ImGui::SetItemTooltip(Formatted.GetData());
					}
				}
				else
				{
					ImGui::SetItemTooltip("Not runned yet");
				}
			}
		}
		ImGui::End();
#else
		ImGui::Text("Engine compiled without automation tests");
#endif
	}
}
