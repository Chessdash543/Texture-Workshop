#pragma once

#include <Geode/Geode.hpp>
#include <Geode/utils/web.hpp>
#include <Geode/loader/Event.hpp>
#include <Geode/ui/TextInput.hpp>
#include <cctype>
#include <algorithm>
#include <Geode/utils/file.hpp>

#include "../../tps/headers/TWSPack.hpp"

using namespace geode::prelude;

class TWSPackInfo : public Popup {
public:
    static TWSPackInfo* create(TWSPack* tp) {
        auto popup = new TWSPackInfo;
        if (popup->init(tp)) {
            popup->autorelease();
            return popup;
        }
        delete popup;
        return nullptr;
    }
    
    void updateDownloadStata(); // stata 

    CCLabelBMFont* versionLabel;
    CCLabelBMFont* nameLabel;

    CCMenuItemSpriteExtra* tpDownload;
    CCMenuItemSpriteExtra* tpDelete;

    TWSPack* texturePack;

    ~TWSPackInfo();

protected:
    bool init(TWSPack* tp);

    void onDownload(CCObject* sender);
    void onDelete(CCObject* sender);
};