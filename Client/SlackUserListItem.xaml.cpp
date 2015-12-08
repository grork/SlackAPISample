//
// SlackUserListItem.xaml.cpp
// Implementation of the SlackUserListItem class
//

#include "pch.h"

#include "ProfileImageDownloader.h"
#include "SlackUser.h"
#include "SlackUserListItem.xaml.h"

using namespace Client;

using namespace concurrency;
using namespace Platform;
using namespace SharedCode;
using namespace SlackDataObjects;
using namespace Windows::Foundation;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Media::Imaging;


SlackUserListItem::SlackUserListItem()
{
	InitializeComponent();
}

void SlackUserListItem::RenderSimpleUser(SlackUser^ user, SelectorItem^ container)
{
    this->NameBlock->Text = user->Name;
    this->RealNameBlock->Text = user->RealName;
    this->TitleBlock->Text = user->Title;

    SolidColorBrush^ userColorBrush = ref new SolidColorBrush(user->Color);
    container->Background = userColorBrush;
}

void SlackUserListItem::SlackUserListItem::RenderComplexUser(SlackUser^ user)
{
    SlackUserListItem^ self = this;

    ProfileImageDownloader::DownloadImageForUser(user, ProfileImageSize::Small).then([self](Uri^ image)
    {
        self->UserImage->Source = ref new BitmapImage(image);
    }, task_continuation_context::use_current());
}

void SlackUserListItem::ClearContent(SelectorItem^ container)
{
    String^ empty = L"";
    this->NameBlock->Text = empty;
    this->RealNameBlock->Text = empty;
    this->TitleBlock->Text = empty;

    container->ClearValue(SelectorItem::BackgroundProperty);

    this->UserImage->Source = nullptr;
}
