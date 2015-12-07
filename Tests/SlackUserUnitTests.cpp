#include "pch.h"
#include "CppUnitTest.h"
#include "SlackUser.h"

using namespace concurrency;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace Platform;
using namespace Windows::Foundation;

namespace Tests
{
    TEST_CLASS(SlackUserTests)
    {
    public:
        TEST_METHOD(CanInstantiateSlackUser)
        {
            auto slackUser = ref new SlackDataObjects::SlackUser();
            Assert::IsNotNull(slackUser, "Couldn't create user");
        }
    };
}