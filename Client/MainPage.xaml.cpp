#include "pch.h"
#include "MainPage.xaml.h"
#include "SlackUser.h"
#include "SlackUserListItem.xaml.h"
#include "UserDetailPage.xaml.h"
#include "UserListRequest.h"

using namespace Client;

using namespace concurrency;
using namespace Platform;
using namespace Requests;
using namespace SlackDataObjects;
using namespace Windows::Foundation;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Interop;

MainPage::MainPage()
{
	InitializeComponent();

    auto request = ref new UserListRequest(App::SlackApiToken);
    this->_usersRequest = request->GetResultAsync();
}

void MainPage::Page_Loaded(Object^ sender, RoutedEventArgs^ e)
{
    MainPage^ self = this;
    this->_usersRequest.then([self](UserListResult^ result)
    {
        self->UsersList->ItemsSource = result->Result;
    }, task_continuation_context::use_current());
}


void MainPage::UsersList_ContainerContentChanging(ListViewBase^ sender, ContainerContentChangingEventArgs^ args)
{
    // Assume it's going to be our control since we own all parts.
    SlackUserListItem^ control = static_cast<SlackUserListItem^>(args->ItemContainer->ContentTemplateRoot);

    if (args->InRecycleQueue)
    {
        control->ClearContent(args->ItemContainer);
        return;
    }

    if (args->Phase == 0)
    {
        control->RenderSimpleUser(static_cast<SlackUser^>(args->Item), args->ItemContainer);
        args->RegisterUpdateCallback(ref new TypedEventHandler<ListViewBase^, ContainerContentChangingEventArgs^>(this, &MainPage::UsersList_ContainerContentChanging));
    }
    else if (args->Phase == 1)
    {
        control->RenderComplexUser(static_cast<SlackUser^>(args->Item));
    }
}


void MainPage::DoubleData_Click(Object^, RoutedEventArgs^)
{
    SlackUsers^ users = dynamic_cast<SlackUsers^>(this->UsersList->ItemsSource);
    
    auto currentCount = users->Size;

    for (unsigned int i = 0; i < currentCount; i++)
    {
        auto itemToCopy = users->GetAt(i);
        users->Append(itemToCopy);
    }
}


void MainPage::UsersList_ItemClick(Object^ sender, ItemClickEventArgs^ e)
{
    Controls::Frame^ frame = dynamic_cast<Controls::Frame^>(Window::Current->Content);
    frame->Navigate(TypeName(UserDetailPage::typeid), e->ClickedItem);

}
