#include "pch.h"
#include "ProfileImageDownloader.h"
#include "SlackUser.h"
#include "UserDetailPage.xaml.h"

using namespace Client;

using namespace concurrency;
using namespace Platform;
using namespace Platform::Collections;
using namespace SharedCode;
using namespace SlackDataObjects;
using namespace Windows::Foundation;
using namespace Windows::UI::Core;
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
    UserDetailPage^ self = this;

    ProfileImageDownloader::DownloadImageForUser(user, ProfileImageSize::Largest).then([self](Uri^ uri)
    {
        self->UserImage->Source = ref new BitmapImage(uri);
    }, task_continuation_context::use_current());

    this->NameBlock->Text = user->Name;
    this->RealNameBlock->Text = user->RealName;
    this->TitleBlock->Text = user->Title;

    Vector<String^>^ additionalInformation = ref new Vector<String^>();

    additionalInformation->Append(user->Email);
    additionalInformation->Append(user->TimeZoneName);
    additionalInformation->Append(user->Id);
    
    this->AdditionalInfo->ItemsSource = additionalInformation;
}

void UserDetailPage::Window_Resized(Object^, WindowSizeChangedEventArgs^ e)
{
    bool previouslyBiggerThanThreshold = (this->_previousSize > MainPage::DESKTOP_SIZE_THRESHOLD);
    bool nowBiggerThanThreshold = (e->Size.Width > MainPage::DESKTOP_SIZE_THRESHOLD);

    if ((previouslyBiggerThanThreshold && nowBiggerThanThreshold)
        || (!previouslyBiggerThanThreshold && !nowBiggerThanThreshold)
        || (previouslyBiggerThanThreshold && !nowBiggerThanThreshold))
    {
        // Nothing to do because we didn't cross the transition
    }
    else if ((!previouslyBiggerThanThreshold && nowBiggerThanThreshold))
    {
        // Now bigger than we started, and if we're in the main frame,
        // navigate back
        if (Window::Current->Content == this->Frame)
        {
            this->Frame->GoBack();
        }
    }

    this->_previousSize = e->Size.Width;
}

void UserDetailPage::Page_Loaded(Object^, RoutedEventArgs^)
{
    this->_sizeChangedToken = Window::Current->SizeChanged += ref new WindowSizeChangedEventHandler(this, &UserDetailPage::Window_Resized);
}

void UserDetailPage::Page_Unloaded(Object^ sender, RoutedEventArgs^)
{
    Window::Current->SizeChanged -= this->_sizeChangedToken;
}
