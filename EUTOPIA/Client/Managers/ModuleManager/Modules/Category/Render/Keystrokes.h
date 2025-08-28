#pragma once
#include "../../ModuleBase/Module.h"
class Keystrokes : public Module {
   public:
    Keystrokes();



    void onLevelRender() override;
};