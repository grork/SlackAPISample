#include "pch.h"
#include "MainPage.xaml.h"
#include "SlackUser.h"
#include "SlackUserListItem.xaml.h"
#include "UserDetailPage.xaml.h"
#include "UserListRequest.h"
#include "UserListState.h"

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
}
void MainPage::OnNavigatedTo(NavigationEventArgs^ e)
{
    UserListState^ state = dynamic_cast<UserListState^>(e->Parameter);
    this->_usersRequest = state->GetUserList();
    this->_pageState = state;
}

void MainPage::Page_Loaded(Object^, RoutedEventArgs^)
{
    // Initalizae the previous size so we know when we're resizing over our threshold
    this->_previousSize = Window::Current->Bounds.Width;

    MainPage^ self = this;
    this->_usersRequest.then([self](UserListResult^ result)
    {
        if (result->Result == nullptr)
        {
            VisualStateManager::GoToState(self, L"Offline", true);
        }

        self->UsersList->ItemsSource = result->Result;

        // if we've got saved state in our code, then we should
        // try and restore that
        if (self->_pageState->CurrentUser != nullptr)
        {
            // if we've big enough to show the details page, navigate to it ...
            if (Window::Current->Bounds.Width > MainPage::DESKTOP_SIZE_THRESHOLD)
            {
                self->_NavigateDetailFrameToUser(self->_pageState->CurrentUser);
            }
            else
            {
                // ... but if we're not, clear it so we don't keep it hanging around
                self->_pageState->CurrentUser = nullptr;
            }
        }
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
    else if ((previouslyBiggerThanThreshold && !nowBiggerThanThreshold) && (this->_pageState->CurrentUser != nullptr))
    {
        this->_NavigateMainFrameToUser(this->_pageState->CurrentUser);
        Window::Current->SizeChanged -= this->_sizeChangedToken;
    }

    this->_previousSize = e->Size.Width;
}

void MainPage::UsersList_ItemClick(Object^ sender, ItemClickEventArgs^ e)
{
    SlackUser^ user = dynamic_cast<SlackUser^>(e->ClickedItem);
    this->_pageState->CurrentUser = user;

    if (Window::Current->Bounds.Width < MainPage::DESKTOP_SIZE_THRESHOLD)
    {
        this->_NavigateMainFrameToUser(user);
    }
    else
    {
        this->_NavigateDetailFrameToUser(user);
    }
}

void MainPage::_NavigateDetailFrameToUser(SlackUser^ user)
{
    this->DetailFrame->Navigate(TypeName(UserDetailPage::typeid), user, ref new EntranceNavigationTransitionInfo());
}

void MainPage::_NavigateMainFrameToUser(SlackUser^ user)
{
    this->Frame->Navigate(TypeName(UserDetailPage::typeid), user, ref new DrillInNavigationTransitionInfo());
}

void MainPage::DetailFrame_Navigated(Object^, NavigationEventArgs^ e)
{
    this->_pageState->CurrentUser = dynamic_cast<SlackUser^>(e->Parameter);
}
