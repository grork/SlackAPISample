#include "pch.h"
#include "MainPage.xaml.h"
#include "UserListRequest.h"

using namespace Client;

using namespace concurrency;
using namespace Platform;
using namespace Requests;
using namespace Windows::UI::Xaml;

MainPage::MainPage()
{
	InitializeComponent();

    auto request = ref new UserListRequest(App::SlackApiToken);
    this->_userList = request->GetResultAsync();
}

void MainPage::Page_Loaded(Object^ sender, RoutedEventArgs^ e)
{
    MainPage^ self = this;
    this->_userList.then([self](UserListResult^ result)
    {
        self->UserItemsControl->ItemsSource = result->Result;
    }, task_continuation_context::use_current());
}
