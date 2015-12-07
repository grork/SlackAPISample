#include "pch.h"
#include "CppUnitTest.h"
#include "UserListRequest.h"

using namespace concurrency;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Storage;

#define SLACK_API_TOKEN L"REDACTED"
#define LOCAL_CACHE_FILE_NAME L"slackcache.json"

namespace Tests
{
    TEST_CLASS(RequestTests)
    {
    public:

        void CopyTestCacheResultToLocalCache(String^ sourceFileName)
        {
            // We need to "Fake out" the response by placing something known in the
            // internal cache for the request. Slightly "inside the beltway" w.r.t.
            // the internal workings, but not crazy
            auto packageFolder = Windows::ApplicationModel::Package::Current->InstalledLocation;
            auto moveResponseFileToCache = create_task(packageFolder->GetFileAsync(sourceFileName)).then([](StorageFile^ file)
            {
                return file->CopyAsync(ApplicationData::Current->LocalCacheFolder, LOCAL_CACHE_FILE_NAME);
            });

            moveResponseFileToCache.wait();
        }

        TEST_METHOD_INITIALIZE(InitTests)
        {
            StorageFolder^ cacheFolder = ApplicationData::Current->LocalCacheFolder;
            auto fileCleanupTask = create_task(cacheFolder->TryGetItemAsync(LOCAL_CACHE_FILE_NAME)).then([](IStorageItem^ item)
            {
                if (item == nullptr)
                {
                    return create_task(task_completion_event<void>());
                }
                
                return create_task(item->DeleteAsync());
            });

            fileCleanupTask.wait();
        }

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
            Assert::IsTrue(result->HasResult, L"Expected data");
            Assert::IsFalse(result->Result->IsEmpty(), L"Expected some data to be returned");
            Assert::IsFalse(result->WasSatisfiedFromCache, L"Should NOT have been satisified from on disk cache");
        }

        TEST_METHOD(RequestReportsHttpErrorWhenBadUrlUsed)
        {
            auto req = ref new Requests::UserListRequest(SLACK_API_TOKEN, ref new Uri("https://slack.com/methods/NOT_REAL"));
            auto resultOperation = req->GetResultAsync();
            auto result = resultOperation.get();

            Assert::IsFalse(result->IsSuccessful, L"Request should not have been successful");
            Assert::IsFalse(result->HasResult, L"Didn't expect data");
            Assert::IsTrue(Requests::ApiResultStatus::HttpError == result->ApiStatus, L"Incorrect Api Status");
        }

        TEST_METHOD(RequestReportsHttpErrorWhenBadHostIsUsed)
        {
            auto req = ref new Requests::UserListRequest(SLACK_API_TOKEN, ref new Uri("https://a/methods/NOT_REAL"));
            auto resultOperation = req->GetResultAsync();
            auto result = resultOperation.get();

            Assert::IsFalse(result->IsSuccessful, L"Request should not have been successful");
            Assert::IsFalse(result->HasResult, L"Didn't expect data");
            Assert::IsTrue(Requests::ApiResultStatus::HttpError == result->ApiStatus, L"Incorrect Api Status");
        }

        TEST_METHOD(RequestLoadsCachedResponseWhenFailing)
        {
            this->CopyTestCacheResultToLocalCache("SuccessfulPayload.json");

            // This uses a fake URL so it fails and loads from disk
            auto req = ref new Requests::UserListRequest(SLACK_API_TOKEN, ref new Uri("https://a/methods/NOT_REAL"));
            auto resultOperation = req->GetResultAsync();
            auto result = resultOperation.get();

            Assert::IsTrue(result->IsSuccessful, L"Request should have been successful");
            Assert::IsTrue(result->HasResult, L"Expect data");
            Assert::IsFalse(result->Result->IsEmpty(), L"Expected some data");
            Assert::IsTrue(Requests::ApiResultStatus::Success == result->ApiStatus, L"Incorrect Api Status");
            Assert::IsTrue(result->WasSatisfiedFromCache, L"Should have been satisified from on disk cache");
        }

        TEST_METHOD(RequestIndicatesCorrectStatusWhenParsingBadPayload)
        {
            this->CopyTestCacheResultToLocalCache("CorruptPayload.json");

            // This uses a fake URL so it fails and loads from disk
            auto req = ref new Requests::UserListRequest(SLACK_API_TOKEN, ref new Uri("https://a/methods/NOT_REAL"));
            auto resultOperation = req->GetResultAsync();
            auto result = resultOperation.get();

            Assert::IsFalse(result->IsSuccessful, L"Request should not have been successful");
            Assert::IsFalse(result->HasResult, L"Didn't expect data");
            Assert::IsTrue(result->Result->IsEmpty(), L"Didn't expect data");
            Assert::IsTrue(Requests::ApiResultStatus::BadPayload == result->ApiStatus, L"Incorrect Api Status");
        }

        TEST_METHOD(RequestParsesNotAuthedResponse)
        {
            this->CopyTestCacheResultToLocalCache("NotAuthedPayload.json");

            // This uses a fake URL so it fails and loads from disk
            auto req = ref new Requests::UserListRequest(SLACK_API_TOKEN, ref new Uri("https://a/methods/NOT_REAL"));
            auto resultOperation = req->GetResultAsync();
            auto result = resultOperation.get();

            Assert::IsFalse(result->IsSuccessful, L"Request should not have been successful");
            Assert::IsFalse(result->HasResult, L"Didn't expect data");
            Assert::IsTrue(result->Result->IsEmpty(), L"Expected some data");
            Assert::IsTrue(Requests::ApiResultStatus::NotAuthed == result->ApiStatus, L"Incorrect Api Status");
        }

        TEST_METHOD(RequestParsesInvalidAuthResponse)
        {
            this->CopyTestCacheResultToLocalCache("InvalidAuthPayload.json");

            // This uses a fake URL so it fails and loads from disk
            auto req = ref new Requests::UserListRequest(SLACK_API_TOKEN, ref new Uri("https://a/methods/NOT_REAL"));
            auto resultOperation = req->GetResultAsync();
            auto result = resultOperation.get();

            Assert::IsFalse(result->IsSuccessful, L"Request should not have been successful");
            Assert::IsFalse(result->HasResult, L"Didn't expect data");
            Assert::IsTrue(result->Result->IsEmpty(), L"Didn't expect data");
            Assert::IsTrue(Requests::ApiResultStatus::InvalidAuth == result->ApiStatus, L"Incorrect Api Status");
        }

        TEST_METHOD(RequestParsesAccountInactiveResponse)
        {
            this->CopyTestCacheResultToLocalCache("AccountInactivePayload.json");

            // This uses a fake URL so it fails and loads from disk
            auto req = ref new Requests::UserListRequest(SLACK_API_TOKEN, ref new Uri("https://a/methods/NOT_REAL"));
            auto resultOperation = req->GetResultAsync();
            auto result = resultOperation.get();

            Assert::IsFalse(result->IsSuccessful, L"Request should not have been successful");
            Assert::IsFalse(result->HasResult, L"Didn't expect data");
            Assert::IsTrue(result->Result->IsEmpty(), L"Didn't expect data");
            Assert::IsTrue(Requests::ApiResultStatus::AccountInactive == result->ApiStatus, L"Incorrect Api Status");
        }
    };
}