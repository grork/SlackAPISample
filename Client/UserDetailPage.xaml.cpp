#include "pch.h"
#include "SlackUser.h"
#include "UserDetailPage.xaml.h"

using namespace Client;

using namespace Platform;
using namespace Platform::Collections;
using namespace SlackDataObjects;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Media::Imaging;
using namespace Windows::UI::Xaml::Navigation;


UserDetailPage::UserDetailPage()
{
	InitializeComponent();
}

void UserDetailPage::OnNavigatedTo(NavigationEventArgs^ e)
{
    SlackUser^ user = static_cast<SlackUser^>(e->Parameter);
    this->MainGrid->Background = ref new SolidColorBrush(user->Color);

    if ((user->Images != nullptr) && (user->Images->Size > 0))
    {
        auto largestImage = user->Images->GetAt(user->Images->Size - 1);
        this->UserImage->Source = ref new BitmapImage(largestImage);
    }

    this->NameBlock->Text = user->Name;
    this->RealNameBlock->Text = user->RealName;
    this->TitleBlock->Text = user->Title;

    Vector<String^>^ additionalInformation = ref new Vector<String^>();

    additionalInformation->Append(user->Email);
    additionalInformation->Append(user->TimeZoneName);
    additionalInformation->Append(user->Id);
    
    this->AdditionalInfo->ItemsSource = additionalInformation;
}
