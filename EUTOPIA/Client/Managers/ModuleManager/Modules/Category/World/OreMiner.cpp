#pragma once
#include "OreMiner.h"

#include "../../../ModuleManager.h"
#include "../../ModuleBase/Module.h"
#include "../Player/PacketMine.h"

bool stone = false;
static bool quartz = false;
static bool lapis = false;

OreMiner::OreMiner() : Module("OreMiner", "Mines the ores around you", Category::WORLD) {
    registerSetting(new SliderSetting<int>("Range", "Scan radius", &range, 3, 1, 12));
    registerSetting(new BoolSetting("Diamond", "Break Diamond", &diamond, true));
    registerSetting(new BoolSetting("Iron", "Break Iron", &iron, false));
    registerSetting(new BoolSetting("Gold", "Break Gold", &gold, false));
    registerSetting(new BoolSetting("Coal", "Break Coal", &coal, false));
    registerSetting(new BoolSetting("Lapis", "Break Lapis", &lapis, false));
    registerSetting(new BoolSetting("Quartz", "Break Quartz", &quartz, false));
    registerSetting(new BoolSetting("Redstone", "Break Redstone", &redstone, false));
    registerSetting(new BoolSetting("Emerald", "Break Emerald", &emerald, false));
    registerSetting(new BoolSetting("AncientDebris", "Break Ancient Debris", &ancient, false));
    // registerSetting(new BoolSetting("Stone", "Break stone", &stone, false));
}

void OreMiner::onEnable() {
    GI::DisplayClientMessage("%s[OreMiner] Enabled", MCTF::WHITE);
}

void OreMiner::onNormalTick(LocalPlayer* localPlayer) {
    if(!localPlayer)
        return;

    GameMode* gm = localPlayer->gamemode;
    if(!gm)
        return;

    PacketMine* packetMine = ModuleManager::getModule<PacketMine>();
    if(!packetMine || !packetMine->isEnabled())
        packetMine->setEnabled(true);

    BlockSource* region = GI::getClientInstance()->getRegion();
    if(!region)
        return;

    // 🔹 First, check the head block (above the player)
    Vec3<float> pos = localPlayer->getPos();

    // 🔹 Then search for closest ore
    std::vector<int> ores;
    if(diamond)
        ores.push_back(56);
    if(iron)
        ores.push_back(15);
    if(gold)
        ores.push_back(14);
    if(coal)
        ores.push_back(16);
    if(quartz)
        ores.push_back(153);
    if(lapis)
        ores.push_back(21);
    if(redstone)
        ores.push_back(74);
    if(emerald)
        ores.push_back(129);
    if(ancient)
        ores.push_back(526);
    if(stone) {
        ores.push_back(1);   // Stone
        ores.push_back(4);   // Cobblestone
        ores.push_back(48);  // Mossy Cobblestone
        ores.push_back(98);  // Stone Bricks

        ores.push_back(113);  // Deepslate
        ores.push_back(114);  // Tuff
        ores.push_back(155);  // Basalt
        ores.push_back(491);  // Smooth Basalt
        ores.push_back(486);  // Blackstone
        ores.push_back(487);  // Polished Blackstone

        ores.push_back(112);  // End Stone
        ores.push_back(3);    // Dirt
        ores.push_back(2);    // Grass Block
        ores.push_back(12);   // Sand
        ores.push_back(13);   // Gravel
        ores.push_back(243);  // Podzol
        ores.push_back(110);  // Mycelium
        ores.push_back(87);   // Netherrack
        ores.push_back(88);   // Soul Sand
        ores.push_back(89);   // Glowstone (optional, if you want glowing blocks)
        ores.push_back(45);   // Bricks (common in structures)

        ores.push_back(103);  // Melon (if terrain includes farms)
        ores.push_back(18);   // Leaves (if you want to "tunnel" through trees)
    }

    Vec3<float> playerPos = localPlayer->getPos();
    BlockPos base((int)playerPos.x, (int)playerPos.y + 1, (int)playerPos.z);

    float closestDist = FLT_MAX;
    BlockPos closestOre(0, 0, 0);

    for(int x = -range; x <= range; x++) {
        for(int y = -range; y <= range; y++) {
            for(int z = -range; z <= range; z++) {
                BlockPos pos = base.add2(x, y, z);
                Block* blk = region->getBlock(pos);
                if(!blk || !blk->blockLegacy)
                    continue;

                int id = blk->blockLegacy->blockId;
                if(std::find(ores.begin(), ores.end(), id) == ores.end())
                    continue;

                float dist = playerPos.dist(pos.CastTo<float>());
                if(dist < closestDist) {
                    closestDist = dist;
                    closestOre = pos;
                }
            }
        }
    }

    if(closestDist == FLT_MAX)
        return;

    Vec2<float> rot = playerPos.CalcAngle(closestOre.toFloat().add2(0.5f, 0.5f, 0.5f));

    auto rotComp = localPlayer->getActorRotationComponent();
    if(rotComp) {
        rotComp->presentRot = rot;
    }


    if(gm->destroyProgress <= 0.f || packetMine->getBreakPos() != closestOre) {
        bool b = false;
        gm->startDestroyBlock(closestOre, 0, b);
        packetMine->mineBlock(closestOre, 0);
    }
}

void OreMiner::onDisable() {
    PacketMine* packetMine = ModuleManager::getModule<PacketMine>();
    if(packetMine) {
        packetMine->setEnabled(false);
    }

    GI::DisplayClientMessage("%s[OreMiner] Disabled", MCTF::WHITE);
}
