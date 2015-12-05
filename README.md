This is a sample project REDACTED

# TODO #

Strike through = Done

## Getting Setup ##
+ ~~Create Git Repo on TFS for shared work~~
	+ Check in TODO from here
	+ REDACTED
+ Add Empty Project
+ Add Empty Test Project
+ ~~Spend time understanding the auth / calling API~~
	+ ~~CURL it up yo'~~
	+ ~~Check support for 'stuff didn't change since last request'~~ _(No, no caching)_

## Basic Data Access ##
+ Add network request that outputs to the main content area on success and failure
+ Add sample response file to repo
	+ Normal response
	+ Error responses from api page
	+ Mutated responses
+ Add unit tests for parsing & handling the basic response
+ Add support to writing the network response to disk
+ Add loading data from disk if present & offline (go to network if online)
	+ Mock offline network status check?
	+ Should be checking more for failure, rather than being online?
+ Add Object model representing the items
	+ Deserialize in some form from the JSON to the Object models
+ Add support for merging any changes in a network request to offer change notification (Data already written to disk, so no need to write "diffs").
+ Should this be done via an ID broadcast that is "no, you should probably load this item directly again"
	+ Is this just making the thing too damn complicated?

## UI ##
+ Add listview, displays basic data through {binding}
+ Create a user control for the list item
+ Update listview to use CCC + add call back into custom control
+ Add invoke handler to navigate to a new user detail page
+ Add back requested support to navigate back
+ Add new basic layout for user details on the new page
+ Understand image size urls
	+ Add to both list item, and details page
	+ Custom Image loading control to provide nice transition from blank to full?
+ Add loading spinner to startup if > 150ms of load time
+ Add support for splitting the view in full screen mode
	+ Secondary frame, or primary frame that overlays the listview?
+ Use page/Control for user page as detail for master-detail
+ Make layout look nice following 4px grid


#### Reference ####
API Doc: https://api.slack.com/methods/users.list
API Token: REDACTED