#include "pch.h"
#include "MainPage.xaml.h"
#include "UserListRequest.h"

using namespace Client;

using namespace Platform;

MainPage::MainPage()
{
	InitializeComponent();

    auto req = ref new Requests::UserListRequest("");
    if (req == nullptr)
    {
        auto dialog = ref new Windows::UI::Popups::MessageDialog("No request");
        dialog->ShowAsync();
    }
}
