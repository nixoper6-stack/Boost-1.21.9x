#pragma once

#include <string>
#include <vector>

#include "../../ModuleBase/Module.h"
#include "../SDK/NetWork/Packets/InventoryTranscationPacket.h"
#include "../SDK/NetWork/Packets/NetworkStackLatencyPacket.h"
#include "../SDK/NetWork/Packets/PacketEvent.h"
#include "../SDK/NetWork/Packets/PlayerAuthInputPacket.h"
#include "../Utils/Maths.h"


class Actor;
class ItemUseOnActorInventoryTransaction;

class DisablerNew : public Module {
   public:
    DisablerNew();           
    void onSendPacket(Packet* packet);

   private:
    int Mode = 0;
    std::vector<int64_t> mLatencyTimestamps;
    int mClientTicks;
    bool mShouldUpdateClientTicks;
    Actor* mFirstAttackedActor;
};
