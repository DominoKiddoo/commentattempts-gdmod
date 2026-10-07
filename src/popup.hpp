#pragma once

#include <Geode/Geode.hpp>
#include "sharedVars.hpp"
using namespace geode::prelude;

class ExtraCommentSettingsPopup : public geode::Popup {
protected:
    bool init() {
        if (!Popup::init(280.f, 140.f))
            return false;

        this->setTitle("Extra Comment Settings");

        auto vanillaLabel = CCLabelBMFont::create("%", "chatFont.fnt");
        vanillaLabel->setScale(0.8f);
        vanillaLabel->setOpacity(125);
        vanillaLabel->setColor(ccColor3B(0, 0, 0));
        m_mainLayer->addChild(vanillaLabel);
        vanillaLabel->setID("attemptCheckboxLabel"_spr);
        vanillaLabel->setAnchorPoint(CCPoint(0, 0.5f));
        vanillaLabel->setPosition(CCPoint(37, 81.5f));


        auto toggleSpr1 = CCSprite::createWithSpriteFrameName("GJ_checkOn_001.png");
        auto toggleSpr2 = CCSprite::createWithSpriteFrameName("GJ_checkOff_001.png");
        toggleSpr1->setScale(0.6f); toggleSpr2->setScale(0.6f);

        CCMenuItemToggler* percentToggle = CCMenuItemExt::createTogglerWithStandardSprites(
            0.6f,
            [this, vanillaLabel](CCMenuItemToggler* sender) {
                auto on = !sender->isToggled();
                sEnabledPercent = on;
                ShareCommentLayer* scLayer = CCDirector::get()->getRunningScene()->getChildByType<ShareCommentLayer>();
                if (on) {
                    vanillaLabel->setColor(ccColor3B(255, 255, 255));
                    vanillaLabel->setOpacity(255);
                    
                    scLayer->m_percentEnabled = true;
                    if (!scLayer) return;

                    auto newString = fmt::format("{}%", geode::utils::numToString(scLayer->m_percent));
                    vanillaLabel->setString(newString.c_str());
                } else {
                    scLayer->m_percentEnabled = false;
                    vanillaLabel->setColor(ccColor3B(0, 0, 0));
                    vanillaLabel->setOpacity(125);
                    vanillaLabel->setString("%");
                }
            });
        
        percentToggle->toggle(sEnabledPercent);
        percentToggle->setID("attemptToggle"_spr);
        m_buttonMenu->addChild(percentToggle);
        percentToggle->setPosition(CCPoint(20, 81.5f));
        percentToggle->setID("attemptToggle"_spr);




        // my custom things :3
        auto label = CCLabelBMFont::create("Attempts", "chatFont.fnt");
        label->setScale(0.8f);
        label->setOpacity(125);
        label->setColor(ccColor3B(0, 0, 0));
        m_mainLayer->addChild(label);
        label->setID("attemptCheckboxLabel"_spr);
        label->setAnchorPoint(CCPoint(0, 0.5f));
        label->setPosition(CCPoint(37, 34.f));

        CCMenuItemToggler* attToggle = CCMenuItemExt::createTogglerWithStandardSprites(
            0.6f,
            [this, label](CCMenuItemToggler* sender) {
                auto on = !sender->isToggled();
                sEnabledAttempts = on;

                if (on) {
                    label->setColor(ccColor3B(255, 255, 255));
                    label->setOpacity(255);

                    LevelInfoLayer* infoLayer = CCDirector::get()->getRunningScene()->getChildByType<LevelInfoLayer>();
                    if (!infoLayer) return;
                    label->setString(fmt::format("{} Attempts", infoLayer->m_level->m_attempts).c_str());
                } else {
                    label->setColor(ccColor3B(0, 0, 0));
                    label->setOpacity(125);
                    label->setString("Attempts");
                }
            });
        
        attToggle->toggle(sEnabledAttempts);
        attToggle->setID("attemptToggle"_spr);
        m_buttonMenu->addChild(attToggle);
        attToggle->setPosition(CCPoint(20, 34.f));
        attToggle->setID("attemptToggle"_spr);
        
        if (sEnabledPercent) {
            vanillaLabel->setColor(ccColor3B(255, 255, 255));
            vanillaLabel->setOpacity(255);
            
            ShareCommentLayer* scLayer = CCDirector::get()->getRunningScene()->getChildByType<ShareCommentLayer>();
            if (!scLayer) return true;

            auto newString = fmt::format("{}%", geode::utils::numToString(scLayer->m_percent));
            vanillaLabel->setString(newString.c_str());
        }

        if (sEnabledAttempts) {
            label->setColor(ccColor3B(255, 255, 255));
            label->setOpacity(255);

            LevelInfoLayer* infoLayer = CCDirector::get()->getRunningScene()->getChildByType<LevelInfoLayer>();
            if (!infoLayer) return true;
            label->setString(fmt::format("{} Attempts", infoLayer->m_level->m_attempts).c_str());
        }


        return true;
    }

public:

    static ExtraCommentSettingsPopup* create() {
        auto ret = new ExtraCommentSettingsPopup();
        if (ret->init()) {
            ret->autorelease();
            return ret;
        }

        delete ret;
        return nullptr;
    }
};
