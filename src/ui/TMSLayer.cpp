using namespace geode::prelude;

#include "headers/TMSLayer.hpp"
#include "headers/TMSPackCell.hpp"
#include "../tps/headers/TMSPack.hpp"
#include "../header/boobs.hpp"
#include "./headers/TMSFilters.hpp"

#include <cctype>
#include <algorithm>
#include <matjson.hpp>
#include <Geode/ui/GeodeUI.hpp>
#include <Geode/loader/Event.hpp>

TMSLayer* TMSLayer::create() {
    auto ret = new TMSLayer();
    if (ret && ret->init()) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
};

CCScene* TMSLayer::scene() {
    auto layer = TMSLayer::create();
    auto scene = CCScene::create();
    scene->addChild(layer);
    return scene;
}

bool TMSLayer::init() {
    if(!CCLayer::init())
        return false;

    setID("TMSLayer");

    auto director = CCDirector::sharedDirector();
    auto winSize = director->getWinSize();

    get = this;

    CCSprite* backSpr = CCSprite::createWithSpriteFrameName("GJ_arrow_03_001.png");
    CCMenuItemSpriteExtra* backBtn = CCMenuItemSpriteExtra::create(backSpr, this, menu_selector(TMSLayer::onClose));
    backBtn->setID("back-button");

    CCMenu* backMenu = CCMenu::create();
    backMenu->setID("back-menu");
    backMenu->addChild(backBtn);
    addChild(backMenu, 1);

    backMenu->setPosition(ccp(director->getScreenLeft() + 25.f, director->getScreenTop() - 22.f));
    setKeyboardEnabled(true);
    setKeypadEnabled(true);

    m_background = CCLayerGradient::create({181, 69, 20, 255}, {48, 15, 104, 255}, {1, -1});
    m_background->setID("background");
    m_background->setAnchorPoint({ 0.f, 0.f });
    m_background->setContentSize(CCDirector::get()->getWinSize());
    addChild(m_background, -3);

    auto spriteTexture = CCSprite::create("TMS_Cubes.png"_spr);
    spriteTexture->setAnchorPoint({0, 0});

    ccTexParams params = {GL_LINEAR, GL_LINEAR, GL_REPEAT, GL_REPEAT};
    spriteTexture->getTexture()->setTexParameters(&params);

    auto rect = spriteTexture->getTextureRect();
    rect.size = rect.size * (CCPoint(winSize) / CCPoint(spriteTexture->getScaledContentSize()) * 2);
    rect.origin = CCPoint{0, 0};
    spriteTexture->setTextureRect(rect);
    spriteTexture->setRotation(18);
    spriteTexture->setOpacity(100);
    spriteTexture->setPosition({-winSize.width/2, 0});
    spriteTexture->setID("background-overlay");
    spriteTexture->setBlendFunc({ GL_SRC_ALPHA, GL_ONE_MINUS_CONSTANT_ALPHA });
    spriteTexture->runAction(CCRepeatForever::create(CCSequence::create(
        CCMoveBy::create(5, {16, 49}),
        CCMoveBy::create(0, {-16, -49}),
        nullptr
    )));

    addChild(spriteTexture, -2);

    auto bg = cocos2d::extension::CCScale9Sprite::create("square02_small.png");
    bg->setID("content-background");
    this->addChild(bg);

    bg->setPosition(winSize / 2);
    bg->setContentHeight(252);
    bg->setContentWidth(380);
    bg->setOpacity(135);
    bg->setPositionY(bg->getPositionY() - 15);

    outline = CCSprite::createWithSpriteFrameName("TMS_Outline.png"_spr);
    outline->setID("outline");
    this->addChild(outline);
    outline->setPosition(winSize / 2);
    outline->setScale(1.2);
    outline->setZOrder(1);

    pagesMenu = CCMenu::create();
    pagesMenu->setID("pages-menu");
    outline->addChild(pagesMenu, 1);
    pagesMenu->setPosition(0, 0);

    auto nextPageSpr = CCSprite::createWithSpriteFrameName("GJ_arrow_02_001.png");
    nextPageSpr->setFlipX(true);
    nextPage = CCMenuItemSpriteExtra::create(
        nextPageSpr,
        this,
        menu_selector(TMSLayer::onNextPage)
    );
    nextPage->setAnchorPoint({0, .5});
    pagesMenu->addChild(nextPage);
    nextPage->setPosition(ccp(outline->getContentSize().width + 1.5, (outline->getContentHeight() / 2) - 7));
    //nextPage->setVisible(false);

    auto prevPageSpr = CCSprite::createWithSpriteFrameName("GJ_arrow_02_001.png");
    prevPage = CCMenuItemSpriteExtra::create(
        prevPageSpr,
        this,
        menu_selector(TMSLayer::onPrevPage)
    );
    prevPage->setAnchorPoint({1, .5});
    pagesMenu->addChild(prevPage);
    prevPage->setPosition({-3.5, (outline->getContentHeight() / 2) - 7});
    prevPage->setVisible(false);
    
    pagesMenu->setContentSize(outline->getContentSize());

    buttonMenu = CCMenu::create();
    buttonMenu->setID("button-menu");
    addChild(buttonMenu, 1);
    
    buttonMenu->setPosition(0, 0);
    buttonMenu->setContentSize(winSize);

    auto discordSprite = CCSprite::createWithSpriteFrameName("gj_discordIcon_001.png");
    auto discordButton = CCMenuItemSpriteExtra::create(
        discordSprite,
        this,
        menu_selector(TMSLayer::onDiscord)
    );
    discordButton->setID("discord-button");
    buttonMenu->addChild(discordButton);
    discordButton->setPosition(ccp(director->getScreenRight() - 20, director->getScreenBottom() + 20));

    auto refreshSpr = CCSprite::createWithSpriteFrameName("TMS_RefreshButton.png"_spr);
    refreshSpr->setScale(0.8);
    refreshButton = CCMenuItemSpriteExtra::create(
        refreshSpr,
        this,
        menu_selector(TMSLayer::onRefresh)
    );
    refreshButton->setID("refresh-button");
    buttonMenu->addChild(refreshButton);
    refreshButton->setPosition(ccp(director->getScreenLeft() + 25, director->getScreenBottom() + 65));
    refreshButton->setVisible(false);

    auto filesSpr = CCSprite::createWithSpriteFrameName("TMS_FileButton.png"_spr);
    filesSpr->setScale(0.8);
    auto filesBtn = CCMenuItemSpriteExtra::create(
        filesSpr,
        this,
        menu_selector(TMSLayer::onPacksFolder)
    );
    filesBtn->setID("files-button");
    buttonMenu->addChild(filesBtn);
    filesBtn->setPosition(ccp(director->getScreenLeft() + 25, director->getScreenBottom() + 25));

    /*auto buttonSpr1 = CCSprite::createWithSpriteFrameName("TMS_Button.png"_spr);
    auto featuredSpr = CCSprite::createWithSpriteFrameName("GJ_sStarsIcon_001.png");
    buttonSpr1->addChild(featuredSpr);
    featuredSpr->setPosition(ccp(buttonSpr1->getContentSize().width / 2, buttonSpr1->getContentSize().height / 2));
    buttonSpr1->setScale(0.5);
    auto featuredBtn = CCMenuItemSpriteExtra::create(
        buttonSpr1,
        this,
        menu_selector(TMSLayer::onPacksFolder)
    );
    featuredBtn->setID("featured-button");
    buttonMenu->addChild(featuredBtn);
    featuredBtn->setPosition(ccp(director->getScreenLeft() + 25, director->getScreenBottom() + 25));*/
    // big yahu save me bro

    scroll = ScrollLayer::create(ccp(313, 180));
    scroll->setAnchorPoint(ccp(0, 0));
    scroll->ignoreAnchorPointForPosition(false);
    scroll->setZOrder(-1);
    scroll->setPosition(8, 8);

    outline->addChild(scroll);

    loading = LoadingCircleSprite::create(1);
    loading->setID("loading");
    loading->setScale(0.6f);
    loading->setPosition(ccp(outline->getContentWidth() / 2, outline->getContentHeight() / 2 + -21.875f));
    outline->addChild(loading);
    loading->setVisible(false);

    /*pageJson = matjson::parse(R"({
        "1": {
            "packID": 12,
            "packName": "featured test",
            "downloadLink": "https://github.com/Uproxide/storage/releases/download/a/Legacy.Uproxide.Difficulties.zip",
            "packLogo": "https://github.com/Uproxide/storage/releases/download/a/update.png",
            "packDescription": "testing tp.\n# hello!",
            "packCreator": "roxi",
            "packVersion": "1.2.0",
            "gdVersion": "2.2081",
            "packFeature": 1,
            "packDownloads": 105465,
            "creationDate": 0,
            "lastUpdated": 1755111695474
        },
        "2": {
            "packID": 12,
            "packName": "unfeatured test",
            "downloadLink": "https://github.com/Uproxide/storage/releases/download/a/Legacy.Uproxide.Difficulties.zip",
            "packLogo": "https://github.com/Uproxide/storage/releases/download/a/update.png",
            "packDescription": "testing tp.\n# hello!",
            "packCreator": "naomi",
            "packVersion": "1.2.0",
            "gdVersion": "2.2081",
            "packFeature": 0,
            "packDownloads": 105465,
            "creationDate": 0,
            "lastUpdated": 1755111695474
        }
    })").unwrap();*/
    boobs::search = "";
    boobs::page = 1;

    getTexturePacks(boobs::search); // web web web sahur
    getTexturePacksCount(boobs::search); // web web web sahur the 2

    inp = TextInput::create(300, "Search", "bigFont.fnt");
    inp->setContentHeight(20);
    inp->setAnchorPoint(ccp(0, 0));
    inp->ignoreAnchorPointForPosition(false); 
    this->outline->addChild(inp);

    inp->setPosition(ccp(6, 194.2));
    inp->hideBG();
    auto inputNode = inp->getInputNode();
    inputNode->setPositionY(inputNode->getPositionY() - 5);
    inputNode->setPositionX(5);
    inputNode->m_textLabel->setAnchorPoint(ccp(0, 0.5));
    inputNode->m_textLabel->setScale(0.5);
    inp->setDelegate(this);
    inp->setCommonFilter(CommonFilter::Any);

    inputText = "";
    this->schedule(schedule_selector(TMSLayer::doThingIdrk), 0.5f);

    return true;
}

void TMSLayer::getTexturePacks(std::string searchQuery) {
    if (scroll && scroll->m_contentLayer->getChildrenCount() > 0) 
        scroll->m_contentLayer->removeAllChildren();

    if (pageCount) pageCount->setVisible(false);
    if (loading) loading->setVisible(true);

    if (auto errorSlop = outline->getChildByID("error-text"_spr)) {
        outline->removeChild(errorSlop, true);
    }

    // Load local test.json
    auto jsonResult = geode::utils::file::readString("test.json");
    if (!jsonResult) {
        log::error("Failed to read test.json: {}", jsonResult.error());
        auto errorText = CCLabelBMFont::create("Failed to load texture packs data!", "bigFont.fnt");
        outline->addChild(errorText);
        errorText->setScale(0.3);
        errorText->setID("error-text"_spr);
        errorText->setPosition({ outline->getContentWidth() / 2, outline->getContentHeight() / 2 });
        loading->setVisible(false);
        nextPage->setVisible(false);
        prevPage->setVisible(false);
        return;
    }

    auto fullJson = matjson::parse(jsonResult.value());
    if (!fullJson) {
        log::error("Failed to parse test.json: {}", fullJson.error());
        auto errorText = CCLabelBMFont::create("Failed to parse texture packs data!", "bigFont.fnt");
        outline->addChild(errorText);
        errorText->setScale(0.3);
        errorText->setID("error-text"_spr);
        errorText->setPosition({ outline->getContentWidth() / 2, outline->getContentHeight() / 2 });
        loading->setVisible(false);
        nextPage->setVisible(false);
        prevPage->setVisible(false);
        return;
    }

    matjson::Value packsArray = matjson::Value::array();
    for (auto& [key, value] : fullJson.value().asObject().unwrap()) {
        packsArray.push(value);
    }

    // Filter if search query is provided
    if (!searchQuery.empty()) {
        matjson::Value filtered = matjson::Value::array();
        std::string lowerQuery = searchQuery;
        std::transform(lowerQuery.begin(), lowerQuery.end(), lowerQuery.begin(), ::tolower);
        for (auto& pack : packsArray) {
            std::string name = pack["packName"].asString().unwrapOr("");
            std::string desc = pack["packDescription"].asString().unwrapOr("");
            std::string lowerName = name;
            std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(), ::tolower);
            std::string lowerDesc = desc;
            std::transform(lowerDesc.begin(), lowerDesc.end(), lowerDesc.begin(), ::tolower);
            if (lowerName.find(lowerQuery) != std::string::npos || lowerDesc.find(lowerQuery) != std::string::npos) {
                filtered.push(pack);
            }
        }
        packsArray = filtered;
    }

    pageJson = packsArray;

    if (pageJson.size() == 0) {
        auto errorText = CCLabelBMFont::create("No texture packs found!", "bigFont.fnt");
        outline->addChild(errorText);
        errorText->setScale(0.3);
        errorText->setID("error-text"_spr);
        errorText->setPosition({ outline->getContentWidth() / 2, outline->getContentHeight() / 2 });
        loading->setVisible(false);
        nextPage->setVisible(false);
        prevPage->setVisible(false);
        return;
    }

    // PAGINAÇÃO LOCAL
    int itemsPerPage = 10;
    int start = (boobs::page - 1) * itemsPerPage;
    int end = std::min(start + itemsPerPage, (int)pageJson.size());

    if (start >= pageJson.size()) {
        // Página fora do alcance
        nextPage->setVisible(false);
        prevPage->setVisible(boobs::page > 1);
        loading->setVisible(false);
        return;
    }

    matjson::Value pageSubset = matjson::Value::array();
    for (int i = start; i < end; i++) {
        pageSubset.push(pageJson[i]);
    }

    setupTPCells(pageSubset); // chama a função nova com o subset da página
}

void TMSLayer::getTexturePacksCount(std::string searchQuery) {
    // Since pagination is done client-side, we calculate page info from already loaded data
    if (!pageJson.isArray() || pageJson.size() == 0) {
        log::warn("No texture pack data available for pagination calculation");
        return;
    }

    int totalItems = pageJson.size();
    int itemsPerPage = 10;
    int totalPages = (totalItems + itemsPerPage - 1) / itemsPerPage; // Ceiling division

    // Ensure current page is valid
    if (boobs::page > totalPages) {
        boobs::page = totalPages;
        getTexturePacks(boobs::search); // Reload with correct page
        return;
    }

    // Update page navigation visibility
    prevPage->setVisible(boobs::page > 1);
    nextPage->setVisible(boobs::page < totalPages);

    // Update page count display
    auto director = CCDirector::sharedDirector();
    std::string formattedText = fmt::format("Page {}/{} ({} Total)", boobs::page, totalPages, totalItems).c_str();
    if (pageCount) {
        pageCount->setString(formattedText.c_str());
        pageCount->setVisible(true);
    } else {
        pageCount = CCLabelBMFont::create(formattedText.c_str(), "goldFont.fnt");
        this->addChild(pageCount);
        pageCount->setScale(0.3);
        pageCount->setAnchorPoint({1, 1});
        pageCount->setPosition(ccp(director->getScreenRight() - 2, director->getScreenTop() - 2));
    }

    log::info("Calculated pagination: Page {}/{} ({} total items)", boobs::page, totalPages, totalItems);
}

void TMSLayer::setupTPCells(const matjson::Value& pageSubset) {
    if (pagesMenu) pagesMenu->setVisible(true);
    int i = 0;
    scroll->m_contentLayer->setAnchorPoint(ccp(0,1));

    scroll->m_contentLayer->removeAllChildren();
    loading->setVisible(false);

    tps.clear();

    prevPage->setVisible(boobs::page > 1);
    nextPage->setVisible(true);

    for (auto& tpObject : pageSubset) {
        bool featured = tpObject["packFeature"].asInt().unwrap() == 1;

        TMSPack* tp = TMSPack::create(
            tpObject["packID"].asInt().unwrap(),
            tpObject["packName"].asString().unwrap(),
            tpObject["downloadLink"].asString().unwrap(),
            "https://texture-makers-server.vercel.app" + tpObject["packLogo"].asString().unwrap(),
            tpObject["packDescription"].asString().unwrap(),
            tpObject["packCreator"].asString().unwrap(),
            tpObject["packVersion"].asString().unwrap(),
            tpObject["gdVersion"].asString().unwrap(),
            featured,
            tpObject["packDownloads"].asInt().unwrap()
        );

        tps.push_back(tp);

        stupid = !stupid;

        TMSPack* existingTp = nullptr;
        for (auto* downloadingTp : boobs::downloading) {
            if (downloadingTp && downloadingTp->ID == tpObject["packID"].asInt().unwrap()) {
                existingTp = downloadingTp;
            }
        }

        TMSPackCell* tpCell = existingTp ? TMSPackCell::create(existingTp, stupid)
                                         : TMSPackCell::create(tp, stupid);

        scroll->m_contentLayer->addChild(tpCell);
        tpCell->setPosition(0, 314 - (35 * i));
        tpCell->pagesMenu = pagesMenu;
        tpCell->inp = inp;
        i++;
    }

    if (i < 10) {
        nextPage->setVisible(false);
    }

    scroll->m_contentLayer->setContentSize(ccp(scroll->m_contentLayer->getContentSize().width, 35 * 10));
    scroll->moveToTop();
}

void TMSLayer::onClose(CCObject*) {
    auto mainMenu = MenuLayer::scene(false);
    CCDirector::sharedDirector()->pushScene(CCTransitionFade::create(0.5f, mainMenu));
}

void TMSLayer::onDiscord(CCObject*) {
    utils::web::openLinkInBrowser("https://discord.gg/pUGPY9hQ22");
}

void TMSLayer::onRefresh(CCObject*) {
    // deprecated
}

void TMSLayer::onPacksFolder(CCObject*) {
    utils::file::openFolder(Loader::get()->getInstalledMod("geode.texture-loader")->getConfigDir());
}

// prev/next page of texture packs
void TMSLayer::onPrevPage(CCObject*) {
    if (boobs::page == 1) {
        return;
    }

    boobs::page -= 1;
    getTexturePacks(boobs::search);
    getTexturePacksCount(boobs::search);
}

void TMSLayer::onNextPage(CCObject*) {
    boobs::page += 1;
    getTexturePacks(boobs::search);
    getTexturePacksCount(boobs::search);
}

void TMSLayer::onSort(CCObject*) {
    auto sortPopup = TMSFilters::create();
    sortPopup->show();
}

void TMSLayer::onSearch(CCObject*) {
    boobs::search = inp->getString();
    boobs::page = 1;
    getTexturePacks(boobs::search);
    getTexturePacksCount(boobs::search);
}

void TMSLayer::keyBackClicked() {
    onClose(nullptr);
}

void TMSLayer::textChanged(CCTextInputNode*) {
    boobs::search = inp->getString();
}

void TMSLayer::doThingIdrk(float) {
    if (!inp) {
        return;
    }

    auto currentSearch = inp->getString();
    if (currentSearch == inputText) {
        return;
    }

    inputText = currentSearch;
    boobs::search = currentSearch;
    boobs::page = 1;
    getTexturePacks(boobs::search);
}

TMSLayer::~TMSLayer()
{
    //boobs::search = "";
    boobs::page = 1;
    get = nullptr;
}

/*
class TextureWorkshop : public cocos2d::CCLayer
{


texture workshop = running
hackers = false
deny ai thumbnail = true
good mod have 1 mil download free = true
geodoe 2.208 support = true
*/

