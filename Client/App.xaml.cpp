//
// App.xaml.cpp
// Implementation of the App class.
//

#include "pch.h"
#include "MainPage.xaml.h"
#include "UserListState.h"

using namespace Client;

using namespace Platform;
using namespace Windows::ApplicationModel;
using namespace Windows::ApplicationModel::Activation;
using namespace Windows::Foundation;
using namespace Windows::UI::Core;
using namespace Windows::UI::ViewManagement;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Interop;
using namespace Windows::UI::Xaml::Navigation;
using namespace Windows::UI::Xaml::Media::Animation;

App::App()
{
	InitializeComponent();
}

void App::OnLaunched(LaunchActivatedEventArgs^ e)
{

#if _DEBUG
		// Show graphics profiling information while debugging.
		if (IsDebuggerPresent())
		{
			// Display the current frame rate counters
			 DebugSettings->EnableFrameRateCounter = true;
		}
#endif

	auto rootFrame = dynamic_cast<Frame^>(Window::Current->Content);

	// Do not repeat app initialization when the Window already has content,
	// just ensure that the window is active
	if (rootFrame == nullptr)
	{
        // Listen to the global system back button to handle back navigation
        // This includes the taskbar, window chrome, and hardware back button
        // on phones
        SystemNavigationManager^ systemNavigationManager = SystemNavigationManager::GetForCurrentView();
        systemNavigationManager->BackRequested += ref new EventHandler<BackRequestedEventArgs^>(this, &App::HandleBackRequested);

		// Create a Frame to act as the navigation context
		rootFrame = ref new Frame();
        TransitionCollection^ transitions = ref new TransitionCollection();
        transitions->Append(ref new NavigationThemeTransition());
        rootFrame->ContentTransitions = transitions;
        this->_mainFrame = rootFrame;
        rootFrame->Navigated += ref new NavigatedEventHandler(this, &App::HandleNavigation);
		rootFrame->Navigate(TypeName(MainPage::typeid), ref new UserListState());

		// Place the frame in the current Window
		Window::Current->Content = rootFrame;

        // the minsize on desktop to mimic that of a small phone to allow easy testing
        // without having to deploy to the phone/emulator
        ApplicationView::GetForCurrentView()->SetPreferredMinSize(Size(320, 480));
	}

    // Ensure the current window is active
    Window::Current->Activate();
}

void App::HandleNavigation(Object^, NavigationEventArgs^)
{
    SystemNavigationManager^ systemNavigationManager = SystemNavigationManager::GetForCurrentView();
    if (this->_mainFrame->BackStackDepth > 0)
    {
        // if we've got something in our back stack, we'd like the OS to show a back button in the
        // title bar if it's appropriate
        systemNavigationManager->AppViewBackButtonVisibility = AppViewBackButtonVisibility::Visible;
    }
    else
    {
        systemNavigationManager->AppViewBackButtonVisibility = AppViewBackButtonVisibility::Collapsed;
    }
}

void App::HandleBackRequested(Object^, BackRequestedEventArgs^ args)
{
    if (!this->_mainFrame->CanGoBack)
    {
        return;
    }

    this->_mainFrame->GoBack();
    args->Handled = true;
}