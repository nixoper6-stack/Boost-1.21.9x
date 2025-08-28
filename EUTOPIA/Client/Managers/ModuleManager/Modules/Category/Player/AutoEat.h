#pragma once
#include "../../ModuleBase/Module.h"

class AutoEat : public Module {
   private:
    int minFoodCount;
    int delayy;
    int tickCounter;
    int minHealth;
    bool autoEatt;

   public:
    AutoEat();
    void onNormalTick(LocalPlayer* localPlayer) override;
};
