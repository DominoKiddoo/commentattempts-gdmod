#include <Geode/Geode.hpp>

#include <Geode/modify/ShareCommentLayer.hpp>
#include <Geode/modify/LevelInfoLayer.hpp>
#include <Geode/modify/CommentCell.hpp>
#include <regex>
#include <string>

using namespace geode::prelude;

class $modify(MyShareCommentLayer, ShareCommentLayer) {

	struct Fields {
		bool m_addPercent = false;
		bool m_ignoreAttempts;
	};

	bool init(gd::string title, int charLimit, CommentType type, int ID, gd::string desc) {
		if (!ShareCommentLayer::init(title, charLimit, type, ID,  desc)) return false;

		if (type == CommentType::Level) {
			log::info("skibidi skibidi toiler");
			auto bg = m_mainLayer->getChildByType<CCScale9Sprite>();
			bg->setContentHeight(153);
			bg->setPositionY(bg->getPositionY() - 12);


			auto label = CCLabelBMFont::create("Attempts", "chatFont.fnt");
			label->setScale(0.8f);
			label->setOpacity(125);
			label->setColor(ccColor3B(0, 0, 0));
			m_mainLayer->addChild(label);
			label->setID("attemptCheckboxLabel"_spr);
			label->setAnchorPoint(CCPoint(0, 0.5f));
			label->setPosition(CCPoint(118, 170));


			auto toggleSpr1 = CCSprite::createWithSpriteFrameName("GJ_checkOn_001.png");
			auto toggleSpr2 = CCSprite::createWithSpriteFrameName("GJ_checkOff_001.png");
			toggleSpr1->setScale(0.6f); toggleSpr2->setScale(0.6f);

			CCMenuItemToggler* attToggle = CCMenuItemExt::createTogglerWithStandardSprites(
				0.6f,
				[this, label](CCMenuItemToggler* sender) {
					auto on = !sender->isToggled();
					log::info("on: {}", on);
					m_fields->m_addPercent = on;
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
			
			attToggle->toggle(false);
			attToggle->setID("attemptToggle"_spr);
			m_buttonMenu->addChild(attToggle);
			attToggle->setPosition(CCPoint(-180, 10.5));
			attToggle->setID("attemptToggle"_spr);

		}

		return true;
	}

	void onShare(CCObject* sender) {
		if (m_commentType == CommentType::Level && m_fields->m_addPercent == true) {
			if (m_fields->m_ignoreAttempts == true) {
				ShareCommentLayer::onShare(sender);
				m_fields->m_ignoreAttempts = false;
				return;
			}

			LevelInfoLayer* infoLayer = CCDirector::get()->getRunningScene()->getChildByType<LevelInfoLayer>();
			if (infoLayer) {
				int attempts = infoLayer->m_level->m_attempts;

				std::string oldComment = m_descText;
				std::string newComment = fmt::format("{}({} att)", m_descText, geode::utils::numToString(attempts));
				log::info("new TUFF comment: {}", newComment);
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

			std::regex attRegex(R"(\((\d+) att\))");
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
        (void)self.setHookPriorityBeforePost("CommentCell::loadFromComment", "thesillydoggo.comment_emojis"); // this was stolen from hiimjustin000.developer_badges
        (void)self.setHookPriorityBeforePost("CommentCell::loadFromComment", "prevter.comment_emojis"); // this was stolen from hiimjustin000.developer_badges
		(void)self.setHookPriorityBeforePost("CommentCell::loadFromComment", "hiimjustin000.developer_badges");
    }


	void loadFromComment(GJComment* comment) {
		if (!comment) {
			CommentCell::loadFromComment(comment);
			return;
		}


		CommentCell::loadFromComment(comment);

		auto usernameMenu = m_mainLayer->querySelector("main-menu > user-menu > username-menu");
		if (!usernameMenu) return;

		std::regex attRegex(R"(\((\d+) att\))");
		std::string commentText = comment->m_commentString;
		std::smatch match;
		
		if (std::regex_search(commentText, match, attRegex)) {
			log::info("comment contains attempts! match {}", match[0].str());
			
			commentText.erase(match[0].first, match[0].second);
			comment->m_commentString = commentText;
			std::string matchString = match[0].str();
			
			auto toSet = match[1].str() + " attempts";
			auto attemptLabel = CCLabelBMFont::create(toSet.c_str(), "chatFont.fnt");
			attemptLabel->setColor(ccColor3B(0, 0, 0));
			attemptLabel->setID("attemptLabel"_spr);
			attemptLabel->setOpacity(150);
			attemptLabel->setScale(0.480);
			usernameMenu->addChild(attemptLabel);
			usernameMenu->updateLayout();

			
		}
		
	}

};
