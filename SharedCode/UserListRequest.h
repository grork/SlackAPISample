#pragma once

#include <pch.h>

namespace Requests {

    ref class UserListRequest;

    /// <summary>
    /// Represents the different "expected" categories of errors we might get back
    /// from calling the API.
    /// </summary>
    enum class ApiResultStatus
    {
        None,
        Success,
        BadPayload,
        NotAuthed,
        InvalidAuth,
        AccountInactive,
        HttpError,
        Unknown,
    };

    /// <summary>
    /// Wrapper class to create structured result around the
    /// result of calling the User list API. Helps with error
    /// handling, to avoid exception based error handling for
    /// these semi "expected" results.
    /// </summary>
    ref class UserListResult sealed
    {
        friend ref class UserListRequest;
    internal:
        /// <summary>
        /// Constructs a result with the explicitly supplied Api status
        /// </summary>
        UserListResult(ApiResultStatus apiStatus, Platform::String^ result, bool wasSatisfiedFromCache = false);

        property ApiResultStatus ApiStatus { ApiResultStatus get(); }
        property bool IsSuccessful { bool get(); }
        property bool HasResult { bool get(); }
        property bool WasSatisfiedFromCache { bool get(); }

        /// <summary>
        /// Returns the data retrieved by this request.
        ///
        /// If the result was not successful, this will throw an exception
        /// </summary>
        property Platform::String^ Result { Platform::String^ get(); }

    private:
        ApiResultStatus _apiStatusCode;
        Platform::String^ _data;
        bool _wasSatisifiedFromCache = false;
    };

    /// <summary>
    /// Requests the users from Slack using the supplied API Key
    /// </summary>
    ref class UserListRequest sealed
    {
    public:
        virtual ~UserListRequest(); // Cranky CX compiler issue when using delegating constructors
    internal:
        UserListRequest(Platform::String^ apiToken);

        /// <summary>
        /// "test" constructor to allow the caller to specify an overloaded URL that
        /// will be used instead of the computed URL.
        /// </summary>
        UserListRequest(Platform::String^ apiToken, Windows::Foundation::Uri^ overrideUrl);
        concurrency::task<UserListResult^> GetResultAsync();

    private:
        Windows::Foundation::Uri^ _urlOverride;
        Platform::String^ _apiToken;
        Windows::Foundation::Uri^ _GetRequestUrl();
        void _WriteResponseToDisk(Windows::Web::Http::IHttpContent^ content);
        concurrency::task<Platform::String^> _LoadResponseFromDisk();
        static ApiResultStatus _GetResultStatusFromJson(Windows::Data::Json::JsonObject^ json);

        static const wchar_t* BASE_URL;
        static const wchar_t* LOCAL_CACHE_FILE_NAME;
    };
}