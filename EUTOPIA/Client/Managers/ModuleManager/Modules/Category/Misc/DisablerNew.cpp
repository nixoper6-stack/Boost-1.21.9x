#include "DisablerNew.h"

#include <../SDK/NetWork/Packets/NetworkStackLatencyPacket.h>
#include <../SDK/NetWork/Packets/PlayerAuthInputPacket.h>
#include <Windows.h>
// virtual ~InventoryTransaction() = default;
#include "../SDK\NetWork\Packets\InventoryTranscationPacket.h"
#include "../SDK/World/Inventory/Transcation/ItemUseOnActorInventoryTransaction.h"
#include "../Utils/Maths.h"


DisablerNew::DisablerNew() : Module("DisablerNew", "Disable the anticheat", Category::MISC) {
    registerSetting(new EnumSetting("Anticheat", "Anticheat to disable",
                                    {"Lifeboat", "SentinelFull", "BDS", "Sentinel"}, &Mode, 0));
}

Vec3<float> FromGLM(const glm::vec3& v) {
    return Vec3<float>{v.x, v.y, v.z};
}

void DisablerNew::onSendPacket(Packet* packet) {
    auto player = Game.getLocalPlayer();
    if(!player)
        return;

    if(Mode == 0 && packet->getId() == PacketID::PlayerAuthInput) {
        auto gtpkt = MinecraftPacket::createPacket(PacketID::PlayerAuthInput);
        auto authPacket = reinterpret_cast<PlayerAuthInputPacket*>(gtpkt.get());

        authPacket->mInputData &= ~AuthInputAction::JUMPING;
        authPacket->mInputData &= ~AuthInputAction::WANT_UP;
        authPacket->mInputData &= ~AuthInputAction::JUMP_DOWN;
        authPacket->mInputData &= ~AuthInputAction::START_JUMPING;

        authPacket->mInputData |=
            AuthInputAction::JUMPING | AuthInputAction::WANT_UP | AuthInputAction::JUMP_DOWN;

        float targetY = authPacket->mPos.y - 1.f;
        static int jumpTicks = 0;
        static int maxTicks = 11;

        if(jumpTicks < maxTicks) {
            authPacket->mInputData |= AuthInputAction::START_JUMPING;
            jumpTicks++;
        } else {
            authPacket->mInputData &= ~AuthInputAction::START_JUMPING;
            jumpTicks = 0;
        }

        float tickPerc = (float)jumpTicks / (float)maxTicks;
        float newY = Math::lerp(authPacket->mPos.y, targetY, tickPerc);
        authPacket->mPos.y = newY;

        GI::getPacketSender()->sendToServer(authPacket);
    }

    if(Mode == 1) {
        if(packet->getId() == PacketID::NetworkStackLatency) {
            auto gtpkt = MinecraftPacket::createPacket(PacketID::NetworkStackLatency);
            auto latency = reinterpret_cast<NetworkStackLatencyPacket*>(gtpkt.get());
            mLatencyTimestamps.push_back(latency->mCreateTime);
            return;
        }
    }

    if(Mode == 2) {
        if(packet->getId() == PacketID::PlayerAuthInput) {
            auto pkt = reinterpret_cast<PlayerAuthInputPacket*>(packet);
            pkt->mInputData |= AuthInputAction::START_SNEAKING;
            if(!pkt->hasInputData(AuthInputAction::SNEAKING))
                pkt->mInputData |= AuthInputAction::STOP_SNEAKING;
        }
    }

    if(Mode == 3) {
        if(packet->getId() == PacketID::PlayerAuthInput) {
            auto pkt = reinterpret_cast<PlayerAuthInputPacket*>(packet);
            if(mShouldUpdateClientTicks)
                mClientTicks = pkt->mClientTick;

            pkt->mInputData |= AuthInputAction::START_GLIDING | AuthInputAction::JUMPING |
                               AuthInputAction::WANT_UP | AuthInputAction::JUMP_DOWN |
                               AuthInputAction::NORTH_JUMP_DEPRECATED |
                               AuthInputAction::START_JUMPING;

            pkt->mInputData &= ~AuthInputAction::STOP_GLIDING;
            pkt->mInputData |= AuthInputAction::SPRINT_DOWN | AuthInputAction::SPRINTING |
                               AuthInputAction::START_SPRINTING;

            pkt->mInputData &= ~AuthInputAction::STOP_SPRINTING;
            pkt->mInputData &= ~AuthInputAction::SNEAK_DOWN;
            pkt->mInputData &= ~AuthInputAction::SNEAKING;
            pkt->mInputData &= ~AuthInputAction::START_SNEAKING;

            glm::vec3 pos(pkt->mPos.x, pkt->mPos.y, pkt->mPos.z);

            pkt->mClientTick = mClientTicks;
            pkt->mPos = {static_cast<float>(INT_MAX), static_cast<float>(INT_MAX),
                         static_cast<float>(INT_MAX)};
            GI::getPacketSender()->sendToServer(pkt);
            mClientTicks++;

            pkt->mClientTick = mClientTicks;
            pkt->mPos = {static_cast<float>(-INT_MAX), static_cast<float>(-INT_MAX),
                         static_cast<float>(-INT_MAX)};
            GI::getPacketSender()->sendToServer(pkt);
            mClientTicks++;

            pkt->mClientTick = mClientTicks;
            pkt->mPos = FromGLM(pos);
            mClientTicks++;
        }
    }
}
