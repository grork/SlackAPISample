#include "pch.h"
#include "SlackUser.h"
#include <locale>
#include <codecvt>

using namespace SlackDataObjects;

using namespace Platform;
using namespace Platform::Collections;
using namespace std;
using namespace Windows::Foundation;
using namespace Windows::Data::Json;
using namespace Windows::UI;

static const std::initializer_list<const wchar_t*> IMAGE_KEYS = { L"image_24", L"image_32", L"image_48", L"image_72", L"image_192", L"image_original" };

SlackUser::SlackUser(String^ id,
                     String^ realName,
                     String^ name,
                     String^ title,
                     UrlList^ images) :
    _id(id),
    _realName(realName),
    _name(name),
    _title(title),
    _images(images),
    _color(Colors::Transparent)
{
}

SlackUser^ SlackUser::FromJson(JsonObject^ json)
{
    auto id = json->GetNamedString("id");
    auto realName = json->GetNamedString("real_name");
    auto name = json->GetNamedString("name");

    auto profile = json->GetNamedObject("profile");
    auto title = profile->GetNamedString("title");
    auto images = ref new Vector<Uri^>();
    
    // Using list of possible image keys, try to load all the images
    // in the order defined by the list of possible values, and map
    // them into an ordered vector of urls.
    for (auto imageKey : IMAGE_KEYS)
    {
        if (!profile->HasKey(StringReference(imageKey)))
        {
            continue;
        }

        auto uri = ref new Uri(profile->GetNamedString(StringReference(imageKey)));
        images->Append(uri);
    }

    auto user = ref new SlackUser(id, realName, name, title, images);

    // Now try to parse the *optional* sections. e.g if these are missing
    // it's not the end of the world.
    if (profile->HasKey("email"))
    {
        user->_email = profile->GetNamedString("email");
    }

    if (json->HasKey("tz"))
    {
        user->_timeZoneName = json->GetNamedString("tz");
    }

    if (json->HasKey("color"))
    {
        user->_color = SlackUser::ParseColorString(json->GetNamedString("color"));
    }
    
    return user;
}

Color SlackUser::ParseColorString(String^ colorString)
{
    // Thanks Stack overflow! http://stackoverflow.com/questions/4804298/how-to-convert-wstring-into-string
    wstring_convert<codecvt_utf8<wchar_t>> converter;
    string compositeString = converter.to_bytes(colorString->Data());

    char red = (char)strtoul(compositeString.substr(0, 2).c_str(), nullptr, 16);
    char green = (char)strtoul(compositeString.substr(2, 2).c_str(), nullptr, 16);
    char blue = (char)strtoul(compositeString.substr(4, 2).c_str(), nullptr, 16);

    return ColorHelper::FromArgb(0xFF, red, green, blue);
}

String^ SlackUser::Id::get()
{
    return this->_id;
}

String^ SlackUser::RealName::get()
{
    return this->_realName;
}

String^ SlackUser::Name::get()
{
    return this->_name;
}

String^ SlackUser::Title::get()
{
    return this->_title;
}

UrlList^ SlackUser::Images::get()
{
    return this->_images;
}

String^ SlackUser::Email::get()
{
    return this->_email;
}

String^ SlackUser::TimeZoneName::get()
{
    return this->_timeZoneName;
}

Color SlackUser::Color::get()
{
    return this->_color;
}