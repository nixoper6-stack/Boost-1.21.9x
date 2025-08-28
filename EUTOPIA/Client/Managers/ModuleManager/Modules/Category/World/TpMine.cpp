#define NOMINMAX
#include "TpMine.h"

#include <algorithm>
#include <cmath>
#include <limits>
#include <vector>

bool lapis = false;
bool AntiSuffocation = false;

TpMine::TpMine() : Module("TpMine", "Teleport to ores", Category::WORLD) {
    registerSetting(new SliderSetting<int>("Range", "Search range for ores", &range, 30, 3, 300));
    registerSetting(new BoolSetting("AntiGravel", "Avoid ores under gravel", &antiGravel, false));
    registerSetting(new BoolSetting("AntiBorder", "Avoid ores near borders", &antiBorder, false));
    registerSetting(new BoolSetting("AntiLava", "Avoid ores near lava", &antiLava, true));
    registerSetting(new BoolSetting("Diamond", "Target Diamond ore", &dim, false));
    registerSetting(new BoolSetting("Iron", "Target Iron ore", &iron, false));
    registerSetting(new BoolSetting("Coal", "Target Coal ore", &coal, false));
    registerSetting(new BoolSetting("Gold", "Target Gold ore", &gold, false));
    registerSetting(new BoolSetting("Emerald", "Target Emerald ore", &emerald, false));
    registerSetting(new BoolSetting("AncientDebris", "Target Ancient Debris", &ancient, false));
    registerSetting(new BoolSetting("Redstone", "Target Redstone ore", &redstone, false));
    registerSetting(new BoolSetting("Quartz", "Target Quartz ore", &quartz, false));
    registerSetting(new BoolSetting("Lapis", "Target Lapis ore", &lapis, false));
    registerSetting(new BoolSetting("Spawners", "Target Spawners", &spawners, false));
    registerSetting(new BoolSetting("Custom", "Enable custom block ID", &cuOre, false));
    registerSetting(new SliderSetting<int>("CustomID", "Custom Block ID", &cuID, 1, 0, 252));
    registerSetting(new SliderSetting<int>("Vein Size", "Min cluster size", &clusterSize, 1, 1, 9));
    // registerSetting(new BoolSetting("AntiSuffocation", "Avoid suffocation when teleporting",
    // &AntiSuffocation, false));
}

void TpMine::onEnable() {
    GI::DisplayClientMessage("%s[TpMine] Enabled", MCTF::WHITE);
    tpdone = false;
    LocalPlayer* localPlayer = GI::getLocalPlayer();
    if(!localPlayer)
        return;

    BlockSource* region = GI::getClientInstance()->getRegion();
    if(!region)
        return;

    closestDistance = std::numeric_limits<float>::max();
    goal = Vec3<float>(0, 0, 0);

    Minecraft* mc = GI::getClientInstance()->minecraft;
    *mc->minecraftTimer = 10700.0f;

    Vec3<float> playerPos = localPlayer->getPos();
    int minX = std::max((int)playerPos.x - range, 0);
    int maxX = (int)playerPos.x + range;
    int minZ = std::max((int)playerPos.z - range, 0);
    int maxZ = (int)playerPos.z + range;

    std::vector<int> targetOres;
    if(coal)
        targetOres.push_back(16);
    if(iron)
        targetOres.push_back(15);
    if(gold)
        targetOres.push_back(14);
    if(redstone)
        targetOres.push_back(74);
    if(dim)
        targetOres.push_back(56);
    if(emerald)
        targetOres.push_back(129);
    if(ancient)
        targetOres.push_back(526);
    if(quartz)
        targetOres.push_back(153);
    if(lapis)
        targetOres.push_back(21);
    if(spawners)
        targetOres.push_back(52);
    if(cuOre)
        targetOres.push_back(cuID);

    for(int x = minX; x <= maxX; x++) {
        for(int y = 5; y <= 128; y++) {
            for(int z = minZ; z <= maxZ; z++) {
                BlockPos blockPos(x, y, z);
                Block* block = region->getBlock(blockPos);
                if(!block || !block->blockLegacy)
                    continue;

                int blockId = block->blockLegacy->blockId;
                if(std::find(targetOres.begin(), targetOres.end(), blockId) == targetOres.end())
                    continue;

                int clusterCount = 0;
                bool clusterInvalid = false;

                for(int dx = -1; dx <= 1 && !clusterInvalid; dx++) {
                    for(int dy = -1; dy <= 1 && !clusterInvalid; dy++) {
                        for(int dz = -1; dz <= 1 && !clusterInvalid; dz++) {
                            BlockPos neighborPos(x + dx, y + dy, z + dz);
                            Block* neighborBlock = region->getBlock(neighborPos);
                            if(!neighborBlock || !neighborBlock->blockLegacy)
                                continue;
                            if(neighborBlock->blockLegacy->blockId != blockId)
                                continue;

                            clusterCount++;

                            if(antiGravel) {
                                Block* aboveBlock = region->getBlock(
                                    BlockPos(neighborPos.x, neighborPos.y + 1, neighborPos.z));
                                if(aboveBlock && aboveBlock->blockLegacy &&
                                   aboveBlock->blockLegacy->blockId == 13) {
                                    clusterInvalid = true;
                                    break;
                                }
                            }

                            if(antiBorder) {
                                if(neighborPos.x > 1090 || neighborPos.x < 50 ||
                                   neighborPos.z > 1090 || neighborPos.z < 50) {
                                    clusterInvalid = true;
                                    break;
                                }
                            }

                            if(antiLava) {
                                for(int lx = -10; lx <= 10 && !clusterInvalid; lx++) {
                                    for(int ly = -10; ly <= 10 && !clusterInvalid; ly++) {
                                        for(int lz = -10; lz <= 10 && !clusterInvalid; lz++) {
                                            BlockPos lavaCheckPos = neighborPos.add2(lx, ly, lz);
                                            Block* lavaBlock = region->getBlock(lavaCheckPos);
                                            if(lavaBlock && lavaBlock->blockLegacy) {
                                                int lavaId = lavaBlock->blockLegacy->blockId;
                                                if(lavaId == 10 || lavaId == 11) {
                                                    clusterInvalid = true;
                                                    break;
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }

                if(clusterInvalid || clusterCount < clusterSize)
                    continue;

                float dist = playerPos.dist(Vec3<float>(x, y, z));
                if(dist < closestDistance) {
                    closestDistance = dist;
                    goal = Vec3<float>(x + 0.5f, y + 1.0f, z + 0.5f);
                }
            }
        }
    }

    if(closestDistance < std::numeric_limits<float>::max()) {
        works = true;
        // Game.getClientInstance()->playUI("levelup", 1, 1.0f);
        GI::DisplayClientMessage("%s[TpMine] Found block", MCTF::WHITE);
    } else {
        GI::DisplayClientMessage("%s[TpMine] No ores found", MCTF::RED);
        this->setEnabled(false);
        Minecraft* mc = GI::getClientInstance()->minecraft;
        *mc->minecraftTimer = 20.0f;   
    }
}

void TpMine::onNormalTick(LocalPlayer* actor) {
    LocalPlayer* localPlayer = GI::getLocalPlayer();
    if(!localPlayer || !works)
        return;
    auto isSafePosition = [&](const Vec3<float>& targetPos) {
        if(!AntiSuffocation)
            return true;

        BlockPos targetBlockPos((int)targetPos.x, (int)targetPos.y + 1, (int)targetPos.z);
        Block* aboveBlock = GI::getClientInstance()->getRegion()->getBlock(targetBlockPos);
        if(aboveBlock && aboveBlock->blockLegacy && aboveBlock->blockLegacy->blockId == 0) {
            return true;
        }

        for(int dx = -1; dx <= 1; dx++) {
            for(int dy = -1; dy <= 1; dy++) {
                for(int dz = -1; dz <= 1; dz++) {
                    BlockPos surroundingBlockPos((int)(targetPos.x + dx), (int)(targetPos.y + dy),
                                                 (int)(targetPos.z + dz));
                    Block* surroundingBlock =
                        GI::getClientInstance()->getRegion()->getBlock(surroundingBlockPos);
                    if(surroundingBlock && surroundingBlock->blockLegacy &&
                       surroundingBlock->blockLegacy->blockId != 0) {
                        return false;
                    }
                }
            }
        }

        return true;
    };

    bool midClickPressed = (GetAsyncKeyState(VK_MBUTTON) & 0x8000) != 0;

    auto isLavaAt = [](const Vec3<float>& pos) {
        BlockPos bp((int)pos.x, (int)pos.y, (int)pos.z);
        Block* block = GI::getClientInstance()->getRegion()->getBlock(bp);
        if(block && block->blockLegacy) {
            int id = block->blockLegacy->blockId;
            return (id == 10 || id == 11);
        }
        return false;
    };

    auto checkLavaBetween = [&](const Vec3<float>& from, const Vec3<float>& to) {
        int steps = 8;
        for(int i = 1; i <= steps; ++i) {
            float t = i / (float)steps;
            Vec3<float> point = from.lerpTo(to, t);
            if(antiLava && isLavaAt(point))
                return true;
        }
        return false;
    };

    Vec3<float> currentPos = localPlayer->getPos();
    float distance = currentPos.dist(goal);

    if(midClickPressed && midClickTP) {
        float lerpFactor = 0.25f;
        if(distance <= 0.25f) {
            localPlayer->teleportTo(goal, true, 1, 1, false);
        } else {
            Vec3<float> newPos = currentPos.lerpTo(goal, lerpFactor);
            if(checkLavaBetween(currentPos, newPos)) {
                GI::DisplayClientMessage("%s[TpMine] Lava detected while teleporting!", MCTF::RED);
                this->setEnabled(false);
                return;
            }
            localPlayer->teleportTo(newPos, true, 1, 1, false);
        }
        return;
    }

    if(!midClickPressed && !tpdone) {
        if(distance <= 1.0f) {
            localPlayer->teleportTo(goal, true, 1,1, false);
            this->setEnabled(false);
            return;
        }

        float lerpFactor = (distance <= 13.0f)    ? 0.55f
                           : (distance <= 20.0f)  ? 0.45f
                           : (distance <= 100.0f) ? 0.10f
                                                  : 0.075f;
        Vec3<float> newPos = currentPos.lerpTo(goal, lerpFactor);

        if(checkLavaBetween(currentPos, newPos)) {
            GI::DisplayClientMessage("%s[TpMine] Lava detected mid-lerp", MCTF::RED);
            this->setEnabled(false);
            return;
        }

        localPlayer->teleportTo(newPos, true, 1, 1, false);
    }
}

void TpMine::onDisable() {
    tpdone = true;
    Minecraft* mc = GI::getClientInstance()->minecraft;
    *mc->minecraftTimer = 20.0f;
    GI::DisplayClientMessage("%s[TpMine] Disabled", MCTF::WHITE);
}
