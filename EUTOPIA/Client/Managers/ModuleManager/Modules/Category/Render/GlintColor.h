#pragma once
#include "../../ModuleBase/Module.h"

class GlintColor : public Module {
   private:
    UIColor color = UIColor(255, 0, 0);
    Vec3<float>* glintColorPtr = nullptr;

   public:
    GlintColor();



    void onDisable() override;
    void onClientTick() override;
};
