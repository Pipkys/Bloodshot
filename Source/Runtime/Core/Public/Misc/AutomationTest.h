#pragma once

#ifdef BS_WITH_AUTOMATION_TESTS

#include "Containers/Map.h"
#include "Containers/String.h"
#include "Containers/StringView.h"
#include "Logging/LoggingMacros.h"
#include "Platform/Platform.h"
#include "String/Format.h"

namespace Bloodshot
{
	class IAutomationTest;

	class FAutomationTestFramework final
	{
		friend class IAutomationTest;

	public:
		NODISCARD static FAutomationTestFramework& GetInstance();

		NODISCARD FORCEINLINE static IAutomationTest* GetCurrentTest()
		{
			return GetInstance().CurrentTest;
		}

		NODISCARD FORCEINLINE static TMap<FStringView, IAutomationTest*>& GetTests()
		{
			return GetInstance().AutomationTests;
		}

		static void RunTest(FStringView Name);
		static void RunAllTests();

	private:
		FAutomationTestFramework() {}

		FORCEINLINE static void RegisterTest(FStringView InName, IAutomationTest* const InTest)
		{
			GetInstance().AutomationTests.Emplace(InName, InTest);
		}

		FORCEINLINE static void UnregisterTest(FStringView InName)
		{
			GetInstance().AutomationTests.Remove(InName);
		}

		TMap<FStringView, IAutomationTest*> AutomationTests;
		IAutomationTest* CurrentTest = nullptr;
	};

	class IAutomationTest
	{
		friend class FAutomationTestFramework;

	public:
		IAutomationTest(FStringView InName)
			: Name(InName.GetData())
		{
			FAutomationTestFramework::RegisterTest(Name, this);
		}

		virtual ~IAutomationTest()
		{
			FAutomationTestFramework::UnregisterTest(Name);
		}

		FORCEINLINE void AddError(FStringView ConditionText, FStringView ErrorText)
		{
			FString Formatted = String::Format("{}, {} - {}", Name, ConditionText, ErrorText);
			BS_LOG(Error, Formatted.GetData());
			Errors.EmplaceBack(std::move(Formatted));
		}

		NODISCARD FORCEINLINE float GetDuration() const
		{
			return Duration;
		}

		NODISCARD FORCEINLINE bool IsRunnedOnce() const
		{
			return bRunnedOnce;
		}

		NODISCARD FORCEINLINE size_t GetErrorCount() const
		{
			return Errors.GetSize();
		}

		NODISCARD FORCEINLINE const TArray<FString>& GetErrors() const
		{
			return Errors;
		}

	protected:
		FStringView Name;
		float Duration = 0.f;
		bool bRunnedOnce = false;
		TArray<FString> Errors;

		virtual void Run() = 0;
	};
}

#define IMPLEMENT_AUTOMATION_TEST(Class, StrName) \
class Class final : public IAutomationTest \
{ \
	public: Class(FStringView InName) : IAutomationTest(InName) {}\
	private: virtual void Run() override { Body(); } \
	void Body(); \
}; \
namespace \
{ \
	Class Class##Instance(StrName); \
} \
void Class::Body()

#define TEST_CASE_NAMED(ClassName) IMPLEMENT_AUTOMATION_TEST(ClassName, #ClassName)

#define TEST_ASSERT(Expression) if(!(Expression)) { FAutomationTestFramework::GetCurrentTest()->AddError(#Expression, "Condition failed"); }

#endif
