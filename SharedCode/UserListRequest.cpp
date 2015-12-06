#include "pch.h"
#include "UserListRequest.h"

using namespace concurrency;
using namespace Platform;
using namespace Requests;
using namespace Windows::Foundation;
using namespace Windows::Web::Http;

const wchar_t* UserListRequest::BASE_URL = L"https://slack.com/api/users.list";

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
    auto resultTask = create_task(httpClient->GetAsync(this->_GetRequestUrl())).then([](task<HttpResponseMessage^> completed_task) {
        // This handles the response as a task, rather than the raw response, because
        // when offline (e.g. no internet) this continuation will barf. Note that offline
        // is merely one of the ways it could barf; generally speaking "real network issue"
        // covers these.
        HttpResponseMessage^ response;
        bool wasSuccessful = false;
        try
        {
            response = completed_task.get();
            wasSuccessful = response->IsSuccessStatusCode;
        }
        catch (...)
        {
            wasSuccessful = false;
        }

        if (!wasSuccessful)
        {
            return ref new UserListResult(ApiResultStatus::HttpError);
        }

        auto result = ref new UserListResult(L"TEMPORARY_DATA");
        return result;
    });

    return resultTask;
}


#pragma region UserListResult
UserListResult::UserListResult(String^ result) : _data(result), _apiStatusCode(ApiResultStatus::Success)
{ }

UserListResult::UserListResult(ApiResultStatus apiStatus)
    : _apiStatusCode(apiStatus)
{ }

String^ UserListResult::Result::get()
{
    if (!this->IsSuccessful)
    {
        throw ref new InvalidArgumentException(L"Can't get data from result if not successful");
    }

    return this->_data;
}

bool UserListResult::IsSuccessful::get()
{
    return (ApiResultStatus::Success == this->ApiStatus);
}

ApiResultStatus UserListResult::ApiStatus::get()
{
    return this->_apiStatusCode;
}
#pragma endregion UserListResult