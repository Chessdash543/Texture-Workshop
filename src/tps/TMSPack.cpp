using namespace geode::prelude;

#include "headers/TMSPack.hpp"
#include "../ui/headers/TMSPackCell.hpp"
#include "../ui/headers/TMSPackInfo.hpp"
#include "../header/boobs.hpp"

void TMSPack::downloadTP() {
    isDownloading = true;
    boobs::downloading.push_back(this);
    if (cell) {
        cell->tpDownload->setVisible(false);
        downloadingIndicator->setVisible(true);
    }

    if (info) {
        info->tpDownload->setVisible(false);
        downloadingIndicatorInfoPage->setVisible(true);
    }
    auto weak = geode::WeakRef(this); // weakref is fucking bullshit god GEODE IS GOING TO MAKE ME KILL MYSELF

    auto req = geode::utils::web::WebRequest();

    req.onProgress([this, weak](geode::utils::web::WebProgress const& progress) {
        if (downloadingIndicator && weak.valid()) {
            downloadingIndicator->setValue(progress.downloadProgress().value_or(0.69420f) / 100.0f);
        }

        if (downloadingIndicatorInfoPage && weak.valid()) {
            downloadingIndicatorInfoPage->setValue(progress.downloadProgress().value_or(0.69420f) / 100.0f);
        }
    });

    m_downloadTPlistener.spawn( 
        req.get(this->DownloadURL),
        [this, weak](geode::utils::web::WebResponse res) {
            if (!weak.lock()) {
                return;
            }

            if (res.ok()) {
                if(res.into(fmt::format("{}/packs/{}.zip", Loader::get()->getInstalledMod("geode.texture-loader")->getConfigDir(), TPName))) {
                    Notification::create("Download Complete!", CCSprite::createWithSpriteFrameName("GJ_completesIcon_001.png"));
                    downloadSuccessful = true;
                    isDownloading = false;
                    boobs::downloading.erase(std::remove(boobs::downloading.begin(), boobs::downloading.end(), this), boobs::downloading.end()); // texture workshop pack sacrificing itself to kill cheeseworks
                    std::string versionSaveThing = fmt::format("{}", TPName);
                    Mod::get()->setSavedValue<std::string>(versionSaveThing, TPVersion);
                    if (info) {
                        info->tpDelete->setVisible(true);
                        downloadingIndicatorInfoPage->setVisible(false);
                    }

                    if (cell) {
                        cell->updateDownloadStata();                    
                    } else {
                        this->release(); 
                        // no it seemed like a cool idea at the time
                        // but it was costing me a shitload
                        // so i had to release them back into the wild
                    }
                }
            }
        }
    );
}