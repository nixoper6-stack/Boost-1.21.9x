#define NOMINMAX
#include "ChestTP.h"

#include <algorithm>
#include <cmath>
#include <limits>
#include <vector>

bool skipDoubleChests = false;
bool noBorder = false;
ClientInstance* ci = GI::getClientInstance();
constexpr float BORDER_MIN_X = 41.0f;
constexpr float BORDER_MAX_X = 1109.0f;
constexpr float BORDER_MIN_Z = 41.0f;
constexpr float BORDER_MAX_Z = 1109.0f;

ChestTP::ChestTP()
    : Module("ChestTP", "Teleport to nearest chest between Y=60-70", Category::WORLD) {
    registerSetting(new SliderSetting<int>("Range", "Search range for chests", &range, 30, 1, 300));
    registerSetting(new BoolSetting("CustomY", "Enable custom Y level", &customY, false));
    registerSetting(
        new SliderSetting<int>("CustomYLevel", "Select custom Y level", &customYLevel, 60, 60, 80));
    registerSetting(
        new BoolSetting("SkipDoubleChests", "Ignore double chests", &skipDoubleChests, false));
    registerSetting(new BoolSetting("NoBorder", "Ignore chests near the border", &noBorder, false));
}

void ChestTP::onEnable() {
    tpdone = false;
    works = false;

    LocalPlayer* localPlayer = GI::getLocalPlayer();
    if(!localPlayer)
        return;

    BlockSource* region = GI::getClientInstance()->getRegion();
    if(!region)
        return;

    
    if(ci != nullptr) {
        MinecraftSim* mc = ci->minecraftSim;
        if(mc != nullptr) {
            mc->setSimTimer(static_cast<float>(10700.f));
            float speed = static_cast<float>(10700.f) / 20.0f;
            mc->setSimSpeed(speed);
        }
    }

    Vec3<float> playerPos = localPlayer->getPos();
    closestDistance = std::numeric_limits<float>::max();
    goal = Vec3<float>(0, 0, 0);

    int minX = std::max((int)playerPos.x - range, 0);
    int maxX = (int)playerPos.x + range;
    int minZ = std::max((int)playerPos.z - range, 0);
    int maxZ = (int)playerPos.z + range;

    int minY = customY ? customYLevel : 60;
    int maxY = customY ? customYLevel : 70;

    for(int x = minX; x <= maxX; x++) {
        for(int y = minY; y <= maxY; y++) {
            for(int z = minZ; z <= maxZ; z++) {
                BlockPos pos(x, y, z);
                Block* block = region->getBlock(pos);

                if(!block || !block->blockLegacy || block->blockLegacy->blockId != 54)
                    continue;

                if(skipDoubleChests) {
                    static const std::vector<Vec3<int>> directions = {
                        {1, 0, 0}, {-1, 0, 0}, {0, 0, 1}, {0, 0, -1}};

                    bool isDouble = false;
                    for(const auto& dir : directions) {
                        BlockPos neighborPos(x + dir.x, y, z + dir.z);
                        Block* neighborBlock = region->getBlock(neighborPos);
                        if(neighborBlock && neighborBlock->blockLegacy &&
                           neighborBlock->blockLegacy->blockId == 54) {
                            isDouble = true;
                            break;
                        }
                    }

                    if(isDouble)
                        continue;
                }

                Block* above = region->getBlock(BlockPos(x, y + 1, z));
                if(above && above->blockLegacy && above->blockLegacy->blockId != 0)
                    continue;

                if(noBorder) {
                    if(x <= BORDER_MIN_X || x >= BORDER_MAX_X || z <= BORDER_MIN_Z ||
                       z >= BORDER_MAX_Z)
                        continue;
                }

                if(visitedChests.find(pos) != visitedChests.end())
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
        visitedChests.insert(BlockPos(goal.x, goal.y, goal.z));
        works = true;
        GI::DisplayClientMessage("%s[ChestTP] Chest found", MCTF::WHITE);
    } else {
        GI::DisplayClientMessage("%s[ChestTP] No chests found in range", MCTF::RED);
        this->setEnabled(false);
        if(ci != nullptr) {
            MinecraftSim* mc = ci->minecraftSim;
            if(mc != nullptr) {
                mc->setSimTimer(static_cast<float>(20.f));
                float speed = static_cast<float>(20.f) / 20.0f;
                mc->setSimSpeed(speed);
            }
        }
        
    }
}

void ChestTP::onNormalTick(LocalPlayer* localPlayer) {
    if(!localPlayer || !works || tpdone)
        return;

    Vec3<float> currentPos = localPlayer->getPos();
    float distance = currentPos.dist(goal);

    float lerpFactor = 0.2f;
    if(distance <= 13.0f)
        lerpFactor = 0.55f;
    else if(distance <= 20.0f)
        lerpFactor = 0.45f;
    else if(distance <= 100.0f)
        lerpFactor = 0.10f;
    else
        lerpFactor = 0.075f;

    if(distance <= 0.25f) {
        localPlayer->teleport(goal);
        tpdone = true;
        this->setEnabled(false);
    } else {
        Vec3<float> newPos = currentPos.lerpTo(goal, lerpFactor);
        localPlayer->teleport(newPos);
    }
}

void ChestTP::onDisable() {
    tpdone = true;
    if(ci != nullptr) {
        MinecraftSim* mc = ci->minecraftSim;
        if(mc != nullptr) {
            mc->setSimTimer(static_cast<float>(20.f));
            float speed = static_cast<float>(20.f) / 20.0f;
            mc->setSimSpeed(speed);
        }
    }
    GI::DisplayClientMessage("%s[ChestTP] Disabled", MCTF::WHITE);
}
