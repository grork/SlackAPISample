#include "pch.h"
#include "SlackUser.h"

#pragma once

namespace SharedCode
{
    enum class ProfileImageSize
    {
        Small,
        Largest
    };

    ref class ProfileImageDownloader sealed
    {
    internal:
        /// <summary>
        /// Helper to download & cache images from urls based on the supplied user.
        /// If these have already been downloaded, we're going to favour the local version
        /// of them irrespective of the networkstatus
        /// </summary>
        static concurrency::task<Windows::Foundation::Uri^> DownloadImageForUser(SlackDataObjects::SlackUser^ user, ProfileImageSize profileImageSize);
    };

}