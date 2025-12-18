#include <chrono>
#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/GJBaseGameLayer.hpp>

using namespace geode::prelude;

static CCLabelBMFont *cpsLabel = nullptr;
static int clicks = 0;
static auto lastTime = std::chrono::steady_clock::now();

void registerClick()
{
    clicks++;
}

void updateCPS()
{
    auto now = std::chrono::steady_clock::now();
    std::chrono::duration<double> elapsed = now - lastTime;

    if (elapsed.count() >= 1.0 && cpsLabel)
    {
        double cps = clicks / elapsed.count();
        cpsLabel->setString(fmt::format("CPS: {:.1f}", cps).c_str());
        clicks = 0;
        lastTime = now;
    }
}

void addBottomRightLabel(CCNode *parent)
{
    auto winSize = CCDirector::sharedDirector()->getWinSize();

    cpsLabel = CCLabelBMFont::create("CPS: 0", "chatFont.fnt");
    cpsLabel->setAnchorPoint({1.f, 0.f});
    cpsLabel->setPosition({winSize.width - 10.f, 10.f});
    cpsLabel->setScale(0.4f);
    cpsLabel->setZOrder(1000);

    parent->addChild(cpsLabel);
}

class $modify(CPSPlayLayer, PlayLayer)
{
    void update(float dt)
    {
        PlayLayer::update(dt);
        updateCPS();
    }
};

class $modify(CPSInput, GJBaseGameLayer)
{
    void handleButton(bool down, int button, bool isPlayer1)
    {
        GJBaseGameLayer::handleButton(down, button, isPlayer1);

        if (down)
        {
            registerClick();
        }
    }
};

class $modify(CPSInit, PlayLayer)
{
    bool init(GJGameLevel *level, bool p1, bool p2)
    {
        if (!PlayLayer::init(level, p1, p2))
            return false;

        addBottomRightLabel(this);
        return true;
    }
};
