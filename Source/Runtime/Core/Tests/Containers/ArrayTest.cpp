#ifdef BS_WITH_AUTOMATION_TESTS

#pragma warning(push)
#pragma warning(disable: 26800) // Use of a moved from object: "Array"

#include "Containers/Array.h"
#include "Misc/AutomationTest.h"
#include "TestToken.h"

namespace Bloodshot::Test
{
	TEST_CASE_NAMED(FArrayDefaultConstructorTest)
	{
		FTestToken::Reset();
		TArray<FTestToken> Array;
		TEST_ASSERT(!Array.GetData());
		TEST_ASSERT(!FTestToken::GetConstructionCalls());
	}

	TEST_CASE_NAMED(FArrayCopyConstructorTest)
	{
		TArray<FTestToken> Array = {0, 1, 2, 3, 4};
		FTestToken::Reset();
		TArray<FTestToken> Copied = Array;
		TEST_ASSERT(Array.GetData() != Copied.GetData());
		TEST_ASSERT(Array.GetSize() == Copied.GetSize());
		TEST_ASSERT(Array.GetCapacity() == Copied.GetCapacity());
		TEST_ASSERT(!FTestToken::GetMoveCalls());
		TEST_ASSERT(FTestToken::GetCopyConstructorCalls() == Copied.GetSize());
		TEST_ASSERT(FTestToken::GetConstructionCalls() == FTestToken::GetCopyConstructorCalls());
	}

	TEST_CASE_NAMED(FArrayMoveConstructorTest)
	{
		TArray<FTestToken> Array = {0, 1, 2, 3, 4};
		FTestToken::Reset();
		TArray<FTestToken> Moved = std::move(Array);
		TEST_ASSERT(Moved.GetSize() == 5);
		TEST_ASSERT(Moved.GetCapacity() == 5);
		TEST_ASSERT(Array.IsEmpty());
		TEST_ASSERT(!Array.GetData());
		TEST_ASSERT(!Array.GetCapacity());
		TEST_ASSERT(!FTestToken::GetCopyCalls());
		TEST_ASSERT(!FTestToken::GetMoveConstructorCalls());
		TEST_ASSERT(!FTestToken::GetConstructionCalls());
	}

	TEST_CASE_NAMED(FArrayConstructorFromAllocatorTest)
	{
		FTestToken::Reset();
		FDefaultAllocator::ForElementType<FTestToken> Allocator;
		TArray<FTestToken> Array(Allocator);
		TEST_ASSERT(!Array.GetData());
		TEST_ASSERT(!FTestToken::GetConstructionCalls());
	}

	TEST_CASE_NAMED(FArrayConstructorFromInitializerListTest)
	{
		FTestToken::Reset();
		TArray<FTestToken> Array = {50, 100, 0, 2, 3};
		TEST_ASSERT(Array.GetSize() == 5);
		TEST_ASSERT(Array.GetCapacity() == 5);
		TEST_ASSERT(FTestToken::GetConstructorCalls() == 5);
		TEST_ASSERT(FTestToken::GetCopyConstructorCalls() == 5);
	}

	TEST_CASE_NAMED(FArrayAssignOfInitializerListWhileEmptyTest)
	{
		FTestToken::Reset();
		TArray<FTestToken> Array;
		Array = {0, 1, 2, 3, 4};
		TEST_ASSERT(FTestToken::GetConstructorCalls() == 5);
		TEST_ASSERT(FTestToken::GetCopyConstructorCalls() == 5);
	}

	TEST_CASE_NAMED(FArrayAssignOfInitializerListWhileNotEmptyTest)
	{
		TArray<FTestToken> Array = {11, 12, 13, 14, 15, 16, 17, 18};
		FTestToken::Reset();
		Array = {0, 1, 2, 3, 4};
		TEST_ASSERT(FTestToken::GetDestructionCalls() == 8 + 5);
		TEST_ASSERT(FTestToken::GetConstructorCalls() == 5);
		TEST_ASSERT(FTestToken::GetCopyConstructorCalls() == 5);
	}

	TEST_CASE_NAMED(FArrayClearWhileEmptyTest)
	{
		TArray<FTestToken> Array;
		TEST_ASSERT(!Array.GetSize());
		Array.Clear();
		TEST_ASSERT(!Array.GetSize());
	}

	TEST_CASE_NAMED(FArrayClearWhileNotEmptyTest)
	{
		TArray<FTestToken> Array = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
		FTestToken::Reset();
		Array.Clear();
		TEST_ASSERT(FTestToken::GetDestructionCalls() == 10);
		TEST_ASSERT(Array.GetData());
		TEST_ASSERT(Array.GetCapacity() == 10);
	}

	TEST_CASE_NAMED(FArrayDisposeWhileEmptyTest)
	{
		TArray<FTestToken> Array;
		TEST_ASSERT(!Array.GetData());
		Array.Dispose();
		TEST_ASSERT(!Array.GetData());
	}

	TEST_CASE_NAMED(FArrayDisposeWhileNotEmptyTest)
	{
		TArray<FTestToken> Array = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
		FTestToken::Reset();
		Array.Dispose();
		TEST_ASSERT(!Array.GetData());
		TEST_ASSERT(!Array.GetCapacity());
		TEST_ASSERT(!Array.GetSize());
		TEST_ASSERT(FTestToken::GetDestructionCalls() == 10);
	}
}

#pragma warning(pop)

#endif
