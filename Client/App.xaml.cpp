//
// App.xaml.cpp
// Implementation of the App class.
//

#include "pch.h"
#include "MainPage.xaml.h"

using namespace Client;

using namespace Platform;
using namespace Windows::ApplicationModel;
using namespace Windows::ApplicationModel::Activation;
using namespace Windows::Foundation;
using namespace Windows::UI::ViewManagement;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Interop;
using namespace Windows::UI::Xaml::Navigation;

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
		// Create a Frame to act as the navigation context and associate it with
		// a SuspensionManager key
		rootFrame = ref new Frame();
		rootFrame->Navigate(TypeName(MainPage::typeid), e->Arguments);

		// Place the frame in the current Window
		Window::Current->Content = rootFrame;

        // the minsize on desktop to mimic that of a small phone to allow better testing
        // without having to deploy to the phone/emulator
        ApplicationView::GetForCurrentView()->SetPreferredMinSize(Size(320, 480));
	}

    // Ensure the current window is active
    Window::Current->Activate();
}