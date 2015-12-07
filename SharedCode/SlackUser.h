#pragma once

namespace SlackDataObjects
{
    typedef Windows::Foundation::Collections::IVector<Windows::Foundation::Uri^> UrlList;
    /// <summary>
    /// Rich class representing the basic user object from the Slack service
    /// Details: https://api.slack.com/types/user
    /// </summary>
    public ref class SlackUser sealed
    {
    public:
        property Platform::String^ Id { Platform::String^ get(); }
        property Platform::String^ RealName { Platform::String^ get(); }
        property Platform::String^ Name { Platform::String^ get(); }
        property Platform::String^ Title { Platform::String^ get(); }

        /// <summary>
        /// Ordered list of images -- from the smallest available to the largest
        /// </summary>
        property UrlList^ Images { UrlList^ get(); }        // Extended properties -- may not be present
        property Platform::String^ Email { Platform::String^ get(); }
        property Platform::String^ TimeZoneName { Platform::String^ get(); }
        property Windows::UI::Color Color { Windows::UI::Color get(); }

    internal:
        /// <summary>
        /// Parses a user object from supplied data. Parses both "core"
        /// and, if present, extended data
        /// </summary>
        static SlackUser^ FromJson(Windows::Data::Json::JsonObject^ json);
    private:
        SlackUser(Platform::String^ id,
                  Platform::String^ realName,
                  Platform::String^ name,
                  Platform::String^ title,
                  UrlList^ images);

        Platform::String^ _id;
        Platform::String^ _realName;
        Platform::String^ _name;
        Platform::String^ _title;
        UrlList^ _images;
        Platform::String^ _email;
        Platform::String^ _timeZoneName;
        Windows::UI::Color _color;

        static Windows::UI::Color ParseColorString(Platform::String^ colorString);
    };
}
