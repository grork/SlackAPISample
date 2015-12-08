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
using namespace Windows::UI::Core;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Interop;
using namespace Windows::UI::Xaml::Media::Animation;
using namespace Windows::UI::Xaml::Navigation;

const float MainPage::DESKTOP_SIZE_THRESHOLD = 640;

MainPage::MainPage()
{
	InitializeComponent();

    auto request = ref new UserListRequest(App::SlackApiToken);
    this->_usersRequest = request->GetResultAsync();
}

void MainPage::Page_Loaded(Object^, RoutedEventArgs^)
{
    this->_previousSize = Window::Current->Bounds.Width;

    MainPage^ self = this;
    this->_usersRequest.then([self](UserListResult^ result)
    {
        self->UsersList->ItemsSource = result->Result;
    }, task_continuation_context::use_current());

    this->_sizeChangedToken = Window::Current->SizeChanged += ref new WindowSizeChangedEventHandler(this, &MainPage::Window_Resized);
}

void MainPage::Page_Unloaded(Object^ sender, RoutedEventArgs^)
{
    Window::Current->SizeChanged -= this->_sizeChangedToken;
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

void MainPage::Window_Resized(Object^, WindowSizeChangedEventArgs^ e)
{
    bool previouslyBiggerThanThreshold = (this->_previousSize > MainPage::DESKTOP_SIZE_THRESHOLD);
    bool nowBiggerThanThreshold = (e->Size.Width > MainPage::DESKTOP_SIZE_THRESHOLD);

    if ((previouslyBiggerThanThreshold && nowBiggerThanThreshold)
        || (!previouslyBiggerThanThreshold && !nowBiggerThanThreshold)
        || (!previouslyBiggerThanThreshold && nowBiggerThanThreshold))
    {
        // Nothing to do because we didn't cross the transition
    }
    else if ((previouslyBiggerThanThreshold && !nowBiggerThanThreshold) && (this->_lastInvokedUser != nullptr))
    {
        this->_NavigateMainFrameToUser(this->_lastInvokedUser);
        Window::Current->SizeChanged -= this->_sizeChangedToken;
    }

    this->_previousSize = e->Size.Width;
}

void MainPage::UsersList_ItemClick(Object^ sender, ItemClickEventArgs^ e)
{
    SlackUser^ user = dynamic_cast<SlackUser^>(e->ClickedItem);

    if (Window::Current->Bounds.Width < 640)
    {
        this->_NavigateMainFrameToUser(user);
    }
    else
    {
        this->DetailFrame->Navigate(TypeName(UserDetailPage::typeid), user, ref new EntranceNavigationTransitionInfo());
    }
}

void MainPage::_NavigateMainFrameToUser(SlackUser^ user)
{
    this->Frame->Navigate(TypeName(UserDetailPage::typeid), user, ref new DrillInNavigationTransitionInfo());
}


void MainPage::DetailFrame_Navigated(Object^, NavigationEventArgs^ e)
{
    this->_lastInvokedUser = dynamic_cast<SlackUser^>(e->Parameter);
}
