//
// SlackUserListItem.xaml.h
// Declaration of the SlackUserListItem class
//

#pragma once

#include "SlackUserListItem.g.h"
#include "SlackUser.h"

namespace Client
{
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class SlackUserListItem sealed
	{
	public:
		SlackUserListItem();

        /// <summary>
        /// Renders the simple user information (e.g. quick, low CPU intensity)
        /// </summary>
        void RenderSimpleUser(SlackDataObjects::SlackUser^ user, Windows::UI::Xaml::Controls::Primitives::SelectorItem^ container);

        /// <summary>
        /// Renders more complex data, only if we've been on the screen long enough
        /// (e.g. later phases of a listview rendering)
        /// </summary>
        void RenderComplexUser(SlackDataObjects::SlackUser^ user);

        /// <summary>
        /// Clears all the previous content from the users layout so we don't see cross-user data
        /// </summary>
        void ClearContent(Windows::UI::Xaml::Controls::Primitives::SelectorItem^ container);
	};
}
