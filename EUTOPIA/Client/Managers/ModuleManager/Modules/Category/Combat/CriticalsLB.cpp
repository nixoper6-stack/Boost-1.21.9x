#include "CriticalsLB.h"

CriticalsLB::CriticalsLB()
    : Module("CriticalsLB", "Each hit is a critical (Lifeboat safe)", Category::COMBAT) {
    registerSetting(new BoolSetting("LifeBoat", "Bypasses lifeboat kick", &lb, true));
}

void CriticalsLB::sendPacketAsync(const PlayerAuthInputPacket& pkt) {
    if(!GI::canUseMoveKeys())
        return;

    auto packet = MinecraftPacket::createPacket(PacketID::PlayerAuthInput);
    auto* newPkt = reinterpret_cast<PlayerAuthInputPacket*>(packet.get());

    
    std::memcpy(newPkt, &pkt, sizeof(PlayerAuthInputPacket));

    if(!GI::getPacketSender())
        return;

    GI::getPacketSender()->sendToServer(newPkt);
}

void CriticalsLB::setPrevAuthInputPacket(const PlayerAuthInputPacket& pkt) {
    if(!GI::canUseMoveKeys())
        return;

    
    std::memcpy(&lastInputPacket, &pkt, sizeof(PlayerAuthInputPacket));
}

void CriticalsLB::onSendPacket(Packet* packet) {
    if(!GI::getLocalPlayer())
        return;
    if(!GI::canUseMoveKeys())
        return;

    if(packet->getId() == PacketID::MovePlayer) {
        auto& movePkt = *reinterpret_cast<MovePlayerPacket*>(packet);
        movePkt.mOnGround = false;
    } else if(packet->getId() == PacketID::PlayerAuthInput) {
        const auto& currAuthPkt = *reinterpret_cast<PlayerAuthInputPacket*>(packet);
        setPrevAuthInputPacket(currAuthPkt);
    }
}

void CriticalsLB::onAttack(Actor* target, bool& cancel) {
    if(!GI::canUseMoveKeys())
        return;
    if(lastInputPacket.mClientTick < 5)
        return;

    PlayerAuthInputPacket pktToModify;

    
    std::memcpy(&pktToModify, &lastInputPacket, sizeof(PlayerAuthInputPacket));

    pktToModify.removeMovingInput();
    pktToModify.mInputData |= AuthInputAction::JUMPING;
    pktToModify.mInputData |= AuthInputAction::JUMP_DOWN;
    pktToModify.mPos.y += 1.0f;

    sendPacketAsync(pktToModify);

    pktToModify.mInputData &= ~AuthInputAction::JUMPING;
    pktToModify.mInputData &= ~AuthInputAction::JUMP_DOWN;
    pktToModify.mPos.y -= 0.5f;

    for(int i = 0; i < 6; i++) {
        sendPacketAsync(pktToModify);
    }
}
