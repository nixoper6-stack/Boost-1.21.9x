#pragma once
#include "../../ModuleBase/Module.h"

#include <vector>
#include <memory>

class Aura : public Module {
private:
    // Settings
    bool mFistFriends = true;
    bool mHotbarOnly = true;
    bool mAutoFireSword = false;
    bool mAutoBow = false;
    bool mThirdPerson = false;
    bool mThirdPersonOnlyOnAttack = false;
    
    // Targeting
    float mRange = 5.0f;
    float mWallRange = 0.0f;
    bool mIncludeMobs = false;
    
    // Attack settings
    int mAPS = 1;
    int mDelay = 1;
    
    // Rotation settings
    int mRotateMode = 0; // 0=None, 1=Normal, 2=Smooth
    
    // Internal state
    std::vector<Actor*> mTargetList;
    Actor* mCurrentTarget = nullptr;
    bool mRotating = false;
    AABB mTargetedAABB;
    int mAttackTicks = 0;
    
    // Bow state
    bool mChargingBow = false;
    int mBowUseTicks = 0;
    static constexpr int MAX_BOW_USE_TICKS = 17;
    
    // Helper methods
    int getSword(Actor* target);
    bool shouldUseFireSword(Actor* target);
    void rotate(Actor* target);
    void shootBow(Actor* target);
    void attack(Actor* target);
    bool isValidTarget(Actor* target);
    
public:
    Aura();
    
    virtual void onEnable() override;
    virtual void onDisable() override;
    virtual void onNormalTick(LocalPlayer* localPlayer) override;
    virtual void onSendPacket(Packet* packet) override;
    virtual void onReceivePacket(Packet* packet, bool* cancel) override;
    virtual void onUpdateRotation(LocalPlayer* localPlayer) override;
    virtual void onLevelRender() override;
    
    // Static members for cross-module communication
    static inline bool sHasTarget = false;
    static inline Actor* sTarget = nullptr;
};