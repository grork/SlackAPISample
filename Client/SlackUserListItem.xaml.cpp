//
// SlackUserListItem.xaml.cpp
// Implementation of the SlackUserListItem class
//

#include "pch.h"

#include "SlackUser.h"
#include "SlackUserListItem.xaml.h"

using namespace Client;

using namespace Platform;
using namespace SlackDataObjects;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Media::Imaging;


// The User Control item template is documented at http://go.microsoft.com/fwlink/?LinkId=234236

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
    this->UserImage->Source = ref new BitmapImage(user->SmallProfileImage);
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
