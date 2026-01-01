#include <chrono>
#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>

#ifdef _WIN32
#include <Windows.h>
#endif

#ifdef __APPLE__
#include <ApplicationServices/ApplicationServices.h>
#include <Carbon/Carbon.h>
#endif

using namespace geode::prelude;

static CCLabelBMFont *cpsLabel = nullptr;
static int clicks = 0;
static auto lastTime = std::chrono::steady_clock::now();

#ifdef _WIN32
static bool lastLMB = false;
static bool lastSpace = false;
static bool lastUp = false;

static bool downNow(int vk)
{
    return (GetAsyncKeyState(vk) & 0x8000) != 0;
}

static void pollWindowsInput()
{
    bool lmb = downNow(VK_LBUTTON);
    if (lmb && !lastLMB)
        clicks++;
    lastLMB = lmb;

    bool sp = downNow(VK_SPACE);
    if (sp && !lastSpace)
        clicks++;
    lastSpace = sp;

    bool up = downNow(VK_UP);
    if (up && !lastUp)
        clicks++;
    lastUp = up;
}
#endif

#ifdef __APPLE__
static bool lastLMB = false;
static bool lastSpace = false;
static bool lastUp = false;

static bool keyDownMac(CGKeyCode key)
{
    return CGEventSourceKeyState(
        kCGEventSourceStateCombinedSessionState,
        key);
}

static bool mouseDownMac(CGMouseButton btn)
{
    return CGEventSourceButtonState(
        kCGEventSourceStateCombinedSessionState,
        btn);
}

static void pollMacInput()
{
    bool lmb = mouseDownMac(kCGMouseButtonLeft);
    if (lmb && !lastLMB)
        clicks++;
    lastLMB = lmb;

    bool space = keyDownMac(kVK_Space);
    if (space && !lastSpace)
        clicks++;
    lastSpace = space;

    bool up = keyDownMac(kVK_UpArrow);
    if (up && !lastUp)
        clicks++;
    lastUp = up;
}
#endif

static void updateCPS()
{
    if (!cpsLabel)
        return;

    auto now = std::chrono::steady_clock::now();
    std::chrono::duration<double> elapsed = now - lastTime;

    if (elapsed.count() >= 0.1)
    {
        double cps = clicks / elapsed.count();
        cpsLabel->setString(fmt::format("CPS: {:.1f}", cps).c_str());
        clicks = 0;
        lastTime = now;
    }
}

class CPSInputLayer : public CCLayer
{
public:
    static CPSInputLayer *create()
    {
        auto ret = new CPSInputLayer();
        if (ret && ret->init())
        {
            ret->autorelease();
            return ret;
        }
        CC_SAFE_DELETE(ret);
        return nullptr;
    }

    bool init() override
    {
        if (!CCLayer::init())
            return false;

#ifndef _WIN32
        this->setTouchEnabled(true);
        this->setTouchMode(kCCTouchesOneByOne);
        CCDirector::sharedDirector()
            ->getTouchDispatcher()
            ->addTargetedDelegate(this, -128, false);
#endif

        this->schedule(schedule_selector(CPSInputLayer::tick));
        return true;
    }

    void tick(float)
    {
#ifdef _WIN32
        pollWindowsInput();
#elif defined(__APPLE__)
        pollMacInput();
#endif
        updateCPS();
    }

#ifndef _WIN32
    bool ccTouchBegan(CCTouch *, CCEvent *) override
    {
        clicks++;
        return false;
    }
#endif
};

class $modify(CPSPlayLayer, PlayLayer)
{
    bool init(GJGameLevel *level, bool p1, bool p2)
    {
        if (!PlayLayer::init(level, p1, p2))
            return false;

        auto winSize = CCDirector::sharedDirector()->getWinSize();

        cpsLabel = CCLabelBMFont::create("CPS: 0", "chatFont.fnt");
        cpsLabel->setAnchorPoint({1.f, 0.f});
        cpsLabel->setPosition({winSize.width - 10.f, 10.f});
        cpsLabel->setScale(0.4f);
        cpsLabel->setZOrder(1000);
        this->addChild(cpsLabel);

        clicks = 0;
        lastTime = std::chrono::steady_clock::now();

#ifdef _WIN32
        lastLMB = lastSpace = lastUp = false;
#endif
#ifdef __APPLE__
        lastLMB = lastSpace = lastUp = false;
#endif

        this->addChild(CPSInputLayer::create(), 999);
        return true;
    }
};
 