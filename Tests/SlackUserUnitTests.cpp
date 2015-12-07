#include "pch.h"
#include "CppUnitTest.h"
#include "SlackUser.h"

using namespace concurrency;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace Platform;
using namespace Windows::Data::Json;
using namespace Windows::Foundation;
using namespace Windows::Storage;
using namespace Windows::UI;

namespace Tests
{
    TEST_CLASS(SlackUserTests)
    {

    private:
        static JsonObject^ GetJsonForFile(String^ file)
        {
            auto localFolder = Windows::ApplicationModel::Package::Current->InstalledLocation;
            
            auto readDataTask = create_task(localFolder->GetFileAsync(file)).then([](StorageFile^ file)
            {
                return create_task(FileIO::ReadTextAsync(file));
            }).then([](String^ data)
            {
                return JsonObject::Parse(data);
            });


            return readDataTask.get();
        }

    public:
        TEST_METHOD(CanParseMinimalUser)
        {
            JsonObject^ parsedData = SlackUserTests::GetJsonForFile("MinimalUser.json");

            SlackDataObjects::SlackUser^ user = SlackDataObjects::SlackUser::FromJson(parsedData);

            Assert::AreEqual(L"U04LJPD46", user->Id, "Wrong User ID");
            Assert::AreEqual(L"Anthony Mathur", user->RealName, "Wrong real name");
            Assert::AreEqual(L"anthony", user->Name, "Wrong name");
            Assert::AreEqual(L"Development", user->Title, "Wrong title");

            Assert::IsNotNull(user->Images, "Expected some images");
            Assert::IsTrue(2 == user->Images->Size, L"Incorrect number of images returned");

            auto firstImage = user->Images->GetAt(0);
            Assert::AreEqual(L"https://avatars.slack-edge.com/2015-05-01/4698799712_e1784211d2146055eb02_24.jpg",
                firstImage->RawUri, "Urls for first image didn't match");

            auto secondImage = user->Images->GetAt(1);
            Assert::AreEqual(L"https://avatars.slack-edge.com/2015-05-01/4698799712_e1784211d2146055eb02_original.jpg",
                secondImage->RawUri, "Urls for second image didn't match");
        }

        TEST_METHOD(CanParseComplexUser)
        {
            JsonObject^ parsedData = SlackUserTests::GetJsonForFile("SampleSlackUser.json");

            SlackDataObjects::SlackUser^ user = SlackDataObjects::SlackUser::FromJson(parsedData);

            Assert::AreEqual(L"U04LJPD46", user->Id, "Wrong User ID");
            Assert::AreEqual(L"Anthony Mathur", user->RealName, "Wrong real name");
            Assert::AreEqual(L"anthony", user->Name, "Wrong name");
            Assert::AreEqual(L"Development", user->Title, "Wrong title");
            Assert::AreEqual(L"brady+ahmedexercise3@slack-corp.com", user->Email , "Wrong Email");
            Assert::AreEqual(L"America/Los_Angeles", user->TimeZoneName, "Wrong Time Zone Names");

            Assert::IsNotNull(user->Images, "Expected some images");

            auto expectedUrls = {
                L"https://avatars.slack-edge.com/2015-05-01/4698799712_e1784211d2146055eb02_24.jpg",
                L"https://avatars.slack-edge.com/2015-05-01/4698799712_e1784211d2146055eb02_32.jpg",
                L"https://avatars.slack-edge.com/2015-05-01/4698799712_e1784211d2146055eb02_48.jpg",
                L"https://avatars.slack-edge.com/2015-05-01/4698799712_e1784211d2146055eb02_72.jpg",
                L"https://avatars.slack-edge.com/2015-05-01/4698799712_e1784211d2146055eb02_192.jpg",
                L"https://avatars.slack-edge.com/2015-05-01/4698799712_e1784211d2146055eb02_original.jpg"
            };

            Assert::IsTrue(expectedUrls.size() == user->Images->Size, L"Incorrect number of images returned");

            unsigned int index = 0;
            for (auto rawUrl : expectedUrls)
            {
                auto uri = user->Images->GetAt(index++);
                Assert::AreEqual(StringReference(rawUrl), uri->RawUri, "Unepxeceted or out of order URL");
            }

            Color color = user->Color;
            Assert::IsTrue(color.A == 0xFF, L"Wrong Alpha channel");
            Assert::IsTrue(color.R == 0xE7, L"Wrong Red channel");
            Assert::IsTrue(color.G == 0x39, L"Wrong Green channel");
            Assert::IsTrue(color.B == 0x2D, L"Wrong Blue channel");
        }
    };
}