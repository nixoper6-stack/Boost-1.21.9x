#pragma once
#include "../../../ModuleManager.h"
#include "../../ModuleBase/Module.h"
#include "../Player/PacketMine.h"
#include "OreMiner.h"

class OreMiner : public Module {
   private:
    int range = 6;
    int tickDelay = 2;
    int delayCounter = 0;
    BlockPos currentTarget = BlockPos(0, 0, 0);
    bool breaking = false;
    bool spoofSwitch = false;
    bool diamond = true;
    bool iron = false;
    bool gold = false;
    bool coal = false;
    bool redstone = false;
    bool emerald = false;
    bool ancient = false;
    bool dropTP = false;

   public:
    OreMiner();
    void onNormalTick(LocalPlayer* localPlayer) override;
    virtual void onDisable() override;
    virtual void onEnable() override;
};

