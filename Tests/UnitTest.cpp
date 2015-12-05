#include "pch.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace Tests
{
    TEST_CLASS(SampleTests)
    {
    public:
        TEST_METHOD(PassingTest)
        {
            Assert::IsTrue(true, L"This test should pass");
        }

        TEST_METHOD(FailingTest)
        {
            Assert::Fail(L"This test should fail");
        }
    };
}