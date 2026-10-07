#include <Geode/Geode.hpp>

#include <Geode/modify/ShareCommentLayer.hpp>
#include <Geode/modify/LevelInfoLayer.hpp>
#include <Geode/modify/CommentCell.hpp>
#include <regex>
#include <string>
#include "popup.hpp"
#include "sharedVars.hpp"


using namespace geode::prelude;

class $modify(MyShareCommentLayer, ShareCommentLayer) {

	struct Fields {
		bool m_ignoreAttempts;
	};

	bool init(gd::string title, int charLimit, CommentType type, int ID, gd::string desc) {
		if (!ShareCommentLayer::init(title, charLimit, type, ID,  desc)) return false;

		if (type == CommentType::Level) {
			sEnabledAttempts = false;
			sEnabledPercent = false;
			
			auto originalToggle = m_buttonMenu->getChildByType<CCMenuItemToggler>();
			if (!originalToggle) return true;

			originalToggle->setVisible(false);
				
			

			
			if (!m_percentLabel) return true;
			m_percentLabel->setVisible(false);
			

			CCSprite* buttonSprite = CCSprite::createWithSpriteFrameName("accountBtn_settings_001.png");
			buttonSprite->setScale(0.4f);
			auto extraSettingsButton = CCMenuItemSpriteExtra::create( 
				buttonSprite,
				this,
				menu_selector(MyShareCommentLayer::onMoreSettings)
			);

			m_buttonMenu->addChild(extraSettingsButton);
			extraSettingsButton->setID("extraSettingsButton"_spr);
			extraSettingsButton->setPosition(originalToggle->getPosition());
		}

		return true;
	}

	void onMoreSettings(CCObject* sender) {
		auto popup = ExtraCommentSettingsPopup::create();
		popup->show();
	}

	void onShare(CCObject* sender) {

		if (m_commentType == CommentType::Level && sEnabledAttempts == true) {
			m_percentEnabled = sEnabledPercent;
			if (m_fields->m_ignoreAttempts == true) {
				ShareCommentLayer::onShare(sender);
				m_fields->m_ignoreAttempts = false;
				return;
			}

			LevelInfoLayer* infoLayer = CCDirector::get()->getRunningScene()->getChildByType<LevelInfoLayer>();
			if (infoLayer) {
				int attempts = infoLayer->m_level->m_attempts;

				std::string oldComment = m_descText;
				std::string newComment = fmt::format("{} ({} att)", m_descText, geode::utils::numToString(attempts));
				m_descText = newComment;

				if (m_descText.size() > m_charLimit) {
					geode::createQuickPopup(
						"Uh Oh!",
						"Your comment is <cr>too long</c> to post with <cy>your attempt count embedded</c>. Please <cb>go back and edit your comment</c> or <cg>post without attempt count</c>",
						"Go back", "Post without attempt count",
						[this, oldComment, sender](auto, bool btn2) {
							if (btn2) {
								m_descText = oldComment;
								m_fields->m_ignoreAttempts = true;
								ShareCommentLayer::onShare(sender);
							} else {
								return;
							}
						}
						
					);
				} else {
					ShareCommentLayer::onShare(sender);
				}
			}
			
		} else if (m_commentType == CommentType::Level){

			std::regex attRegex(R"(\s*\((\d+) att\))");
			std::string commentText = m_descText;
			std::smatch match;
		
			if (std::regex_search(commentText, match, attRegex)) { // bypass detector (yes yes you can bypass without the mod but theres no real way to prevent againt that afaik)
				commentText.erase(match[0].first, match[0].second);
				m_descText = commentText;

			}
			
			ShareCommentLayer::onShare(sender);

		}
		
		
	}


};

class $modify(MyCommentCell, CommentCell) {

	static void onModify(ModifyBase<ModifyDerive<MyCommentCell, CommentCell>>& self) {
        (void) self.setHookPriorityBeforePost("CommentCell::loadFromComment", "thesillydoggo.comment_emojis"); // this was stolen from hiimjustin000.developer_badges
        (void) self.setHookPriorityBeforePost("CommentCell::loadFromComment", "prevter.comment_emojis"); // this was stolen from hiimjustin000.developer_badges
		(void) self.setHookPriorityBeforePost("CommentCell::loadFromComment", "hiimjustin000.developer_badges");
		(void) self.setHookPriority("CommentCell::loadFromComment", geode::Priority::LastPost);
    }


	void loadFromComment(GJComment* comment) {
		if (!comment || comment->m_isSpam) {
			CommentCell::loadFromComment(comment);
			return;
		}
		std::regex attRegex(R"(\s*\((\d+) att\))");
		std::string commentText = comment->m_commentString;
		std::smatch match;

		bool hasAttempts = false;
		std::string matchCount = "";

		if (std::regex_search(commentText, match, attRegex)) {
			hasAttempts = true;
			matchCount = match[1].str();
			commentText.erase(match[0].first, match[0].second);
			comment->m_commentString = commentText;
    	}

		CommentCell::loadFromComment(comment);

		auto usernameMenu = m_mainLayer->querySelector("main-menu > user-menu > username-menu");
		if (!usernameMenu) return;

				
		if (hasAttempts) {
			auto usernameMenu = m_mainLayer->querySelector("main-menu > user-menu > username-menu");
			if (usernameMenu) {
				auto toSet = matchCount + " attempts";
				auto attemptLabel = CCLabelBMFont::create(toSet.c_str(), "chatFont.fnt");

				attemptLabel->setColor(ccColor3B(0, 0, 0));
				attemptLabel->setID("attemptLabel"_spr);
				attemptLabel->setOpacity(150);
				attemptLabel->setScale(0.480f);

				usernameMenu->addChild(attemptLabel);
				usernameMenu->updateLayout();
			}
		}		
	}

};
