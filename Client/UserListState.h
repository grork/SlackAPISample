#pragma once

#include "pch.h"
#include "UserListRequest.h"
#include "SlackUser.h"

namespace Client
{
    ref class UserListState sealed
    {
    public:
        UserListState();

    internal:
        concurrency::task<Requests::UserListResult^> GetUserList();
        property SlackDataObjects::SlackUser^ CurrentUser;

    private:
        Requests::UserListRequest^ _usersRequest;
        concurrency::task<Requests::UserListResult^> _userRequestTask;
    };
}
