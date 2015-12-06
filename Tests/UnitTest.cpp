#include "pch.h"
#include "CppUnitTest.h"
#include "UserListRequest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace Platform;
using namespace Windows::Foundation;

#define SLACK_API_TOKEN L"REDACTED"

namespace Tests
{
    TEST_CLASS(RequestTests)
    {
    public:
        TEST_METHOD(CanConstructUserListRequeset)
        {
            auto req = ref new Requests::UserListRequest(SLACK_API_TOKEN);
            Assert::IsNotNull(req, "Couldn't construct user request");
        }

        TEST_METHOD(CanSuccessfullyMakeRequestAndGetResult)
        {
            auto req = ref new Requests::UserListRequest(SLACK_API_TOKEN);
            auto resultOperation = req->GetResultAsync();
            auto result = resultOperation.get();

            Assert::IsTrue(result->IsSuccessful, L"Request was not successful");
            Assert::AreEqual("TEMPORARY_DATA", result->Result, L"Incorrect data returned");
        }

        TEST_METHOD(RequestReportsHttpErrorWhenBadUrlUsed)
        {
            auto req = ref new Requests::UserListRequest(SLACK_API_TOKEN, ref new Uri("https://slack.com/methods/NOT_REAL"));
            auto resultOperation = req->GetResultAsync();
            auto result = resultOperation.get();

            Assert::IsFalse(result->IsSuccessful, L"Request should not have been successful");
            Assert::IsTrue(Requests::ApiResultStatus::HttpError == result->ApiStatus, L"Incorrect Api Status");
        }

        TEST_METHOD(RequestReportsHttpErrorWhenBadHostIsUsed)
        {
            auto req = ref new Requests::UserListRequest(SLACK_API_TOKEN, ref new Uri("https://a/methods/NOT_REAL"));
            auto resultOperation = req->GetResultAsync();
            auto result = resultOperation.get();

            Assert::IsFalse(result->IsSuccessful, L"Request should not have been successful");
            Assert::IsTrue(Requests::ApiResultStatus::HttpError == result->ApiStatus, L"Incorrect Api Status");
        }
    };
}