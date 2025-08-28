#include "ESP.h"

#include <DrawUtil.h>
#include <Minecraft/TargetUtil.h>

ESP::ESP() : Module("ESP", "Highlights player/entity", Category::RENDER) {
    registerSetting(new ColorSetting("Color", "NULL", &color, UIColor(0, 0, 255), false));
    registerSetting(new SliderSetting<int>("Alpha", "NULL", &alpha, 40, 0, 255));
    registerSetting(new SliderSetting<int>("LineAlpha", "NULL", &lineAlpha, 135, 0, 255));
    registerSetting(new BoolSetting("Mobs", "NULL", &mobs, false));
    registerSetting(new BoolSetting("ItemESP", "Highlight items", &itemESP, false));
}

void ESP::onLevelRender() {
    LocalPlayer* localPlayer = GI::getLocalPlayer();
    if(!localPlayer)
        return;

    for(auto& entity : localPlayer->level->getRuntimeActorList()) {
        if(TargetUtil::isTargetValid(entity, mobs)) {
            AABBShapeComponent* entityAABBShape = entity->getAABBShapeComponent();
            if(entityAABBShape) {
                AABB box = entityAABBShape->getAABB();
                DrawUtil::drawBox3dFilled(box, UIColor(color.r, color.g, color.b, alpha),
                                          UIColor(color.r, color.g, color.b, lineAlpha));
            }
        }
    }

    if(itemESP) {
        for(auto& entity : localPlayer->level->getRuntimeActorList()) {
            ActorTypeComponent* actorTypeComponent = entity->getActorTypeComponent();
            if(actorTypeComponent) {
                uint32_t entityId = static_cast<uint32_t>(actorTypeComponent->mType);
                if(entityId == 64) {
                    AABBShapeComponent* entityAABBShape = entity->getAABBShapeComponent();
                    if(entityAABBShape) {
                        AABB box = entityAABBShape->getAABB();
                        DrawUtil::drawBox3dFilled(box, UIColor(color.r, color.g, color.b, alpha),
                                                  UIColor(color.r, color.g, color.b, lineAlpha));
                    }
                }
            }
        }
    }
}