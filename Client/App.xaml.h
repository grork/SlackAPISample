#pragma once

#include "App.g.h"

namespace Client
{
	/// <summary>
	/// Provides application-specific behavior to supplement the default Application class.
	/// </summary>
	ref class App sealed
	{
	protected:
		virtual void OnLaunched(Windows::ApplicationModel::Activation::LaunchActivatedEventArgs^ e) override;

    private:
        Windows::UI::Xaml::Controls::Frame^ _mainFrame;
        void HandleNavigation(Platform::Object^ sender, Windows::UI::Xaml::Navigation::NavigationEventArgs^ args);
        void HandleBackRequested(Platform::Object^ sender, Windows::UI::Core::BackRequestedEventArgs^ args);

	internal:
		App();

        static property Platform::String^ SlackApiToken
        {
            Platform::String^ get()
            {
                return Platform::StringReference(L"REDACTED");
            }
        }
	};
}
