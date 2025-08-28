#include "AutoClicker.h"

#include <random>

int minCPS = 8;
int maxCPS = 12;
bool rightclick = false;
bool breakBlocks = false;

int getRandomTicksFromCPS(int a, int b) {
    if(a > b)
        std::swap(a, b);
    static std::random_device rd;
    static std::mt19937 rng(rd());
    std::uniform_int_distribution<int> dist(a, b);
    int cps = dist(rng);
    return std::max(1, 20 / cps);
}

AutoClicker::AutoClicker()
    : Module("AutoClicker", "A simple autoclicker, automatically clicks for you.",
             Category::COMBAT) {
    registerSetting(
        new BoolSetting("RightClick", "Click mouse with right too", &rightclick, false));
    registerSetting(new BoolSetting("Break Blocks", "Break Minecraft blocks", &breakBlocks, false));
    registerSetting(new SliderSetting<int>("Min CPS", "Minimum CPS", &minCPS, minCPS, 1, 20));
    registerSetting(new SliderSetting<int>("Max CPS", "Maximum CPS", &maxCPS, maxCPS, 1, 20));
}

Actor* getActorFromEntityId2(EntityId id) {
    auto player = GI::getLocalPlayer();
    for(auto a : player->level->getRuntimeActorList())
        if(a->getEntityContext()->entity.rawId == id.rawId)
            return a;
    return nullptr;
}

void AutoClicker::onNormalTick(LocalPlayer* lp) {
    auto player = GI::getLocalPlayer();
    if(!player)
        return;
    auto gm = player->getgamemode();
    if(!gm)
        return;

    static int tick = 0;
    static int wait = getRandomTicksFromCPS(minCPS, maxCPS);
    tick++;

    bool ld = GI::isLeftClickDown();
    bool rd = GI::isRightClickDown();
    bool mk = GI::canUseMoveKeys();

    bool mousePressed = ld || (rightclick && rd);

    if(isHoldMode()) {
        if(!mousePressed || !mk) {
            tick = 0;
            return;
        }
    } else {
        if(!mk) {
            tick = 0;
            return;
        }
    }

    if(tick >= wait) {
        if(ld || !isHoldMode()) {
            player->swing();
            auto h = player->level->getHitResult();
            if(h->type == HitResultType::ENTITY) {
                Actor* a = getActorFromEntityId2(h->entity.id);
                if(a &&
                   a->getEntityContext()->entity.rawId != player->getEntityContext()->entity.rawId)
                    gm->attack(a);
            } else if(breakBlocks) {
                bool d = false;
                gm->startDestroyBlock(h->blockPos, h->selectedFace, d);
                gm->stopDestroyBlock(h->blockPos);
                if(d &&
                   GI::getClientInstance()->getRegion()->getBlock(h->blockPos)->blockLegcy->blockid)
                    gm->destroyBlock(h->blockPos, 0);
            }
        }

        if(rightclick && (rd || !isHoldMode())) {
            auto h = player->level->getHitResult();
            gm->buildBlock(h->blockPos, h->selectedFace, true);
        }

        tick = 0;
        wait = getRandomTicksFromCPS(minCPS, maxCPS);
    }
}
