#include <chrono>
#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/PlayerObject.hpp>

using namespace geode::prelude;

static CCLabelBMFont *cpsLabel = nullptr;
static int clicks = 0;
static auto lastTime = std::chrono::steady_clock::now();

enum class CPSPosition
{
    BottomRight,
    TopRight,
    BottomLeft
};

static CPSPosition cpsPos = CPSPosition::BottomRight;

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

void positionLabel()
{
    if (!cpsLabel)
        return;

    auto winSize = CCDirector::sharedDirector()->getWinSize();

    switch (cpsPos)
    {
    case CPSPosition::BottomRight:
        cpsLabel->setAnchorPoint({1.f, 0.f});
        cpsLabel->setPosition({winSize.width - 10.f, 10.f});
        break;

    case CPSPosition::TopRight:
        cpsLabel->setAnchorPoint({1.f, 1.f});
        cpsLabel->setPosition({winSize.width - 10.f, winSize.height - 10.f});
        break;

    case CPSPosition::BottomLeft:
        cpsLabel->setAnchorPoint({0.f, 0.f});
        cpsLabel->setPosition({10.f, 10.f});
        break;
    }
}

void addCPSLabel(CCNode *parent)
{
    cpsLabel = CCLabelBMFont::create("CPS: 0", "chatFont.fnt");
    cpsLabel->setScale(0.4f);
    cpsLabel->setZOrder(1000);

    positionLabel();
    parent->addChild(cpsLabel);
}

class $modify(CPSInit, PlayLayer)
{
    bool init(GJGameLevel *level, bool p1, bool p2)
    {
        if (!PlayLayer::init(level, p1, p2))
            return false;

        clicks = 0;
        lastTime = std::chrono::steady_clock::now();
        addCPSLabel(this);
        return true;
    }

    void update(float dt)
    {
        PlayLayer::update(dt);
        updateCPS();
    }
};

class $modify(CPSInput, PlayerObject)
{
    void pushButton(PlayerButton btn)
    {
        PlayerObject::pushButton(btn);
        registerClick();
    }
};
