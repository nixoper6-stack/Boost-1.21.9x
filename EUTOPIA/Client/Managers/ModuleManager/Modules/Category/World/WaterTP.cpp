#define NOMINMAX
#include "WaterTP.h"

#include <algorithm>
#include <cmath>
#include <limits>
#include <vector>

WaterTP::WaterTP() : Module("WaterTP", "Teleport to nearest water block", Category::WORLD) {
    registerSetting(
        new SliderSetting<int>("Range", "Search range for water blocks", &range, 30, 1, 300));
}

void WaterTP::onEnable() {
    tpdone = false;
    works = false;

    LocalPlayer* localPlayer = Game.getLocalPlayer();
    if(!localPlayer) {
        Game.DisplayClientMessage("%s[WaterTP] No local player found", MCTF::RED);
        return;
    }

    BlockSource* region = Game.getClientInstance()->getRegion();
    if(!region) {
        Game.DisplayClientMessage("%s[WaterTP] No region found", MCTF::RED);
        return;
    }

    ClientInstance* ci = GI::getClientInstance();
    if(ci != nullptr) {
        MinecraftSim* mc = ci->minecraftSim;
        if(mc != nullptr) {
            mc->setSimTimer(10700.0f);
            mc->setSimSpeed(10700.0f / 20.0f);
            Game.DisplayClientMessage("%s[WaterTP] MinecraftSim speed set high", MCTF::WHITE);
        }
    }

    Vec3<float> playerPos = localPlayer->getPos();
    Game.DisplayClientMessage("%s[WaterTP] Player position: (%.2f, %.2f, %.2f)", MCTF::WHITE,
                              playerPos.x, playerPos.y, playerPos.z);

    closestDistance = std::numeric_limits<float>::max();
    goal = Vec3<float>(0, 0, 0);

    int minX = std::max((int)playerPos.x - range, 0);
    int maxX = (int)playerPos.x + range;
    int minZ = std::max((int)playerPos.z - range, 0);
    int maxZ = (int)playerPos.z + range;

    Game.DisplayClientMessage("%s[WaterTP] Searching water blocks in range %d", MCTF::WHITE, range);

    for(int x = minX; x <= maxX; x++) {
        for(int y = 60; y <= 70; y++) {
            for(int z = minZ; z <= maxZ; z++) {
                BlockPos pos(x, y, z);
                Block* block = region->getBlock(pos);

                if(!block || !block->blockLegacy ||
                   (block->blockLegacy->blockId != 8 && block->blockLegacy->blockId != 9))
                    continue;

                Block* above = region->getBlock(BlockPos(x, y + 1, z));
                if(above && above->blockLegacy && above->blockLegacy->blockId != 0)
                    continue;

                float dist = playerPos.dist(Vec3<float>(x, y, z));
                if(dist < closestDistance) {
                    closestDistance = dist;
                    goal = Vec3<float>(x + 0.5f, y + 1.0f, z + 0.5f);
                    Game.DisplayClientMessage(
                        "%s[WaterTP] New closest water found at (%.2f, %.2f, %.2f), distance %.2f",
                        MCTF::WHITE, goal.x, goal.y, goal.z, dist);
                }
            }
        }
    }

    if(closestDistance < std::numeric_limits<float>::max()) {
        works = true;
        Game.DisplayClientMessage("%s[WaterTP] Water found at distance %.2f", MCTF::GREEN,
                                  closestDistance);
    } else {
        Game.DisplayClientMessage("%s[WaterTP] No water found in range", MCTF::RED);
        this->setEnabled(false);

        ClientInstance* ci = GI::getClientInstance();
        if(ci != nullptr) {
            MinecraftSim* mc = ci->minecraftSim;
            if(mc != nullptr) {
                mc->setSimTimer(20.0f);
                mc->setSimSpeed(20.0f / 20.0f);
            }
        }
    }
}

void WaterTP::onNormalTick(LocalPlayer* localPlayer) {
    if(!localPlayer || !works || tpdone) {
        return;
    }

    Vec3<float> currentPos = localPlayer->getPos();
    float distance = currentPos.dist(goal);
    Game.DisplayClientMessage(
        "%s[WaterTP] CurrentPos: (%.2f, %.2f, %.2f), Goal: (%.2f, %.2f, %.2f), Distance: %.2f",
        MCTF::WHITE, currentPos.x, currentPos.y, currentPos.z, goal.x, goal.y, goal.z, distance);

    if(distance <= 0.25f) {
        tpdone = true;
        this->setEnabled(false);
        Game.DisplayClientMessage("%s[WaterTP] Reached goal position.", MCTF::GREEN);
        return;
    }

    float lerpFactor = 0.95f;
    Vec3<float> newPos = currentPos.lerpTo(goal, lerpFactor);
    Game.DisplayClientMessage(
        "%s[WaterTP] Moving towards goal with lerp factor %.2f. NewPos: (%.2f, %.2f, %.2f)",
        MCTF::WHITE, lerpFactor, newPos.x, newPos.y, newPos.z);

    localPlayer->teleport(newPos);
}

void WaterTP::onDisable() {
    tpdone = true;
    ClientInstance* ci = GI::getClientInstance();
    if(ci != nullptr) {
        MinecraftSim* mc = ci->minecraftSim;
        if(mc != nullptr) {
            mc->setSimTimer(20.0f);
            mc->setSimSpeed(20.0f / 20.0f);
        }
    }
    Game.DisplayClientMessage("%s[WaterTP] Disabled", MCTF::WHITE);
}
