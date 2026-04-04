#pragma once

#include <Geode/Geode.hpp>
#include <Geode/utils/web.hpp>
#include <Geode/loader/Event.hpp>
#include <Geode/ui/TextInput.hpp>
#include <cctype>
#include <algorithm>
#include <Geode/utils/file.hpp>

#include "../../tps/headers/TMSPack.hpp"

using namespace geode::prelude;

class TMSPackInfo : public Popup {
public:
    static TMSPackInfo* create(TMSPack* tp) {
        auto popup = new TMSPackInfo;
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

    TMSPack* texturePack;

    ~TMSPackInfo();

protected:
    bool init(TMSPack* tp);

    void onDownload(CCObject* sender);
    void onDelete(CCObject* sender);
};