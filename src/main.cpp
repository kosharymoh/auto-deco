#include <Geode/Geode.hpp>
#include <Geode/modify/EditorPauseLayer.hpp>
#include <Geode/modify/EditorUI.hpp>
#include <Geode/loader/GameEvent.hpp>
#include <Geode/loader/SettingV3.hpp>

#include "Bridge.hpp"
#include "DecoPopup.hpp"

using namespace geode::prelude;

namespace {

void openAutoDeco() {
    if (!autodeco::editorIsOpen()) return;
    if (DecoPopup::isOpen()) return;
    if (auto popup = DecoPopup::create()) popup->show();
}

}  // namespace

// -----------------------------------------------------------------------------
//  Floating button inside the level editor
// -----------------------------------------------------------------------------
class $modify(AutoDecoEditorUI, EditorUI) {
    bool init(LevelEditorLayer* editorLayer) {
        if (!EditorUI::init(editorLayer)) return false;

        auto mod = Mod::get();
        if (!mod->getSettingValue<bool>("show-editor-button")) return true;

        auto where = mod->getSettingValue<std::string>("button-position");
        if (where == "Hidden") return true;

        auto win = CCDirector::sharedDirector()->getWinSize();
        CCPoint pos(34.f, win.height - 75.f);
        if (where == "Top Right") pos = CCPoint(win.width - 34.f, win.height - 75.f);
        else if (where == "Left") pos = CCPoint(34.f, win.height * 0.5f);
        else if (where == "Right") pos = CCPoint(win.width - 34.f, win.height * 0.5f);
        pos.x += static_cast<float>(mod->getSettingValue<int64_t>("button-offset-x"));
        pos.y += static_cast<float>(mod->getSettingValue<int64_t>("button-offset-y"));

        auto sprite = ButtonSprite::create("Deco", "goldFont.fnt", "GJ_button_01.png", 0.7f);
        auto button = CCMenuItemSpriteExtra::create(sprite, this, menu_selector(AutoDecoEditorUI::onAutoDeco));
        button->setID("auto-deco-button");

        auto menu = CCMenu::create();
        menu->setID("auto-deco-menu");
        menu->setPosition({0.f, 0.f});
        button->setPosition(pos);
        menu->addChild(button);
        this->addChild(menu, 100);

        return true;
    }

    void onAutoDeco(CCObject*) {
        openAutoDeco();
    }
};

// -----------------------------------------------------------------------------
//  Button in the editor pause menu (handy on mobile / if the floating one is hidden)
// -----------------------------------------------------------------------------
class $modify(AutoDecoPauseLayer, EditorPauseLayer) {
    void customSetup() {
        EditorPauseLayer::customSetup();

        auto win = CCDirector::sharedDirector()->getWinSize();
        auto sprite = ButtonSprite::create("Auto Deco", "goldFont.fnt", "GJ_button_01.png", 0.6f);
        auto button = CCMenuItemSpriteExtra::create(sprite, this, menu_selector(AutoDecoPauseLayer::onAutoDeco));
        button->setID("auto-deco-pause-button");
        button->setPosition({win.width - 70.f, 36.f});

        auto menu = CCMenu::create();
        menu->setID("auto-deco-pause-menu");
        menu->setPosition({0.f, 0.f});
        menu->addChild(button);
        this->addChild(menu, 10);
    }

    void onAutoDeco(CCObject*) {
        openAutoDeco();
    }
};

// -----------------------------------------------------------------------------
//  Keybind (desktop): set in the mod's settings, default Ctrl+Shift+D / Cmd+Shift+D
// -----------------------------------------------------------------------------
$on_game(Loaded) {
    listenForKeybindSettingPresses("open-popup-keybind", [](Keybind const&, bool down, bool repeat, double) {
        if (down && !repeat) openAutoDeco();
    });
}
