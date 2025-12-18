#include <chrono>
#include <Geode/Geode.hpp>

using namespace geode::prelude;

void addBottomRightLabel(CCNode *parent)
{
    auto winSize = CCDirector::sharedDirector()->getWinSize();

    auto label = CCLabelBMFont::create(
        "CPS: 0",
        "chatFont.fnt");

    label->setAnchorPoint({1.f, 0.f});

    label->setPosition({winSize.width - 10.f,
                        10.f});

    label->setScale(0.4f);
    label->setZOrder(1000);

    parent->addChild(label);
}
