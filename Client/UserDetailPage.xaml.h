//
// UserDetailPage.xaml.h
// Declaration of the UserDetailPage class
//

#pragma once

#include "SlackUser.h"
#include "UserDetailPage.g.h"
#include "MainPage.xaml.h"

namespace Client
{
	/// <summary>
	/// An empty page that can be used on its own or navigated to within a Frame.
	/// </summary>
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class UserDetailPage sealed
	{
	public:
		UserDetailPage();

    protected:
        virtual void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;

    private:
        void Window_Resized(Platform::Object^ sender, Windows::UI::Core::WindowSizeChangedEventArgs^ e);
        Windows::Foundation::EventRegistrationToken _sizeChangedToken;
        float _previousSize = 0.0;

        void Page_Loaded(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void Page_Unloaded(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
    };
}
