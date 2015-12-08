//
// MainPage.xaml.h
// Declaration of the MainPage class.
//

#pragma once

#include "MainPage.g.h"
#include "SlackUserListItem.xaml.h"
#include "UserListRequest.h"

namespace Client
{
	/// <summary>
	/// An empty page that can be used on its own or navigated to within a Frame.
	/// </summary>
	public ref class MainPage sealed
	{
	public:
		MainPage();

    internal:
        static const float DESKTOP_SIZE_THRESHOLD;

    private:
        concurrency::task<Requests::UserListResult^> _usersRequest;
        void Page_Loaded(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void UsersList_ContainerContentChanging(Windows::UI::Xaml::Controls::ListViewBase^ sender, Windows::UI::Xaml::Controls::ContainerContentChangingEventArgs^ args);
        void DoubleData_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void UsersList_ItemClick(Platform::Object^ sender, Windows::UI::Xaml::Controls::ItemClickEventArgs^ e);
        void Window_Resized(Platform::Object^ sender, Windows::UI::Core::WindowSizeChangedEventArgs^ e);
        void _NavigateMainFrameToUser(SlackDataObjects::SlackUser^ user);
        Windows::Foundation::EventRegistrationToken _sizeChangedToken;
        float _previousSize = 0.0;
        SlackDataObjects::SlackUser^ _lastInvokedUser;

        void DetailFrame_Navigated(Platform::Object^ sender, Windows::UI::Xaml::Navigation::NavigationEventArgs^ e);
        void Page_Unloaded(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
    };
}
