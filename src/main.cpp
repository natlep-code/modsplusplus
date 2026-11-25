
#include <Geode/modify/MenuLayer.hpp>
#include <Geode/utils/general.hpp>
#include <Geode/utils/file.hpp>
#include <Geode/utils/web.hpp>
#include <Geode/ui/Button.hpp>
#include <Geode/ui/Popup.hpp>
#include <Geode/Geode.hpp>


using namespace geode::prelude;


static async::TaskHolder<web::WebResponse> s_holder;

class $modify(MyMenuLayer, MenuLayer) {
  bool init() {
    if(!MenuLayer::init()) return false;
    auto spritebtn = CCSprite::create("Btn.png"_spr);
    spritebtn->setScale(0.7f);
  
    auto url = Mod::get()->getSettingValue<std::string>("string-url");// url 
    auto folder = Mod::get()->getSettingValue<asp::fs::Path>("folder-mods"); //folder mods




    auto btn = geode::Button::createWithNode(spritebtn, [url, folder](geode::Button*) {
      geode::createQuickPopup( //warn the user
        "Warning",
        "Mods that are not on Geode may contain viruses or malware, paste a url contain .geode in the url into mod++ Settings",
        "Cancel", "Download",
      [url, folder](FLAlertLayer*, bool btn2) {
        
        if (btn2) {// the all code for web request for download the .geode in the folxer of mods
          std::string name = url.substr(url.find_last_of("/") + 1);
          auto path = folder / name;

          web::WebRequest req = web::WebRequest();

          s_holder.spawn("download mods", req.get(url), [path](web::WebResponse result) {
            if (result.ok()) {
              if (utils::file::writeBinary(path, result.data()).isOk()) {
                geode::createQuickPopup(
                  "success(Mod++)",
                  "mod downloaded, restart geode for active the mod",
                  "Later", "Restart",
                  [](FLAlertLayer*, bool btn3) {
                    if (btn3) {
                      geode::utils::game::restart(true);
                    }
                  }
                );
                Notification::create("mod downloaded, restart geode for activate the mod", NotificationIcon::Success, 2.f)->show();
                log::info("mod downloaded");
              } else {
                FLAlertLayer::create("error", "unable to write file", "Ok")->show();
                Notification::create("error", NotificationIcon::Error, 2.f)->show();
              }//notif for error write
            } else {
              FLAlertLayer::create("error", "download failed", "Ok")->show();
              Notification::create("error", NotificationIcon::Error, 2.f)->show();
            }
          });
        }
      }
    );
    });
          
    if (auto menu = this->getChildByID("bottom-menu")) {
      menu->addChild(btn);
      menu->updateLayout();
    }
    return true;
  }
};
