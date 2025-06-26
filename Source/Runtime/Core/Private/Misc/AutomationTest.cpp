#ifdef BS_WITH_AUTOMATION_TESTS

#include "Misc/Timer.h"
#include "Misc/AutomationTest.h"

namespace Bloodshot
{
	FAutomationTestFramework& FAutomationTestFramework::GetInstance()
	{
		static FAutomationTestFramework Instance;
		return Instance;
	}

	void FAutomationTestFramework::RunTest(FStringView Name)
	{
		FAutomationTestFramework& Instance = GetInstance();
		IAutomationTest*& CurrentTest = Instance.CurrentTest;
		IAutomationTest* const Test = Instance.AutomationTests[Name];
		CurrentTest = Test;
		Test->Errors.Clear();
		FTimer Timer;
		Test->Run();
		Test->Duration = (float)Timer.GetElapsedMilliseconds().count();
		Test->bRunnedOnce = true;
		CurrentTest = nullptr;
	}

	void FAutomationTestFramework::RunAllTests()
	{
		FAutomationTestFramework& Instance = GetInstance();
		TMap<FStringView, IAutomationTest*>& AutomationTests = Instance.AutomationTests;
		IAutomationTest*& CurrentTest = Instance.CurrentTest;
		for (const TPair<FStringView, IAutomationTest*>& AutomationTestPair : AutomationTests)
		{
			IAutomationTest* const Test = AutomationTestPair.Second;
			CurrentTest = Test;
			Test->Errors.Clear();
			FTimer Timer;
			Test->Run();
			Test->Duration = (float)Timer.GetElapsedMilliseconds().count();
			Test->bRunnedOnce = true;
			CurrentTest = nullptr;
		}
	}
}

#endif
