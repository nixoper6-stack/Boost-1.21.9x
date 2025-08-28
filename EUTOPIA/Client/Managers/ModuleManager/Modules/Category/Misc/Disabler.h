#pragma once
#include "../../ModuleBase/Module.h"


class Disabler : public Module {
   public:
    Disabler();

    std::string getModeText() override;
    void onSendPacket(Packet* packet) override;

   private:
    int Mode = 0;
};

