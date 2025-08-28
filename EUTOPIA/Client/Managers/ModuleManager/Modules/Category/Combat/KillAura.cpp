#include "KillAura.h"

#include "../../../../../../Utils/FileUtil.h"
#include "../../Client/Client.h"
#include "../../../../../../Utils/Minecraft/TargetUtil.h"
int pageset = 0;
KillAura::KillAura() : Module("KillAura", "Attacks entities around you", Category::COMBAT) {
    registerSetting(new SliderSetting<float>("Range", "Range in which targets will be hit", &range,
                                             5.f, 3.f, 150.f));
    registerSetting(new SliderSetting<float>("WallRange",
                                             "Range in which targets will be hit through walls",
                                             &wallRange, 0.f, 0.f, 150.f));
    registerSetting(new SliderSetting<float>("predict", "dd", &predictvalue, 0.f, 0.f, 15.f));
    registerSetting(new SliderSetting<float>("test", "dd", &test, 0.f, 0.f, 15.f));
    registerSetting(new SliderSetting<float>("headspeed", "dd", &headspeed, 30.f, 30.f, 360.f));
    registerSetting(
        new SliderSetting<int>("Interval", "Attack delay (Tick)", &interval, 1, 0, 20));
    registerSetting(new SliderSetting<int>("Multiplier", "numbers of attack per tick", &multiplier,
                                           1, 1, 5));
    registerSetting(new EnumSetting("Rotations", "Rotates to the targets",
                                    {"None", "Normal", "Strafe", "Predict"}, &rotMode, 1, 2));
    registerSetting(new BoolSetting("Randomize", "NULL", &randomize, false));
    registerSetting(new EnumSetting("HitType", "NULL", {"Single", "Multi"}, &hitType, 0));
    registerSetting(new SliderSetting<int>("HitChance", "NULL", &hitChance, 100, 0, 100));
    registerSetting(new EnumSetting("Weapon", "Auto switch to best weapon",
                                    {"None", "Switch", "Spoof"}, &autoWeaponMode, 0, 3));
    registerSetting(new BoolSetting("Mobs", "Attack Mobs", &includeMobs, false, 0));
    // registerSetting(new BoolSetting("HurtTimeCheck", "Don't attack if target is on hurt",
    // &hurtTimeCheck, false));
}

std::string KillAura::getModeText() {
    switch(hitType) {
        case 0: {
            return "Single";
            break;
        }
        case 1: {
            return "Multi";
            break;
        }
    }
    return "NULL";
}

void KillAura::onDisable() {
    targetList.clear();
    oTick = INT_MAX;
    shouldRot = false;
}

void KillAura::onEnable() {}

bool KillAura::sortByDist(Actor* a1, Actor* a2) {
    Vec3<float> lpPos = Game.getLocalPlayer()->getPos();
    return ((a1->getPos().dist(lpPos)) < (a2->getPos().dist(lpPos)));
}

int KillAura::getBestWeaponSlot(Actor* target) {
    LocalPlayer* localPlayer = Game.getLocalPlayer();
    Container* inventory = localPlayer->supplies->container;
    float damage = 0.f;
    int slot = localPlayer->supplies->mSelectedSlot;
    for(int i = 0; i < 9; i++) {
        localPlayer->supplies->mSelectedSlot = i;
        float currentDamage = localPlayer->calculateDamage(target);
        if(currentDamage > damage) {
            damage = currentDamage;
            slot = i;
        }
    }

    return slot;
}

void KillAura::Attack(Actor* target) {
    LocalPlayer* localPlayer = Game.getLocalPlayer();
    GameMode* gm = localPlayer->gamemode;

    int randomNumber = (int)(rand() % 100);
    if(randomNumber < hitChance) {
        gm->attack(target);
    }

    localPlayer->swing();
}

void KillAura::onNormalTick(LocalPlayer* localPlayer) {
    Level* level = localPlayer->level;
    BlockSource* region = Game.getClientInstance()->getRegion();

    targetList.clear();

    for(auto& entity : level->getRuntimeActorList()) {
        if(TargetUtil::isTargetValid(entity, includeMobs)) {
            float rangeCheck = range;
            if(region->getSeenPercent(localPlayer->getPos(), entity->getAABB(entity)) == 0.0f)
                rangeCheck = wallRange;

            if(TargetUtil::sortByDist(localPlayer, entity) <= rangeCheck)
                targetList.push_back(entity);
        }
    }

    if(targetList.empty()) {
        shouldRot = false;
        return;
    }

    std::sort(targetList.begin(), targetList.end(), KillAura::sortByDist);

    int oldSlot = localPlayer->supplies->mSelectedSlot;
    if(autoWeaponMode == 2) {
        Game.getLocalPlayer()->supplies->mSelectedSlot = getBestWeaponSlot(targetList[0]);
    } else if(autoWeaponMode == 1)
        Game.getLocalPlayer()->supplies->mSelectedSlot = getBestWeaponSlot(targetList[0]);

    // rotAngle = localPlayer->getEyePos().CalcAngle(targetList[0]->getPos());
    // rotAngle = localPlayer->getEyePos().CalcAngle(targetList[0]->aabbShape->aabb.getCenter());

    Vec3<float> aimPos = targetList[0]->getPos();
    aimPos.y = targetList[0]->getAABB(targetList[0]).getCenter().y;
    rotAngle = localPlayer->getPos().CalcAngle(aimPos);
    rotAngle5 = localPlayer->getPos().CalcAngle(aimPos);
    Vec2<float> ange = localPlayer->getPos().CalcAngle(targetList[0]->getPos()).normAngles();

    shouldRot = true;

    if(oTick >= interval) {
        if(hitType == 1) {
            for(auto& target : targetList) {
                for(int j = 0; j < multiplier; ++j) {
                    if(targetList[0]->rotation->mYaw >= 90 &&
                       targetList[0]->rotation->mYaw <= 93) {
                        Attack(target);

                    } else {
                        Attack(target);
                    }

                    // Attack(target);
                }
            }
        } else {
            for(int j = 0; j < multiplier; ++j) {
                // Attack(targetList[0]);
                if(targetList[0]->rotation->mYaw >= 90 &&
                   targetList[0]->rotation->mYaw <= 93) {
                    Attack(targetList[0]);
                } else {
                    Attack(targetList[0]);
                }
            }
        }
        oTick = 0;
    } else {
        oTick++;
    }

    if(autoWeaponMode == 2)
        localPlayer->supplies->mSelectedSlot = oldSlot;
}

void KillAura::onUpdateRotation(LocalPlayer* localPlayer) {
    if(!shouldRot)
        return;

    ActorRotationComponent* rotation = localPlayer->getActorRotationComponent();
    ActorHeadRotationComponent* headRot = localPlayer->getActorHeadRotationComponent();
    MobBodyRotationComponent* bodyRot = localPlayer->getMobBodyRotationComponent();
 
    switch(rotMode) {
        case 0: {
            return;
            break;
        }
        case 1: {
            rotation->mPitch = rotAngle5.x;
            rotation->mYaw = rotAngle5.y;
            headRot->mHeadRot = rotAngle5.y;
            bodyRot->yBodyRot = rotAngle5.y;
            break;
        }

        case 2: {
            rotation->mYaw = rotAngle5.y;
            rotation->mPitch = rotAngle5.x;
            break;
        }
        case 3: {  
            if(!targetList.empty()) {
                float distanceXZ1 = localPlayer->getPos().dist(Vec3<float>(
                    targetList[0]->getPos().x, localPlayer->getPos().y, targetList[0]->getPos().z));
                Actor* target = targetList[0];
                float targetYaw = target->rotation->mYaw;
                float predictYaw = targetYaw + (headspeed - 90.0f);  // 180 graus atrás
                if(distanceXZ1 < predictvalue) {
                    float predictRad = predictYaw * (PI / 180.0f);

                    Vec3<float> targetPos = target->getPos();
                    Vec3<float> backwardOffset(-cos(predictRad) * test, 0, -sin(predictRad) * test);
                    Vec3<float> backwardPos =
                        Vec3<float>(targetPos.x + backwardOffset.x, targetPos.y + backwardOffset.y,
                                    targetPos.z + backwardOffset.z);

                    rotAngle = localPlayer->getPos().CalcAngle(backwardPos).normAngles();
                    Vec3<float> aimPos = targetList[0]->getPos();
                    rotAngle2 = localPlayer->getPos().CalcAngle(aimPos).normAngles();

                    rotation->mYaw = rotAngle.y;
                    rotation->mPitch = rotAngle.x;

                } else {
                    Vec3<float> aimPos = targetList[0]->getPos();
                    rotAngle = localPlayer->getPos().CalcAngle(aimPos).normAngles();
                    rotAngle2 = localPlayer->getPos().CalcAngle(aimPos).normAngles();
                    rotation->mYaw = rotAngle.y;
                    rotation->mPitch = rotAngle.x;
                }
            }
            break;
        }
    }
}
void KillAura::onSendPacket(Packet* packet) {
    if(rotMode == 0)
        return;

    if(!shouldRot)
        return;

    if(packet->getId() == PacketID::PlayerAuthInput) {
        PlayerAuthInputPacket* paip = static_cast<PlayerAuthInputPacket*>(packet);
        paip->mRot = rotAngle;
        paip->mYHeadRot = rotAngle.y;
    }
}
