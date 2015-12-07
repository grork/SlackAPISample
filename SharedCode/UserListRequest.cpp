#include "pch.h"
#include "UserListRequest.h"

using namespace concurrency;
using namespace Platform;
using namespace Requests;
using namespace Windows::Data::Json;
using namespace Windows::Foundation;
using namespace Windows::Storage;
using namespace Windows::Storage::Streams;
using namespace Windows::Web::Http;

const wchar_t* UserListRequest::BASE_URL = L"https://slack.com/api/users.list";
const wchar_t* UserListRequest::LOCAL_CACHE_FILE_NAME = L"slackcache.json";

UserListRequest::UserListRequest(String^ apiToken)
{
    this->_apiToken = apiToken;
}

UserListRequest::UserListRequest(String^ apiToken, Uri^ overrideUrl) : UserListRequest(apiToken)
{
    this->_urlOverride = overrideUrl;
}

UserListRequest::~UserListRequest()
{
    // Cranky CX compiler issue when using delegating constructors
}

Uri^ UserListRequest::_GetRequestUrl()
{
    if (this->_urlOverride)
    {
        return this->_urlOverride;
    }

    auto uri = ref new Uri(StringReference(UserListRequest::BASE_URL));
    uri = uri->CombineUri(L"?token=" + this->_apiToken);

    return uri;
}

task<UserListResult^> UserListRequest::GetResultAsync()
{
    auto httpClient = ref new HttpClient();
    auto self = this;

    auto resultTask = create_task(httpClient->GetAsync(this->_GetRequestUrl())).then([self](task<HttpResponseMessage^> completed_task)
    {
        // This handles the response as a task, rather than the raw response, because
        // when offline (e.g. no internet) this continuation will barf. Note that offline
        // is merely one of the ways it could barf; generally speaking "real network issue"
        // covers these.
        HttpResponseMessage^ response;
        bool requestWasSuccessful = false;
        try
        {
            response = completed_task.get();
            requestWasSuccessful = response->IsSuccessStatusCode;
        }
        catch (...)
        {
            requestWasSuccessful = false;
        }

        task<String^> resultRawDataTask;
        bool loadedDataFromOnDiskCache = false;

        if (!requestWasSuccessful)
        {
            // Try loading the result form disk if we're in an error state to return
            // any cached data that we might have.
            resultRawDataTask = self->_LoadResponseFromDisk();
            loadedDataFromOnDiskCache = true;
        }
        else
        {
            self->_WriteResponseToDisk(response->Content);
            resultRawDataTask = create_task(response->Content->ReadAsStringAsync());
        }

        auto readStringTask = resultRawDataTask.then([requestWasSuccessful, loadedDataFromOnDiskCache](String^ data)
        {
            ApiResultStatus apiResult = ApiResultStatus::Success;
            JsonObject^ parsedData;

            if (!requestWasSuccessful)
            {
                apiResult = ApiResultStatus::HttpError;
            }

            if (!data->IsEmpty())
            {
                bool didParse = JsonObject::TryParse(data, &parsedData);
                if (!didParse)
                {
                    apiResult = ApiResultStatus::BadPayload;
                    data = nullptr;
                }
                else
                {
                    apiResult = UserListRequest::_GetResultStatusFromJson(parsedData);
                    if (apiResult != ApiResultStatus::Success)
                    {
                        data = nullptr;
                    }
                }
            }

            return ref new UserListResult(apiResult, data, loadedDataFromOnDiskCache);
        });

        return readStringTask;
    });

    return resultTask;
}

ApiResultStatus UserListRequest::_GetResultStatusFromJson(JsonObject^ json)
{
    bool wasSuccessful = json->GetNamedBoolean(L"ok", false);

    if (wasSuccessful)
    {
        return ApiResultStatus::Success;
    }

    String^ errorData = json->GetNamedString("error");

    if (errorData == StringReference(L"not_authed"))
    {
        return ApiResultStatus::NotAuthed;
    }
    else if (errorData == StringReference(L"invalid_auth"))
    {
        return ApiResultStatus::InvalidAuth;
    }
    else if (errorData == StringReference(L"account_inactive"))
    {
        return ApiResultStatus::AccountInactive;
    }
    
    return ApiResultStatus::Unknown;
}

task<String^> UserListRequest::_LoadResponseFromDisk()
{
    StorageFolder^ localAppData = ApplicationData::Current->LocalCacheFolder;
    auto getFile = localAppData->TryGetItemAsync(StringReference(UserListRequest::LOCAL_CACHE_FILE_NAME));

    return create_task(getFile).then([](IStorageItem^ item)
    {
        // since we used the API that doesn't barf loudly when the file is missing
        // we need to check if what was returned was actually a file, because, it
        // checks for folders AND files. We only really care about files here.
        StorageFile^ file = dynamic_cast<StorageFile^>(item);
        if (file == nullptr)
        {
            // No file, no result
            return task_from_result(ref new String(L""));
        }

        return create_task(FileIO::ReadTextAsync(file));
    });
}

void UserListRequest::_WriteResponseToDisk(IHttpContent^ content)
{
    StorageFolder^ localAppData = ApplicationData::Current->LocalCacheFolder;
    auto createFile = localAppData->CreateFileAsync(StringReference(UserListRequest::LOCAL_CACHE_FILE_NAME),
                                                                   CreationCollisionOption::ReplaceExisting);

    create_task(createFile).then([](StorageFile^ file)
    {
        return create_task(file->OpenAsync(FileAccessMode::ReadWrite));
    }).then([content](IOutputStream^ outputStream)
    {
        return create_task(content->WriteToStreamAsync(outputStream));
    }).then([](task<unsigned long long> writeComplete)
    {
        try
        {
            writeComplete.wait();
        }
        catch (...)
        {
            __debugbreak();
            OutputDebugString(L"Failed to to write data to disk; no offline data will be available");
        }

        OutputDebugString(L"Written all data to disk");
    });
}


#pragma region UserListResult
UserListResult::UserListResult(ApiResultStatus apiStatus, String^ cachedResult, bool wasSatisifiedFromCache)
    : _apiStatusCode(apiStatus), _data(cachedResult), _wasSatisifiedFromCache(wasSatisifiedFromCache)
{ }

String^ UserListResult::Result::get()
{
    return this->_data;
}

bool UserListResult::IsSuccessful::get()
{
    return (ApiResultStatus::Success == this->ApiStatus);
}

bool UserListResult::HasResult::get()
{
    return !this->_data->IsEmpty();
}

ApiResultStatus UserListResult::ApiStatus::get()
{
    return this->_apiStatusCode;
}

bool UserListResult::WasSatisfiedFromCache::get()
{
    return this->_wasSatisifiedFromCache;
}
#pragma endregion UserListResult