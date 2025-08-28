#include "Disabler.h"
#include <../SDK/NetWork/Packets/PlayerAuthInputPacket.h>
#include <Windows.h>  

// adapted by dani

Disabler::Disabler() : Module("DisablerSM", "Disable the anticheat", Category::MISC) {
    registerSetting(new EnumSetting("Server", "change mode", {"SML", "SM"}, &Mode, 0));
}

std::string Disabler::getModeText() {
    static char textStr[15];
    sprintf_s(textStr, 15, "Lifeboat");
    return std::string(textStr);
}

static __int64 ms;
static DWORD lastMS = GetTickCount();
static __int64 timeMS = -1;
static DWORD getCurrentMs() {
    return GetTickCount();
}

static __int64 getElapsedTime() {
    return getCurrentMs() - ms;
}

static void resetTime() {
    lastMS = getCurrentMs();
    timeMS = getCurrentMs();
}

static bool hasTimedElapsed(__int64 time, bool reset) {
    if(getCurrentMs() - lastMS > time) {
        if(reset)
            resetTime();
        return true;
    }
    return false;
}

void Disabler::onSendPacket(Packet* packet) {
    if(Mode == 0 &&
       (packet->getName() == "PlayerAuthInputPacket" || packet->getName() == "MovePlayerPacket")) {
        auto* paip = (PlayerAuthInputPacket*)packet;
        auto* mpp = (MovePlayerPacket*)packet;
        if(paip) {
            float perc = static_cast<float>(paip->mClientTick % 3) / 3.0f;
            float targetY = (perc < 0.5f) ? 0.02f : -0.02f;
            paip->mPos.y = Math::lerp(paip->mPos.y, paip->mPos.y + targetY, perc);
            paip->mMove.y = -(1.0f / 3.0f);
            if(paip->mClientTick % 3 == 0) {
                paip->mInputData |= static_cast<AuthInputAction>(
                    1ULL << static_cast<int>(PlayerAuthInputPacket::InputData::StartJumping));
            }
            paip->mInputData |= static_cast<AuthInputAction>(
                1ULL << static_cast<int>(PlayerAuthInputPacket::InputData::Jumping));
           
        }
        if(mpp) {
            float perc = static_cast<float>(mpp->mTick % 3) / 3.0f;
            float targetY = (perc < 0.5f) ? 0.02f : -0.02f;
            mpp->mPos.y = Math::lerp(mpp->mPos.y, mpp->mPos.y + targetY, perc);
            mpp->mOnGround = true;
        }
    }
    if(Mode == 1 &&
       (packet->getName() == "PlayerAuthInputPacket" || packet->getName() == "MovePlayerPacket")) {
        auto* paip = (PlayerAuthInputPacket*)packet;
        auto* mpp = (MovePlayerPacket*)packet;
        mpp->mTick = 0;
        mpp->mRidingID = 0;
        paip->mClientTick = 0;
    }
}