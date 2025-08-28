#include "Actor.h"
#include "../../GlobalInstance.h"
#include "../../Utils/MemoryUtil.h"
#include "../../../Client/Managers/HooksManager/Hooks/Network/PacketReceiveHook.h"
#include "../SDK/Core/Sigs.h"

// test

// actor setpos 1 > 48 8B C4 48 89 58 18 55 56 57 41 54 41 55 41 56 41 57 48 8D 68 B9
// actor setpos 2 > 48 89 5C 24 10 48 89 6C 24 20 56 57 41 56 48 83 EC 30 48 8B DA

void Actor::sendToSelf(const std::shared_ptr<Packet>& packet) {
    PacketReceiveHook::handlePacket(packet);
}


  
void Actor::setPosition(Vec3<float> pos) { // dani fix idk if it works lolol
    static auto func =
        MemoryUtil::getFuncFromCall(MemoryUtil::findSignature("48 8B C4 48 89 58 18 55 56 57 41 54 41 55 41 56 41 57 48 8D 68 B9")); // actor set pos 2

    MemoryUtil::callFastcall<void, void*, Vec3<float>*>(func, this, &pos);
}

// getpos 48 89 5C 24 10 48 89 74 24 18 48 89 7C 24 20 55 41 54 41 55 41 56 41 57 48 8D AC 24 70 FD FF FF 48 81 EC 90 03 00 00 48 8B 05 ?? ?? ?? ?? 48 33 C4 48 89 85 88 02 00 00 4C 8B F9

void Actor::setPos(Vec3<float> pos) {
    GI::getLocalPlayer()->getStateVectorComponent()->mPos = pos;
}



void Actor::setPos2(Vec3<float> pos) {  // boom
    auto mpp = MinecraftPacket::createPacket<MovePlayerPacket>();
    if(!mpp) {
        return;
    }
    mpp->mPlayerID = this->getRuntimeID();
    auto rotComp = this->getActorRotationComponent();
    mpp->mRot = {rotComp->mPitch, rotComp->mYaw};
    mpp->mYHeadRot = this->getActorHeadRotationComponent()->mHeadRot;
    mpp->mPos = pos;
    mpp->mResetPosition = PositionMode::Teleport;
    mpp->mRidingID = -1;
    mpp->mCause = TeleportationCause::Unknown;
    mpp->mSourceEntityType = ActorType::Player;
    mpp->mTick = 0;
    sendToSelf(mpp);
}





void Actor::teleport(const Vec3<float>& pos) {
    if(Sigs::Actor_setPos == 0)
        return;

    using SetPosFn = void(__fastcall*)(Actor * thisPtr, const Vec3<float>& pos);
    static SetPosFn setPos = reinterpret_cast<SetPosFn>(Sigs::Actor_setPos);

    setPos(this, pos);
}




void Actor::setGameType(int gameType) {
    auto gtpkt = MinecraftPacket::createPacket(PacketID::SetPlayerGameType);
    auto pkt = reinterpret_cast<SetPlayerGameTypePacket*>(gtpkt.get());
    pkt->mPlayerGameType = static_cast<GameType>(gameType);
    GI::getPacketSender()->sendToServer(pkt);
}
