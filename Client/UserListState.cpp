#include "pch.h"
#include "App.xaml.h"
#include "UserListState.h"
#include "UserListRequest.h"

using namespace Client;

using namespace concurrency;
using namespace Requests;

UserListState::UserListState()
{
    this->_usersRequest = ref new UserListRequest(App::SlackApiToken);
    this->_userRequestTask = this->_usersRequest->GetResultAsync();
}

task<Requests::UserListResult^> UserListState::GetUserList()
{
    return this->_userRequestTask;
}
