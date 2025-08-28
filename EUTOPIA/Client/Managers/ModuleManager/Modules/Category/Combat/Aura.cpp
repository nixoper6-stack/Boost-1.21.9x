#include "Aura.h"
#include <algorithm>
#include <Minecraft/InvUtil.h>
#include <Minecraft/WorldUtil.h>
#include <DrawUtil.h>
#include <Minecraft/TargetUtil.h>

Aura::Aura() : Module("Aura", "Advanced combat module with weapon switching and bow support", Category::COMBAT) {
    // Friend settings
    registerSetting(new BoolSetting("FistFriends", "Use fist/fish for friends", &mFistFriends, true));
    
    // Targeting settings
    registerSetting(new SliderSetting<float>("Range", "Attack range", &mRange, 5.0f, 3.0f, 8.0f));
    registerSetting(new SliderSetting<float>("WallRange", "Attack range through walls", &mWallRange, 0.0f, 0.0f, 5.0f));
    registerSetting(new BoolSetting("IncludeMobs", "Target mobs", &mIncludeMobs, false));
    
    // Attack settings
    registerSetting(new SliderSetting<int>("APS", "Attacks per second", &mAPS, 1, 1, 20));
    registerSetting(new SliderSetting<int>("Delay", "Attack delay in ticks", &mDelay, 1, 0, 20));
    
    // Weapon settings
    registerSetting(new BoolSetting("HotbarOnly", "Only use hotbar items", &mHotbarOnly, true));
    
#ifdef __PRIVATE_BUILD__
    registerSetting(new BoolSetting("AutoFireSword", "Auto use fire sword", &mAutoFireSword, false));
    registerSetting(new BoolSetting("AutoBow", "Auto use bow", &mAutoBow, false));
#endif
    
    // Rotation settings
    registerSetting(new EnumSetting("Rotation", "Rotation mode", {"None", "Normal", "Smooth"}, &mRotateMode, 1));
    
    // Visual settings
    registerSetting(new BoolSetting("ThirdPerson", "Third person view", &mThirdPerson, false));
    registerSetting(new BoolSetting("ThirdPersonOnlyOnAttack", "Third person only when attacking", &mThirdPersonOnlyOnAttack, false));
}

int Aura::getSword(Actor* target) {
    auto player = GI::getLocalPlayer();
    if (!player) return -1;
    
    auto supplies = player->getsupplies();
    if (!supplies) return -1;
    
    auto container = supplies->getcontainer();
    if (!container) return -1;
    
    // Check if target is a friend and we should use fist/fish
    // Note: Friend manager not implemented in this version, using placeholder
    bool isFriend = false; // TODO: Implement friend check
    
    if (isFriend && mFistFriends) {
        // Look for TROPICAL_FISH (ID 267) in hotbar
        int fishSlot = -1;
        for (int i = 0; i < (mHotbarOnly ? 9 : 36); i++) {
            auto item = container->getItem(i);
            if (!InvUtil::isVaildItem(item)) continue;
            
            if (InvUtil::getItemId(item) == 267) {
                fishSlot = i;
                break;
            }
        }
        
        if (fishSlot != -1) {
            return fishSlot;
        }
        
        // Find empty slot or inert item
        for (int i = 0; i < (mHotbarOnly ? 9 : 36); i++) {
            auto item = container->getItem(i);
            if (!InvUtil::isVaildItem(item) || InvUtil::getItemId(item) == 0) {
                return i;
            }
        }
        
        // Find non-weapon item
        for (int i = 0; i < (mHotbarOnly ? 9 : 36); i++) {
            auto item = container->getItem(i);
            if (InvUtil::isVaildItem(item)) {
                std::string itemName = InvUtil::getItemName(item);
                // Check if it's not a weapon (simple check)
                if (itemName.find("sword") == std::string::npos && 
                    itemName.find("axe") == std::string::npos &&
                    itemName.find("pickaxe") == std::string::npos) {
                    return i;
                }
            }
        }
        
        return supplies->mSelectedSlot;
    }
    
    // Find best sword
    int bestSword = -1;
    int bestDamage = 0;
    
    for (int i = 0; i < (mHotbarOnly ? 9 : 36); i++) {
        auto item = container->getItem(i);
        if (!InvUtil::isVaildItem(item)) continue;
        
        std::string itemName = InvUtil::getItemName(item);
        if (itemName.find("sword") != std::string::npos) {
            // Simple damage calculation based on item ID
            int damage = 0;
            int itemId = InvUtil::getItemId(item);
            
            switch (itemId) {
                case 268: damage = 5; break;  // Wood sword
                case 272: damage = 6; break;  // Stone sword
                case 267: damage = 7; break;  // Iron sword
                case 283: damage = 8; break;  // Gold sword
                case 276: damage = 9; break;  // Diamond sword
                case 743: damage = 10; break; // Netherite sword
                default: damage = 1; break;
            }
            
            if (damage > bestDamage) {
                bestDamage = damage;
                bestSword = i;
            }
        }
    }
    
    if (shouldUseFireSword(target)) {
        // Find fire sword
        for (int i = 0; i < (mHotbarOnly ? 9 : 36); i++) {
            auto item = container->getItem(i);
            if (!InvUtil::isVaildItem(item)) continue;
            
            std::string itemName = InvUtil::getItemName(item);
            if (itemName.find("fire") != std::string::npos && itemName.find("sword") != std::string::npos) {
                return i;
            }
        }
    }
    
    return bestSword != -1 ? bestSword : supplies->mSelectedSlot;
}

bool Aura::shouldUseFireSword(Actor* target) {
#ifdef __PRIVATE_BUILD__
    if (!mAutoFireSword || !target) return false;
    
    // Check if target is on fire
    // Note: This is a simplified check, actual implementation may vary
    return !target->isOnFire();
#else
    return false;
#endif
}

void Aura::onEnable() {
    mTargetList.clear();
    mCurrentTarget = nullptr;
    mRotating = false;
    mChargingBow = false;
    mAttackTicks = 0;
    
    if (mThirdPerson && !mThirdPersonOnlyOnAttack) {
        auto options = GI::getClientInstance()->getOptions();
        if (options && options->mThirdPerson) {
            options->mThirdPerson->mValue = 1;
        }
    }
}

void Aura::onDisable() {
    sHasTarget = false;
    sTarget = nullptr;
    mCurrentTarget = nullptr;
    mRotating = false;
    mChargingBow = false;
    
    if (mThirdPerson && !mThirdPersonOnlyOnAttack) {
        auto options = GI::getClientInstance()->getOptions();
        if (options && options->mThirdPerson) {
            options->mThirdPerson->mValue = 0;
        }
    }
}

void Aura::rotate(Actor* target) {
    if (mRotateMode == 0 || !target) return; // None
    
    auto player = GI::getLocalPlayer();
    if (!player) return;
    
  mTargetedAABB = mCurrentTarget->getAABB(true);
    mRotating = true;
}

void Aura::shootBow(Actor* target) {
#ifdef __PRIVATE_BUILD__
    if (!mAutoBow || !target) return;
#else
    return;
#endif
    
    auto player = GI::getLocalPlayer();
    if (!player) return;
    
    auto supplies = player->getsupplies();
    if (!supplies) return;
    
    auto container = supplies->getcontainer();
    if (!container) return;
    
    int bowSlot = -1;
    int arrowSlot = -1;
    
    // Find bow and arrows
    for (int i = 0; i < (mHotbarOnly ? 9 : 36); i++) {
        auto item = container->getItem(i);
        if (!InvUtil::isVaildItem(item)) continue;
        
        std::string itemName = InvUtil::getItemName(item);
        if (itemName.find("bow") != std::string::npos) {
            bowSlot = i;
        }
        if (itemName.find("arrow") != std::string::npos) {
            arrowSlot = i;
        }
        
        if (bowSlot != -1 && arrowSlot != -1) break;
    }
    
    if (bowSlot == -1 || arrowSlot == -1) return;
    
}

void Aura::attack(Actor* target) {
    auto player = GI::getLocalPlayer();
    if (!player || !target) return;
    
    auto gameMode = player->getgamemode();
    if (!gameMode) return;
    
    // Switch to best weapon
    int weaponSlot = getSword(target);
    if (weaponSlot != -1 && weaponSlot != InvUtil::getSelectedSlot()) {
        InvUtil::switchSlot(weaponSlot);
        InvUtil::sendMobEquipment(weaponSlot);
    }
    
    // Attack target
    gameMode->attack(target);
    player->swing();
    
    // Update static members for other modules
    sHasTarget = true;
    sTarget = target;
}

bool Aura::isValidTarget(Actor* target) {
    return TargetUtil::isTargetValid(target, mIncludeMobs, mRange);
}

void Aura::onNormalTick(LocalPlayer* localPlayer) {
    if (!localPlayer) return;
    
    auto level = localPlayer->level;
    if (!level) return;
    
    // Clear target list
    mTargetList.clear();
    
    // Find targets
    auto entityList = level->getRuntimeActorList();
    for (auto& entity : entityList) {
        if (!entity || entity == localPlayer) continue;
        
        if (!isValidTarget(entity)) continue;
        
        // Check wall range
        float rangeCheck = mRange;
        auto region = localPlayer->level; 

        float distance = WorldUtil::distanceToEntity(localPlayer->getPos(), entity);
        if(distance > mRange) {
            rangeCheck = mWallRange;
        }
        
        if (WorldUtil::distanceToEntity(localPlayer->getPos(), entity) <= rangeCheck) {
            mTargetList.push_back(entity);
        }
    }
    
    // Sort by distance
    std::sort(mTargetList.begin(), mTargetList.end(), [localPlayer](Actor* a, Actor* b) {
        return WorldUtil::distanceToEntity(localPlayer->getPos(), a) < 
               WorldUtil::distanceToEntity(localPlayer->getPos(), b);
    });
    
    if (mTargetList.empty()) {
        mCurrentTarget = nullptr;
        mRotating = false;
        sHasTarget = false;
        sTarget = nullptr;
        return;
    }
    
    mCurrentTarget = mTargetList[0];
    
    // Rotate to target
    rotate(mCurrentTarget);
    
    // Attack logic
    mAttackTicks++;
    if (mAttackTicks >= mDelay) {
        for (int i = 0; i < mAPS; i++) {
            attack(mCurrentTarget);
        }
        
        // Try bow attack
        shootBow(mCurrentTarget);
        
        mAttackTicks = 0;
    }
}

void Aura::onSendPacket(Packet* packet) {
    // Handle packet modifications if needed
}

void Aura::onReceivePacket(Packet* packet, bool* cancel) {
    // Handle incoming packets if needed
}

void Aura::onUpdateRotation(LocalPlayer* localPlayer) {
    if(!mRotating || !mCurrentTarget || mRotateMode == 0)
        return;

    // Calculate rotation to target
    Vec3<float> targetPos = mCurrentTarget->getPos();
    targetPos.y = mTargetedAABB.lower.y + (mTargetedAABB.upper.y - mTargetedAABB.lower.y) * 0.5f;

    Vec2<float> rotAngle = localPlayer->getEyePos().CalcAngle(targetPos);

    if(mRotateMode == 1) {
        localPlayer->SetRotaion(rotAngle.x, rotAngle.y);  // 修正参数数量
    } else if(mRotateMode == 2) {
        Vec2<float> currentRot = localPlayer->getRotation();
        Vec2<float> deltaRot = rotAngle.sub(currentRot).normAngles();
        float smoothFactor = 0.3f;
        Vec2<float> newRot = currentRot.add(deltaRot.mul(smoothFactor));
        localPlayer->SetRotaion(newRot.x, newRot.y);  // 修正参数数量
    }
}
void Aura::onLevelRender() {
    if (!mCurrentTarget) return;
    
    // Render target ESP
    AABB targetAABB = mCurrentTarget->getAABB(true);
    UIColor color = UIColor(255, 0, 0, 100); // Red with transparency
   
}