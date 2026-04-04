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

class TMSPackCell : public CCLayerColor {
    public:

        TMSPack* texturePack;

        // texture pack info, hey guys its me uproxide this is my awesome comment
        geode::LazySprite* icon;
        CCLabelBMFont* nameLabel;
        CCLabelBMFont* versionLabel;
        CCLayerGradient* gradient;

        CCMenuItemSpriteExtra* tpDownload;
        CCMenuItemSpriteExtra* tpDelete;

        bool lighterColor; // im so green ough

        CCMenu* pagesMenu;
        TextInput* inp;

        static TMSPackCell* create(TMSPack* tp, bool other) {
            TMSPackCell* pRet = new TMSPackCell();
            if (pRet && pRet->init(tp, other)) {
                pRet->autorelease();
                return pRet;
            } else {
                delete pRet;
                return nullptr;
            }
        }

        ~TMSPackCell()
        {
            if (texturePack) {
                texturePack->cell = nullptr;
                texturePack->downloadingIndicator = nullptr;
            }
            texturePack = nullptr;
        }

        void updateDownloadStata(); // stata :joy:
        // id change it to "updateDownloadState" but its funnier to keep it as stata so its staying
    
    protected:
        bool init(TMSPack* tp, bool other);

        void onInfo(CCObject*);
        void onCreator(CCObject*);
        void onDownload(CCObject*);
        void onDelete(CCObject*);

        void checkIfDownloading(); 
};