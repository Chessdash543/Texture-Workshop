#include <algorithm>
#include "./headers/TMSPackInfo.hpp"
#include "./headers/TMSPackCell.hpp"

static std::string formatTPVersion(const std::string& version) {
    if (version.empty() || version[0] == 'v')
        return version;
    return "v" + version;
}

bool TMSPackInfo::init(TMSPack* tp)
{
    if (!Popup::init(335.f, 231.f, "TMS_Box.png"_spr))
        return false;

    texturePack = tp;
    texturePack->info = this;

    float scale = CCDirector::sharedDirector()->getContentScaleFactor()/4;

    LazySprite* icon = LazySprite::create({100, 100});
    icon->setLoadCallback(
        [this, reloadIconTries = 0, tp, icon](Result<> result) mutable {
            if (!result.isOk()) 
            {
                if (reloadIconTries < 3) {
                    log::info("failed to load icon, retrying... (attempt {}/3)", reloadIconTries + 1); 
                    icon->loadFromUrl(tp->IconURL, geode::LazySprite::Format::kFmtPng);
                    reloadIconTries += 1;
                } else {
                    log::error("failed to load icon after 3 attempts, hiding icon.");
                    icon->setVisible(false);
                }
            }
        }
    );
    this->m_mainLayer->addChild(icon);
    icon->loadFromUrl(tp->IconURL, geode::LazySprite::Format::kFmtPng);
    icon->setScale(0.6 * scale);
    icon->setPosition({ 45, 189.5 });
    icon->setZOrder(1);

    if (tp->featured) {
        auto featuredSpr = CCSprite::createWithSpriteFrameName("TMS_Featured.png"_spr);
        featuredSpr->setScale(0.6);
        featuredSpr->setPosition({ 45, 189.5 });
        this->m_mainLayer->addChild(featuredSpr);
    }

    auto line = CCSprite::createWithSpriteFrameName("floorLine_001.png");
    this->m_mainLayer->addChild(line);
    line->setPosition(this->m_mainLayer->getContentSize() / 2);
    line->setPositionY(line->getPositionY() + 40);
    line->setScale(0.675);

    std::string fullDesc = fmt::format("# {}\n{}", tp->TPName, tp->TPDescription);

    if (!tp->ThumbnailURL.empty()) {
        float thumbMaxW = 240;
        float thumbMaxH = 100;

        LazySprite* thumbnail = LazySprite::create({thumbMaxW, thumbMaxH});
        thumbnail->setAnchorPoint({0.5, 0.5});

        thumbnail->setLoadCallback(
            [this, reloadThumbnailTries = 0, tp, thumbnail, thumbMaxW, thumbMaxH](Result<> result) mutable {
                if (!result.isOk()) 
                {
                    if (reloadThumbnailTries < 3) {
                        log::info("failed to load thumbnail, retrying... (attempt {}/3)", reloadThumbnailTries + 1); 
                        thumbnail->loadFromUrl(tp->ThumbnailURL, geode::LazySprite::Format::kFmtPng);
                        reloadThumbnailTries += 1;
                    } else {
                        log::error("failed to load thumbnail after 3 attempts, hiding thumbnail.");
                        thumbnail->setVisible(false);
                    }
                    return;
                }

                auto size = thumbnail->getContentSize();
                float scale = std::min(thumbMaxW / size.width, thumbMaxH / size.height);
                scale = std::min(scale, 1.0f);
                thumbnail->setScale(scale);
            }
        );
        thumbnail->loadFromUrl(tp->ThumbnailURL, geode::LazySprite::Format::kFmtPng);

        this->m_mainLayer->addChild(thumbnail);
        thumbnail->setPosition({167.5, 165});
    }

    auto desc = MDTextArea::create(fullDesc, ccp(300, 90));
    this->m_mainLayer->addChild(desc);
    desc->setPosition(line->getPosition());
    desc->setPositionY(desc->getPositionY() - 130);

    nameLabel = CCLabelBMFont::create(tp->TPName.c_str(), "bigFont.fnt");
    nameLabel->setScale(0.5);
    nameLabel->setAnchorPoint(ccp(0, 0.5f));
    nameLabel->setPosition({76.5, 206.5}); // OH BOY I LOVE HARDCODING!
    nameLabel->setZOrder(1);
    nameLabel->limitLabelWidth(300, 0.45, 0.3);
    this->m_mainLayer->addChild(nameLabel);

    auto creatorLabel = CCLabelBMFont::create(fmt::format("By: {}", tp->TPCreator).c_str(), "goldFont.fnt");
    creatorLabel->setScale(0.45);
    creatorLabel->setAnchorPoint(ccp(0, 0.5f));
    creatorLabel->setPosition({76.5, 192.5}); // OH BOY I LOVE HARDCODING!
    creatorLabel->setZOrder(1);
    this->m_mainLayer->addChild(creatorLabel);

    versionLabel = CCLabelBMFont::create(formatTPVersion(tp->TPVersion).c_str(), "bigFont.fnt");
    versionLabel->setScale(0.3);
    versionLabel->setAnchorPoint(ccp(0, 1));
    versionLabel->setPosition({76.5, 183.5}); // OH BOY I LOVE HARDCODING!
    versionLabel->setColor({51, 153, 255});
    versionLabel->setZOrder(1);
    this->m_mainLayer->addChild(versionLabel);

    auto buttonMenu = CCMenu::create();
    buttonMenu->setContentSize(this->getContentSize());
    buttonMenu->setAnchorPoint(ccp(0, 1));
    buttonMenu->setPosition(0,0);
    this->m_mainLayer->addChild(buttonMenu);

    auto tpDownloadSpr = CCSprite::createWithSpriteFrameName("TMS_DownloadButton.png"_spr);
    tpDownloadSpr->setScale(.325);
    tpDownload = CCMenuItemSpriteExtra::create(
        tpDownloadSpr,
        this,
        menu_selector(TMSPackInfo::onDownload)
    );
    buttonMenu->addChild(tpDownload);
    tpDownload->setPosition({ 321, 217 });

    auto tpDeleteSpr = CCSprite::createWithSpriteFrameName("TMS_DeleteButton.png"_spr);
    tpDeleteSpr->setScale(.325);
    tpDelete = CCMenuItemSpriteExtra::create(
        tpDeleteSpr,
        this,
        menu_selector(TMSPackInfo::onDelete)
    );
    buttonMenu->addChild(tpDelete);
    tpDelete->setPosition({ 321, 217 });
    tpDelete->setVisible(false);

    texturePack->downloadingIndicatorInfoPage = Slider::create(this, nullptr);
    this->m_mainLayer->addChild(texturePack->downloadingIndicatorInfoPage);
    texturePack->downloadingIndicatorInfoPage->m_groove->setPosition(tpDownload->getPosition());
    texturePack->downloadingIndicatorInfoPage->m_groove->setScale(0.575);
    texturePack->downloadingIndicatorInfoPage->m_groove->setAnchorPoint({1, 0.5});
    texturePack->downloadingIndicatorInfoPage->m_touchLogic->setVisible(false);
    texturePack->downloadingIndicatorInfoPage->setVisible(false);

    if (std::filesystem::exists(fmt::format("{}/packs/{}.zip", Loader::get()->getInstalledMod("geode.texture-loader")->getConfigDir(), texturePack->TPName))) {
        if (texturePack->TPVersion != Mod::get()->getSavedValue<std::string>(texturePack->TPName)) {
            nameLabel->setColor({ 181, 219, 255 });
            tpDelete->setVisible(false);
            tpDownload->setVisible(true);
        } else {
            tpDelete->setVisible(true);
            tpDownload->setVisible(false);

            nameLabel->setColor(ccc3(216, 255, 223));
        }
    }

    return true;
}

void TMSPackInfo::onDownload(CCObject* sender) {
    //if (texturePack->isDownloading()) return;
    texturePack->downloadingIndicatorInfoPage->setVisible(true);
    tpDownload->setVisible(false);
    texturePack->cell->updateDownloadStata();
    texturePack->downloadTP();
}

void TMSPackInfo::onDelete(CCObject* sender) {
    //if (texturePack->isDownloading()) return;
    geode::createQuickPopup(
        "Delete Pack",
        fmt::format("Are you sure you want to delete {}?", texturePack->TPName),
        "Nope", "Yeah",
        [this](auto, bool btn2) {
            if (btn2) {
                std::string fileName = fmt::format("{}/packs/{}.zip", Loader::get()->getInstalledMod("geode.texture-loader")->getConfigDir(), texturePack->TPName);
                std::filesystem::remove(fileName);
                Notification::create(fmt::format("Deleted {}!", texturePack->TPName), CCSprite::createWithSpriteFrameName("GJ_completesIcon_001.png"))->show();
                nameLabel->setColor({ 255, 255, 255 });
                tpDelete->setVisible(false);
                tpDownload->setVisible(true);
                texturePack->cell->gradient->removeFromParentAndCleanup(true);
                texturePack->cell->tpDownload->setVisible(true);
                texturePack->cell->tpDelete->setVisible(false);
                texturePack->cell->setOpacity((texturePack->cell->lighterColor) ? 100 : 50); 
                texturePack->cell->gradient = nullptr;
                texturePack->downloadSuccessful = false;
            }
        }
    );
}

void TMSPackInfo::updateDownloadStata() {
    if (texturePack->downloadSuccessful) {
        tpDownload->setVisible(false);
        texturePack->downloadingIndicatorInfoPage->setVisible(false);
        tpDelete->setVisible(true);
    } else {
        tpDownload->setVisible(true);
        texturePack->downloadingIndicatorInfoPage->setVisible(false);
        tpDelete->setVisible(false);
    }
}


TMSPackInfo::~TMSPackInfo()
{
    texturePack->downloadingIndicatorInfoPage = nullptr;
    texturePack->info = nullptr;
}