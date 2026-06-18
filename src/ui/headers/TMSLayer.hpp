#pragma once

using namespace geode::prelude;
#include <Geode/ui/TextInput.hpp>
#include <Geode/utils/web.hpp>
#include <Geode/loader/Event.hpp>
#include <Geode/utils/async.hpp>

#include "../../tps/headers/TMSPack.hpp"

class TMSLayer : public cocos2d::CCLayer, TextInputDelegate {
protected:
    CCLayerGradient* m_background;
    void parseJson(std::string str);
    bool search = false;
    bool noresults = false;
    ScrollLayer* scroll = nullptr;

    bool stupid = true;
    
    CCScale9Sprite* bg;
    CCSprite* outline;
    CCLabelBMFont* pageCount;
    
    CCMenuItemSpriteExtra* refreshButton;
    CCMenuItemSpriteExtra* searchBtn;
    CCMenuItemSpriteExtra* prevPage;
    CCMenuItemSpriteExtra* nextPage;
    CCMenuItemSpriteExtra* sortButton;
    CCMenu* buttonMenu;
    std::string inputText;
    async::TaskHolder<geode::utils::web::WebResponse> m_getTPslistener;
    async::TaskHolder<geode::utils::web::WebResponse> m_getTPsCountlistener;
    CCMenu* pagesMenu = nullptr;
    TextInput* inp;

    LoadingCircleSprite* loading;

    matjson::Value pageJson;
    int m_requestId = 0;

    //int page = 1;

    virtual void textChanged(CCTextInputNode* p0);

    ~TMSLayer();
    
public:
    static TMSLayer* create();
    static cocos2d::CCScene* scene();
    static inline TMSLayer* get = nullptr;
    bool init();
    

    std::vector<TMSPack*> tps = {};

    void onClose(CCObject*);
    void searchTPs();
    void keyBackClicked();

    void getTexturePacks(std::string searchQuery);
    void getTexturePacksCount(std::string searchQuery);
    void setupTPCells(const matjson::Value& pageSubset);

    void onDiscord(CCObject*);
    void onPacksFolder(CCObject*);
    void onRefresh(CCObject*);
    void onRefreshSearch(CCObject*);
    void onSearch(CCObject*);
    void onFilter(CCObject*);

    void onSort(CCObject*);

    void onNextPage(CCObject*);
    void onPrevPage(CCObject*);

    void doThingIdrk(float);
};