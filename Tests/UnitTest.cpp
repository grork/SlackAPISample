#include "pch.h"
#include "CppUnitTest.h"
#include "UserListRequest.h"

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

        TEST_METHOD(CanReferenceSharedCode)
        {
            auto req = ref new Requests::UserListRequest();
            Assert::IsNotNull(req, "Couldn't construct user request");
        }
    };
}