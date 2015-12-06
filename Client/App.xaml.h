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
