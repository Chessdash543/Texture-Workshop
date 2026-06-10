#pragma once
#include <Geode/Geode.hpp>

using namespace geode::prelude;

class TMSPackCell;
class TMSPackInfo;

class TMSPack : public cocos2d::CCObject {
    public:
        TMSPackCell* cell = nullptr;
        TMSPackInfo* info = nullptr;

        std::string TPName, DownloadURL, IconURL, TPDescription, TPCreator, TPVersion, GDVersion, ThumbnailURL;
        int ID, downloads;
        bool featured;

        Slider* downloadingIndicator = nullptr;
        Slider* downloadingIndicatorInfoPage = nullptr;

        static TMSPack* create(int id, std::string name, std::string downloadURL, std::string iconURL, std::string description, std::string creator, std::string version, std::string gdVersion, bool isFeatured, int downloads, std::string thumbnailURL = "") {
            auto tp = new TMSPack();

            tp->ID = id;
            tp->TPName = name;
            tp->DownloadURL = downloadURL;
            tp->IconURL = iconURL;
            tp->TPDescription = description;
            tp->TPCreator = creator;
            tp->TPVersion = version;
            tp->GDVersion = gdVersion;
            tp->featured = isFeatured;
            tp->downloads = downloads;
            tp->ThumbnailURL = thumbnailURL;

            return tp;
        };

        void downloadTP();
        bool isDownloading = false;
        bool downloadSuccessful = false;

        async::TaskHolder<geode::utils::web::WebResponse> m_downloadTPlistener;
};