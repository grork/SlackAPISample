//
// UserDetailPage.xaml.h
// Declaration of the UserDetailPage class
//

#pragma once

#include "SlackUser.h"
#include "UserDetailPage.g.h"

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
	};
}
