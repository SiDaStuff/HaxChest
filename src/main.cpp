#include <Geode/Geode.hpp>
#include <Geode/ui/Popup.hpp>
#include <Geode/ui/TextInput.hpp>

using namespace geode::prelude;

// Helper function to add rewards using consistent logic
inline void addReward(const char* statKey, int amount) {
	if (amount > 0) {
		auto gsm = GameStatsManager::sharedState();
		int current = gsm->getStat(statKey);
		gsm->setStat(statKey, current + amount);
	}
}

// Forward declaration for shards popup
class ShardsPopup;

// Custom Popup for giving rewards
class RewardPopup : public Popup<> {
protected:
	TextInput* m_orbsInput;
	TextInput* m_keysInput;
	TextInput* m_diamondsInput;
	TextInput* m_goldKeysInput;
	TextInput* m_iconIDInput;

	bool setup() override {
		this->setTitle("HaxChest");
		
		// Double column layout - adjusted positioning
		float leftColX = 50.0f;      // Left column X (moved left 100)
		float rightColX = 210.0f;    // Right column X (moved left 100)
		float labelOffsetX = 0.0f;   // Label offset
		float inputOffsetX = 80.0f;  // Input offset
		float startY = 210.0f;       // Start Y position
		float spacing = 32.0f;       // Spacing between rows
		
		// LEFT COLUMN
		// Orbs
		auto orbsLabel = CCLabelBMFont::create("Orbs:", "bigFont.fnt");
		orbsLabel->setScale(0.45f);
		orbsLabel->setPosition({leftColX + labelOffsetX, startY});
		m_mainLayer->addChild(orbsLabel);
		
		m_orbsInput = TextInput::create(80.0f, "0", "bigFont.fnt");
		m_orbsInput->setPosition({leftColX + inputOffsetX, startY});
		m_orbsInput->setString("0");
		m_orbsInput->setFilter("0123456789");
		m_orbsInput->setMaxCharCount(9);
		m_mainLayer->addChild(m_orbsInput);
		
		// Keys
		auto keysLabel = CCLabelBMFont::create("Keys:", "bigFont.fnt");
		keysLabel->setScale(0.45f);
		keysLabel->setPosition({leftColX + labelOffsetX, startY - spacing});
		m_mainLayer->addChild(keysLabel);
		
		m_keysInput = TextInput::create(80.0f, "0", "bigFont.fnt");
		m_keysInput->setPosition({leftColX + inputOffsetX, startY - spacing});
		m_keysInput->setString("0");
		m_keysInput->setFilter("0123456789");
		m_keysInput->setMaxCharCount(9);
		m_mainLayer->addChild(m_keysInput);
		
		// Diamonds
		auto diamondsLabel = CCLabelBMFont::create("Diamonds:", "bigFont.fnt");
		diamondsLabel->setScale(0.45f);
		diamondsLabel->setPosition({leftColX + labelOffsetX, startY - spacing * 2});
		m_mainLayer->addChild(diamondsLabel);
		
		m_diamondsInput = TextInput::create(80.0f, "0", "bigFont.fnt");
		m_diamondsInput->setPosition({leftColX + inputOffsetX, startY - spacing * 2});
		m_diamondsInput->setString("0");
		m_diamondsInput->setFilter("0123456789");
		m_diamondsInput->setMaxCharCount(9);
		m_mainLayer->addChild(m_diamondsInput);
		
		// RIGHT COLUMN
		// Gold Keys
		auto goldKeysLabel = CCLabelBMFont::create("Gold Keys:", "bigFont.fnt");
		goldKeysLabel->setScale(0.45f);
		goldKeysLabel->setPosition({rightColX + labelOffsetX, startY});
		m_mainLayer->addChild(goldKeysLabel);
		
		m_goldKeysInput = TextInput::create(80.0f, "0", "bigFont.fnt");
		m_goldKeysInput->setPosition({rightColX + inputOffsetX, startY});
		m_goldKeysInput->setString("0");
		m_goldKeysInput->setFilter("0123456789");
		m_goldKeysInput->setMaxCharCount(9);
		m_mainLayer->addChild(m_goldKeysInput);
		
		// Icon ID
		auto iconLabel = CCLabelBMFont::create("Icon ID:", "bigFont.fnt");
		iconLabel->setScale(0.45f);
		iconLabel->setPosition({rightColX + labelOffsetX, startY - spacing});
		m_mainLayer->addChild(iconLabel);
		
		m_iconIDInput = TextInput::create(80.0f, "0", "bigFont.fnt");
		m_iconIDInput->setPosition({rightColX + inputOffsetX, startY - spacing});
		m_iconIDInput->setString("0");
		m_iconIDInput->setFilter("0123456789");
		m_iconIDInput->setMaxCharCount(6);
		m_mainLayer->addChild(m_iconIDInput);
		
		// Buttons centered below columns (moved left 100)
		auto openBtn = CCMenuItemSpriteExtra::create(
			ButtonSprite::create("Open Chest", "goldFont.fnt", "GJ_button_01.png", 0.7f),
			this,
			menu_selector(RewardPopup::onOpenChest)
		);
		openBtn->setPosition({80.0f, startY - spacing * 3.5f});
		m_buttonMenu->addChild(openBtn);
		
		auto shardsBtn = CCMenuItemSpriteExtra::create(
			ButtonSprite::create("Shards", "goldFont.fnt", "GJ_button_01.png", 0.6f),
			this,
			menu_selector(RewardPopup::onShardsButton)
		);
		shardsBtn->setPosition({180.0f, startY - spacing * 3.5f});
		m_buttonMenu->addChild(shardsBtn);
		
		// Credits text at bottom (moved left 100)
		auto creditsLabel = CCLabelBMFont::create("by sidastuff", "goldFont.fnt");
		creditsLabel->setScale(0.35f);
		creditsLabel->setPosition({130.0f, startY - spacing * 4.5f});
		creditsLabel->setOpacity(180);
		m_mainLayer->addChild(creditsLabel);
		
		// Enable keyboard input for Shift+Home shortcut
		this->setKeyboardEnabled(true);
		this->setKeypadEnabled(true);
		
		return true;
	}
	
	void keyDown(cocos2d::enumKeyCodes key) override {
		// Check for Home key (0x24) with Shift modifier
		if (key == cocos2d::KEY_Home) {
			// Check if Shift is pressed
			auto keyboard = cocos2d::CCDirector::sharedDirector()->getKeyboardDispatcher();
			if (keyboard->getShiftKeyPressed()) {
				// Auto-run Open Chest function
				this->onOpenChest(nullptr);
				return;
			}
		}
		// Call parent implementation for other keys (like ESC to close)
		Popup::keyDown(key);
	}
	
	void onShardsButton(CCObject*); // Forward declaration, implemented after ShardsPopup is defined
	
	void onOpenChest(CCObject*) {
		// Parse input values
		int orbsAmount = 0;
		int keysAmount = 0;
		int diamondsAmount = 0;
		int goldKeysAmount = 0;
		int iconID = 0;
		
		try {
			std::string orbsStr = m_orbsInput->getString();
			std::string keysStr = m_keysInput->getString();
			std::string diamondsStr = m_diamondsInput->getString();
			std::string goldKeysStr = m_goldKeysInput->getString();
			std::string iconStr = m_iconIDInput->getString();
			
			if (!orbsStr.empty() && orbsStr != "0") orbsAmount = std::stoi(orbsStr);
			if (!keysStr.empty() && keysStr != "0") keysAmount = std::stoi(keysStr);
			if (!diamondsStr.empty() && diamondsStr != "0") diamondsAmount = std::stoi(diamondsStr);
			if (!goldKeysStr.empty() && goldKeysStr != "0") goldKeysAmount = std::stoi(goldKeysStr);
			if (!iconStr.empty() && iconStr != "0") iconID = std::stoi(iconStr);
		} catch (...) {
			FLAlertLayer::create("Error", "Invalid number format!", "OK")->show();
			return;
		}
		
		// Validate that at least one reward is being given
		if (orbsAmount <= 0 && keysAmount <= 0 && diamondsAmount <= 0 && goldKeysAmount <= 0 && iconID <= 0) {
			FLAlertLayer::create("Error", "Please enter at least one reward!", "OK")->show();
			return;
		}
		
		// Add rewards using helper function
		addReward("14", orbsAmount);     // Orbs
		addReward("13", keysAmount);     // Keys
		addReward("12", diamondsAmount); // Diamonds
		addReward("51", goldKeysAmount); // Gold Keys
		
		// Handle icon unlocking
		if (iconID > 0) {
			auto gsm = GameStatsManager::sharedState();
			UnlockType iconType = UnlockType::Cube;
			if (!gsm->isItemUnlocked(iconType, iconID)) {
				auto unlockKey = gsm->getItemKey(iconID, static_cast<int>(iconType));
				gsm->m_unlockedItems->setObject(gsm->m_trueString, unlockKey);
				gsm->m_purchasedItems->setObject(gsm->m_trueString, unlockKey);
			}
		}
		
		// Create visual reward display
		auto chestRewards = CCArray::create();
		if (orbsAmount > 0) chestRewards->addObject(GJRewardObject::create(static_cast<SpecialRewardItem>(7), orbsAmount, 0));
		if (keysAmount > 0) chestRewards->addObject(GJRewardObject::create(static_cast<SpecialRewardItem>(6), keysAmount, 0));
		if (diamondsAmount > 0) chestRewards->addObject(GJRewardObject::create(static_cast<SpecialRewardItem>(8), diamondsAmount, 0));
		if (goldKeysAmount > 0) chestRewards->addObject(GJRewardObject::create(static_cast<SpecialRewardItem>(15), goldKeysAmount, 0));
		if (iconID > 0) chestRewards->addObject(GJRewardObject::createItemUnlock(UnlockType::Cube, iconID));
		
		this->onClose(nullptr);
		
		// Show chest animation
		if (chestRewards->count() > 0) {
			auto rewardLayer = RewardUnlockLayer::create(2, nullptr);
			rewardLayer->show();
			rewardLayer->showCollectReward(GJRewardItem::createWithObjects(static_cast<GJRewardType>(2), chestRewards));
		}
	}

public:
	static RewardPopup* create() {
		auto ret = new RewardPopup();
		// Much smaller popup size
		if (ret && ret->initAnchored(360.0f, 280.0f)) {
			ret->autorelease();
			return ret;
		}
		CC_SAFE_DELETE(ret);
		return nullptr;
	}
};

// Shards Popup for giving shards
class ShardsPopup : public Popup<> {
protected:
	TextInput* m_fireShardInput;
	TextInput* m_iceShardInput;
	TextInput* m_poisonShardInput;
	TextInput* m_shadowShardInput;
	TextInput* m_lavaShardInput;
	TextInput* m_earthShardInput;
	TextInput* m_bloodShardInput;
	TextInput* m_metalShardInput;
	TextInput* m_lightShardInput;
	TextInput* m_soulShardInput;
	bool m_shouldReopenMain = false;

	bool setup() override {
		this->setTitle("Shards");
		
		// Double column layout - adjusted positioning
		float leftColX = 50.0f;   // Left column X (moved left 100)
		float rightColX = 210.0f; // Right column X (moved left 100)
		float labelOffsetX = 0.0f;   // Label offset
		float inputOffsetX = 80.0f;  // Input offset
		float startY = 240.0f;    // Start Y position
		float spacing = 28.0f;
		
		// LEFT COLUMN
		// Fire
		auto fireLabel = CCLabelBMFont::create("Fire:", "bigFont.fnt");
		fireLabel->setScale(0.4f);
		fireLabel->setPosition({leftColX + labelOffsetX, startY});
		m_mainLayer->addChild(fireLabel);
		
		m_fireShardInput = TextInput::create(75.0f, "0", "bigFont.fnt");
		m_fireShardInput->setPosition({leftColX + inputOffsetX, startY});
		m_fireShardInput->setString("0");
		m_fireShardInput->setFilter("0123456789");
		m_fireShardInput->setMaxCharCount(9);
		m_mainLayer->addChild(m_fireShardInput);
		
		// Ice
		auto iceLabel = CCLabelBMFont::create("Ice:", "bigFont.fnt");
		iceLabel->setScale(0.4f);
		iceLabel->setPosition({leftColX + labelOffsetX, startY - spacing});
		m_mainLayer->addChild(iceLabel);
		
		m_iceShardInput = TextInput::create(75.0f, "0", "bigFont.fnt");
		m_iceShardInput->setPosition({leftColX + inputOffsetX, startY - spacing});
		m_iceShardInput->setString("0");
		m_iceShardInput->setFilter("0123456789");
		m_iceShardInput->setMaxCharCount(9);
		m_mainLayer->addChild(m_iceShardInput);
		
		// Poison
		auto poisonLabel = CCLabelBMFont::create("Poison:", "bigFont.fnt");
		poisonLabel->setScale(0.4f);
		poisonLabel->setPosition({leftColX + labelOffsetX, startY - spacing * 2});
		m_mainLayer->addChild(poisonLabel);
		
		m_poisonShardInput = TextInput::create(75.0f, "0", "bigFont.fnt");
		m_poisonShardInput->setPosition({leftColX + inputOffsetX, startY - spacing * 2});
		m_poisonShardInput->setString("0");
		m_poisonShardInput->setFilter("0123456789");
		m_poisonShardInput->setMaxCharCount(9);
		m_mainLayer->addChild(m_poisonShardInput);
		
		// Shadow
		auto shadowLabel = CCLabelBMFont::create("Shadow:", "bigFont.fnt");
		shadowLabel->setScale(0.4f);
		shadowLabel->setPosition({leftColX + labelOffsetX, startY - spacing * 3});
		m_mainLayer->addChild(shadowLabel);
		
		m_shadowShardInput = TextInput::create(75.0f, "0", "bigFont.fnt");
		m_shadowShardInput->setPosition({leftColX + inputOffsetX, startY - spacing * 3});
		m_shadowShardInput->setString("0");
		m_shadowShardInput->setFilter("0123456789");
		m_shadowShardInput->setMaxCharCount(9);
		m_mainLayer->addChild(m_shadowShardInput);
		
		// Lava
		auto lavaLabel = CCLabelBMFont::create("Lava:", "bigFont.fnt");
		lavaLabel->setScale(0.4f);
		lavaLabel->setPosition({leftColX + labelOffsetX, startY - spacing * 4});
		m_mainLayer->addChild(lavaLabel);
		
		m_lavaShardInput = TextInput::create(75.0f, "0", "bigFont.fnt");
		m_lavaShardInput->setPosition({leftColX + inputOffsetX, startY - spacing * 4});
		m_lavaShardInput->setString("0");
		m_lavaShardInput->setFilter("0123456789");
		m_lavaShardInput->setMaxCharCount(9);
		m_mainLayer->addChild(m_lavaShardInput);
		
		// RIGHT COLUMN
		// Earth
		auto earthLabel = CCLabelBMFont::create("Earth:", "bigFont.fnt");
		earthLabel->setScale(0.4f);
		earthLabel->setPosition({rightColX + labelOffsetX, startY});
		m_mainLayer->addChild(earthLabel);
		
		m_earthShardInput = TextInput::create(75.0f, "0", "bigFont.fnt");
		m_earthShardInput->setPosition({rightColX + inputOffsetX, startY});
		m_earthShardInput->setString("0");
		m_earthShardInput->setFilter("0123456789");
		m_earthShardInput->setMaxCharCount(9);
		m_mainLayer->addChild(m_earthShardInput);
		
		// Blood
		auto bloodLabel = CCLabelBMFont::create("Blood:", "bigFont.fnt");
		bloodLabel->setScale(0.4f);
		bloodLabel->setPosition({rightColX + labelOffsetX, startY - spacing});
		m_mainLayer->addChild(bloodLabel);
		
		m_bloodShardInput = TextInput::create(75.0f, "0", "bigFont.fnt");
		m_bloodShardInput->setPosition({rightColX + inputOffsetX, startY - spacing});
		m_bloodShardInput->setString("0");
		m_bloodShardInput->setFilter("0123456789");
		m_bloodShardInput->setMaxCharCount(9);
		m_mainLayer->addChild(m_bloodShardInput);
		
		// Metal
		auto metalLabel = CCLabelBMFont::create("Metal:", "bigFont.fnt");
		metalLabel->setScale(0.4f);
		metalLabel->setPosition({rightColX + labelOffsetX, startY - spacing * 2});
		m_mainLayer->addChild(metalLabel);
		
		m_metalShardInput = TextInput::create(75.0f, "0", "bigFont.fnt");
		m_metalShardInput->setPosition({rightColX + inputOffsetX, startY - spacing * 2});
		m_metalShardInput->setString("0");
		m_metalShardInput->setFilter("0123456789");
		m_metalShardInput->setMaxCharCount(9);
		m_mainLayer->addChild(m_metalShardInput);
		
		// Light
		auto lightLabel = CCLabelBMFont::create("Light:", "bigFont.fnt");
		lightLabel->setScale(0.4f);
		lightLabel->setPosition({rightColX + labelOffsetX, startY - spacing * 3});
		m_mainLayer->addChild(lightLabel);
		
		m_lightShardInput = TextInput::create(75.0f, "0", "bigFont.fnt");
		m_lightShardInput->setPosition({rightColX + inputOffsetX, startY - spacing * 3});
		m_lightShardInput->setString("0");
		m_lightShardInput->setFilter("0123456789");
		m_lightShardInput->setMaxCharCount(9);
		m_mainLayer->addChild(m_lightShardInput);
		
		// Soul
		auto soulLabel = CCLabelBMFont::create("Soul:", "bigFont.fnt");
		soulLabel->setScale(0.4f);
		soulLabel->setPosition({rightColX + labelOffsetX, startY - spacing * 4});
		m_mainLayer->addChild(soulLabel);
		
		m_soulShardInput = TextInput::create(75.0f, "0", "bigFont.fnt");
		m_soulShardInput->setPosition({rightColX + inputOffsetX, startY - spacing * 4});
		m_soulShardInput->setString("0");
		m_soulShardInput->setFilter("0123456789");
		m_soulShardInput->setMaxCharCount(9);
		m_mainLayer->addChild(m_soulShardInput);
		
		// Give Shards button (moved left 100)
		auto giveShardsBtn = CCMenuItemSpriteExtra::create(
			ButtonSprite::create("Give Shards", "goldFont.fnt", "GJ_button_01.png", 0.7f),
			this,
			menu_selector(ShardsPopup::onGiveShards)
		);
		giveShardsBtn->setPosition({130.0f, startY - spacing * 5.5f});
		m_buttonMenu->addChild(giveShardsBtn);
		
		// Credits text (moved left 100)
		auto creditsLabel = CCLabelBMFont::create("by sidastuff", "goldFont.fnt");
		creditsLabel->setScale(0.35f);
		creditsLabel->setPosition({130.0f, startY - spacing * 6.5f});
		creditsLabel->setOpacity(180);
		m_mainLayer->addChild(creditsLabel);
		
		return true;
	}
	
	void onGiveShards(CCObject*) {
		// Parse all shard inputs
		int fireShard = 0, iceShard = 0, poisonShard = 0, shadowShard = 0, lavaShard = 0;
		int earthShard = 0, bloodShard = 0, metalShard = 0, lightShard = 0, soulShard = 0;
		
		try {
			std::string fireStr = m_fireShardInput->getString();
			std::string iceStr = m_iceShardInput->getString();
			std::string poisonStr = m_poisonShardInput->getString();
			std::string shadowStr = m_shadowShardInput->getString();
			std::string lavaStr = m_lavaShardInput->getString();
			std::string earthStr = m_earthShardInput->getString();
			std::string bloodStr = m_bloodShardInput->getString();
			std::string metalStr = m_metalShardInput->getString();
			std::string lightStr = m_lightShardInput->getString();
			std::string soulStr = m_soulShardInput->getString();
			
			if (!fireStr.empty() && fireStr != "0") fireShard = std::stoi(fireStr);
			if (!iceStr.empty() && iceStr != "0") iceShard = std::stoi(iceStr);
			if (!poisonStr.empty() && poisonStr != "0") poisonShard = std::stoi(poisonStr);
			if (!shadowStr.empty() && shadowStr != "0") shadowShard = std::stoi(shadowStr);
			if (!lavaStr.empty() && lavaStr != "0") lavaShard = std::stoi(lavaStr);
			if (!earthStr.empty() && earthStr != "0") earthShard = std::stoi(earthStr);
			if (!bloodStr.empty() && bloodStr != "0") bloodShard = std::stoi(bloodStr);
			if (!metalStr.empty() && metalStr != "0") metalShard = std::stoi(metalStr);
			if (!lightStr.empty() && lightStr != "0") lightShard = std::stoi(lightStr);
			if (!soulStr.empty() && soulStr != "0") soulShard = std::stoi(soulStr);
		} catch (...) {
			FLAlertLayer::create("Error", "Invalid number format!", "OK")->show();
			return;
		}
		
		// Validate at least one shard
		if (fireShard <= 0 && iceShard <= 0 && poisonShard <= 0 && shadowShard <= 0 && lavaShard <= 0 &&
		    earthShard <= 0 && bloodShard <= 0 && metalShard <= 0 && lightShard <= 0 && soulShard <= 0) {
			FLAlertLayer::create("Error", "Please enter at least one shard!", "OK")->show();
			return;
		}
		
		// Add shards using helper function
		addReward("15", fireShard);   // Fire
		addReward("16", iceShard);    // Ice
		addReward("17", poisonShard); // Poison
		addReward("18", shadowShard); // Shadow
		addReward("19", lavaShard);   // Lava
		addReward("20", earthShard);  // Earth
		addReward("21", bloodShard);  // Blood
		addReward("22", metalShard);  // Metal
		addReward("23", lightShard);  // Light
		addReward("24", soulShard);   // Soul
		
		m_shouldReopenMain = false;
		this->onClose(nullptr);
		
		// Create visual reward display
		auto chestRewards = CCArray::create();
		if (fireShard > 0) chestRewards->addObject(GJRewardObject::create(static_cast<SpecialRewardItem>(1), fireShard, 0));
		if (iceShard > 0) chestRewards->addObject(GJRewardObject::create(static_cast<SpecialRewardItem>(2), iceShard, 0));
		if (poisonShard > 0) chestRewards->addObject(GJRewardObject::create(static_cast<SpecialRewardItem>(3), poisonShard, 0));
		if (shadowShard > 0) chestRewards->addObject(GJRewardObject::create(static_cast<SpecialRewardItem>(4), shadowShard, 0));
		if (lavaShard > 0) chestRewards->addObject(GJRewardObject::create(static_cast<SpecialRewardItem>(5), lavaShard, 0));
		if (earthShard > 0) chestRewards->addObject(GJRewardObject::create(static_cast<SpecialRewardItem>(10), earthShard, 0));
		if (bloodShard > 0) chestRewards->addObject(GJRewardObject::create(static_cast<SpecialRewardItem>(11), bloodShard, 0));
		if (metalShard > 0) chestRewards->addObject(GJRewardObject::create(static_cast<SpecialRewardItem>(12), metalShard, 0));
		if (lightShard > 0) chestRewards->addObject(GJRewardObject::create(static_cast<SpecialRewardItem>(13), lightShard, 0));
		if (soulShard > 0) chestRewards->addObject(GJRewardObject::create(static_cast<SpecialRewardItem>(14), soulShard, 0));
		
		// Show chest animation
		if (chestRewards->count() > 0) {
			auto rewardLayer = RewardUnlockLayer::create(2, nullptr);
			rewardLayer->show();
			rewardLayer->showCollectReward(GJRewardItem::createWithObjects(static_cast<GJRewardType>(2), chestRewards));
		}
	}
	
	void onClose(CCObject* sender) override {
		// Reopen main popup if requested
		if (m_shouldReopenMain) {
			auto mainPopup = RewardPopup::create();
			mainPopup->show();
		}
		Popup::onClose(sender);
	}
	
	void setShouldReopenMain(bool shouldReopen) {
		m_shouldReopenMain = shouldReopen;
	}

public:
	static ShardsPopup* create(bool reopenMain = false) {
		auto ret = new ShardsPopup();
		// Compact size for shards
		if (ret && ret->initAnchored(360.0f, 340.0f)) {
			ret->autorelease();
			ret->setShouldReopenMain(reopenMain);
			return ret;
		}
		CC_SAFE_DELETE(ret);
		return nullptr;
	}
};

// Implementation of RewardPopup::onShardsButton (defined after ShardsPopup)
void RewardPopup::onShardsButton(CCObject*) {
	// Create shards popup and tell it to reopen main when closed
	auto shardsPopup = ShardsPopup::create(true);
	shardsPopup->show();
	
	// Close this main popup
	this->onClose(nullptr);
}

#include <Geode/modify/MenuLayer.hpp>
class $modify(MyMenuLayer, MenuLayer) {
	bool init() {
		if (!MenuLayer::init()) return false;

		auto myButton = CCMenuItemSpriteExtra::create(
			CCSprite::createWithSpriteFrameName("chest_03_02_001.png"),
			this,
			menu_selector(MyMenuLayer::onMyButton)
		);

		auto menu = this->getChildByID("bottom-menu");
		menu->addChild(myButton);
		myButton->setID("my-button"_spr);
		menu->updateLayout();

		return true;
	}

	void onMyButton(CCObject*) {
		RewardPopup::create()->show();
	}
};