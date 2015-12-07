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
        String^ message = "";
        if (result->IsSuccessful)
        {
            if (result->WasSatisfiedFromCache)
            {
                message = L"Successful from Cache!";
            }
            else
            {
                message = L"Successful!";
            }
        }
        else
        {
            if (result->HasResult && result->WasSatisfiedFromCache)
            {
                message = "Loaded From cache";
            }
            else
            {
                message = L"Failed to load!";
            }
        }

        self->MessageBlock->Text = message;
    }, task_continuation_context::use_current());
}
