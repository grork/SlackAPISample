#include "pch.h"
#include "ProfileImageDownloader.h"
#include "SlackUser.h"

using namespace SharedCode;

using namespace concurrency;
using namespace Platform;
using namespace SlackDataObjects;
using namespace Windows::Foundation;
using namespace Windows::Storage;
using namespace Windows::Storage::Streams;
using namespace Windows::Web::Http;

task<Uri^> ProfileImageDownloader::DownloadImageForUser(SlackUser^ user, ProfileImageSize profileImageSize)
{
    StorageFolder^ localFolder = ApplicationData::Current->LocalFolder;
    String^ filename;
    Uri^ targetUri;

    // Compute the urls for easy access later
    // Not great: string allocations up the wazzoo
    switch (profileImageSize)
    {
        case ProfileImageSize::Small:
            targetUri = user->SmallProfileImage;
            filename = user->Id + L"_Small.jpg";
            break;

        case ProfileImageSize::Largest:
            filename = user->Id + "_Largest.jpg";
            if ((user->Images != nullptr) && (user->Images->Size > 0))
            {
                targetUri = user->Images->GetAt(user->Images->Size - 1);
            }
            break;
    }

    // Maps the path into something like:
    // %LOCALAPPDATA%\Packages\SlackTestApp.Client_69y2p3dxt9ar8\LocalState\<file>
    // This is easy to nuke on desktop; but on phone really requires uninstall-reinstall
    String^ fullPath = localFolder->Path + "\\" + filename;
    Uri^ localPath = (ref new Uri(L"ms-appdata:///local/"))->CombineUri(filename);

    // Little odd -- but valid UWP/Windows Store/AppContainer compliant/WACK test passing.
    // Rather than going off for a load of async open etc, we're gonna see if the file is
    // there using a sync API. Bit of a cheat...
    WIN32_FILE_ATTRIBUTE_DATA attributeInfo;
    BOOL fileExists = GetFileAttributesEx(fullPath->Data(), GET_FILEEX_INFO_LEVELS::GetFileExInfoStandard, &attributeInfo);

    if (fileExists)
    {
        return task_from_result(localPath);
    }

    // Since the file was not satisfied locally, we now need to go off into the internet and find it.
    auto client = ref new HttpClient();

    auto downloader = create_task(client->GetAsync(targetUri)).then(
        [targetUri, localFolder, filename, localPath](task<HttpResponseMessage^> completed_task)
    {
        // This handles the response as a task, rather than the raw response, because
        // when offline (e.g. no internet) this continuation will barf. Note that offline
        // is merely one of the ways it could barf; generally speaking "real network issue"
        // covers these.
        HttpResponseMessage^ response;
        bool requestWasSuccessful = false;
        try
        {
            response = completed_task.get();
            requestWasSuccessful = response->IsSuccessStatusCode;
        }
        catch (...)
        {
            requestWasSuccessful = false;
        }

        // If we couldn't get the image for some reason
        // just return the original url
        if (!requestWasSuccessful)
        {
            return task_from_result(targetUri);
        }

        return create_task(localFolder->CreateFileAsync(filename, CreationCollisionOption::ReplaceExisting)).then(
            [](StorageFile^ targetFile)
        {
            return create_task(targetFile->OpenAsync(FileAccessMode::ReadWrite));
        }).then(
            [response](IOutputStream^ outputStream)
        {
            return create_task(response->Content->WriteToStreamAsync(outputStream));
        }).then(
            [localPath, targetUri](task<unsigned long long> writeComplete)
        {
            try
            {
                writeComplete.wait();
            }
            catch (...)
            {
                __debugbreak();
                return targetUri;
            }

            return localPath;
        });
    });

    return downloader;
}
