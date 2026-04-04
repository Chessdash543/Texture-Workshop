#include "./headers/TMSFilters.hpp"
#include "./headers/TMSLayer.hpp"
#include "../header/boobs.hpp"

bool TMSFilters::init()
{
    if (!Popup::init(120.f, 200.f, "TMS_Box.png"_spr))
        return false;

    this->setTitle("Sort");

    oldDownloadsValue = boobs::downloadsSort;
    oldRecentValue = boobs::recentlyUpdatedSort;
    oldFeaturedValue = boobs::featuredSort;

    auto bg = cocos2d::extension::CCScale9Sprite::create("square02_small.png");
    m_mainLayer->addChild(bg);
    bg->setPosition(m_mainLayer->getContentSize() / 2);
    bg->setPositionY(bg->getPositionY() - 13);
    bg->setContentWidth(110);
    bg->setContentHeight(160);
    bg->setOpacity(100);

    auto menu = CCMenu::create();
    menu->setContentSize(bg->getContentSize());
    m_mainLayer->addChild(menu);
    menu->setPosition(m_mainLayer->getContentSize() / 2);
    menu->setPositionY(menu->getPositionY() - 13);
    //menu->ignoreAnchorPointForPosition(true); // i cant even lie i hate using this but  it gotta be done.. nvm 

    featuredToggle = CCMenuItemToggler::createWithStandardSprites(
        this,
        menu_selector(TMSFilters::onToggle),
        1
    );
    menu->addChild(featuredToggle);
    featuredToggle->setPosition(menu->getContentSize() / 2);
    featuredToggle->setPositionY(featuredToggle->getPositionY() + 40);
    featuredToggle->setID("featured-filter");
    featuredToggle->setTag(92210);

    downloadsToggle = CCMenuItemToggler::createWithStandardSprites(
        this,
        menu_selector(TMSFilters::onToggle),
        1
    );
    menu->addChild(downloadsToggle);
    downloadsToggle->setPosition(menu->getContentSize() / 2);
    downloadsToggle->setID("downloads-filter");
    downloadsToggle->setTag(92211);

    recentToggle = CCMenuItemToggler::createWithStandardSprites(
        this,
        menu_selector(TMSFilters::onToggle),
        1
    );
    menu->addChild(recentToggle);
    recentToggle->setPosition(menu->getContentSize() / 2);
    recentToggle->setPositionY(recentToggle->getPositionY() - 40);
    recentToggle->setID("recent-filter");
    recentToggle->setTag(92212);

    imListeningToTheFilters = this->listenForClose().listen([this] {
        if (boobs::downloadsSort != oldDownloadsValue || boobs::recentlyUpdatedSort != oldRecentValue || boobs::featuredSort != oldFeaturedValue) {
            TMSLayer::get->getTexturePacks(boobs::search);
        }
        return ListenerResult::Propagate;
    }); // will this work? lets find out! IM ROLLING THE DICE CALL MY ASS CUPHEAD BOOYAH!
    // it infact did work.  fuck yes.

    return true;
}

void TMSFilters::onToggle(CCObject* sender) {
   switch (sender->getTag())
   {
   case 92210: // featured 
    downloadsToggle->toggle(false);
    recentToggle->toggle(false);
    break;
   case 92211: // downloads
    featuredToggle->toggle(false);
    recentToggle->toggle(false);
    break;
   case 92212: // recent
    downloadsToggle->toggle(false);
    featuredToggle->toggle(false);
    break;
   default:
    break;
   }

    boobs::featuredSort = featuredToggle->isToggled();
    boobs::downloadsSort = downloadsToggle->isToggled();
    boobs::recentlyUpdatedSort = recentToggle->isToggled();

    log::info("Featured: {}, Downloads: {}, Recent: {}", boobs::featuredSort, boobs::downloadsSort, boobs::recentlyUpdatedSort);
    log::info("Old Featured: {}, Old Downloads: {}, Old Recent: {}", oldFeaturedValue, oldDownloadsValue, oldRecentValue);
}