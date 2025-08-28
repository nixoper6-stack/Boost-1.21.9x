#pragma once

//Client
#include "Modules/ModuleBase/Module.h"
#include "Modules/Category/Client/ClickGUI.h"
#include "Modules/Category/Client/CustomFont.h"
#include "Modules/Category/Client/NeteaseMusicGUI.h"
#include "Modules/Category/Client/ArrayList.h"
#include "Modules/Category/Client/BGAnimation.h"
#include "Modules/Category/Client/Editor.h"
#include "Modules/Category/Client/HUD.h"
#include "Modules/Category/Client/ColorsMod.h"
//Player
#include "Modules/Category/Player/AntiCrystal.h"
#include "Modules/Category/Player/BlockReach.h"
#include "Modules/Category/Player/Offhand.h"
#include "Modules/Category/Player/PacketMine.h"
#include "Modules/Category/Player/PopCounter.h"
#include "Modules/Category/Player/Clip.h" 
#include "Modules/Category/Player/AutoEat.h" 

//Test
#include "Modules/Category/Test/TestModule.h"
// Combat
#include "Modules/Category/Combat/KillAura.h"
#include "Modules/Category/Combat/Hitbox.h"
#include "Modules/Category/Combat/TPAura.h"
#include "Modules/Category/Combat/Surround.h"
#include "Modules/Category/Combat/BowSpam.h"
#include "Modules/Category/Combat/Criticals.h"
#include "Modules/Category/Combat/Reach.h"
#include "Modules/Category/Combat/Aura.h" 
#include "Modules/Category/Combat/CriticalsLB.h"
#include "Modules/Category/Combat/AutoClicker.h"
#include "Modules/Category/Combat/TriggerBot.h"
#include "Modules/Category/Combat/DamageText.h"

//Movement
#include "Modules/Category/Movement/FlyLB.h"
#include "Modules/Category/Movement/Velocity.h"
#include "Modules/Category/Movement/NoSlow.h"
#include "Modules/Category/Movement/Fly.h"
#include "Modules/Category/Movement/Phase.h"
#include "Modules/Category/Movement/AutoSneak.h"
#include "Modules/Category/Movement/AutoJump.h"
#include "Modules/Category/Movement/AutoSprint.h"
#include "Modules/Category/Movement/Step.h"
#include "Modules/Category/Movement/Jetpack.h"
#include "Modules/Category/Movement/Jesus.h"
#include "Modules/Category/Movement/Spider.h"

//Render
#include "Modules/Category/Render/Keystrokes.h"
#include "Modules/Category/Render/NoRender.h"
#include "Modules/Category/Render/NoSwing.h"
#include "Modules/Category/Render/NameTag.h"
#include "Modules/Category/Render/Swing.h"
#include "Modules/Category/Render/CustomFov.h"
#include "Modules/Category/Render/CameraTweaks.h"
#include "Modules/Category/Render/FullBright.h"
#include "Modules/Category/Render/ChunkBorders.h"
#include "Modules/Category/Render/NoHurtCam.h"
#include "Modules/Category/Render/BlockHighLight.h"
#include "Modules/Category/Render/SetColor.h"
#include "Modules/Category/Render/CrystalChams.h"
#include "Modules/Category/Render/ViewModel.h"
#include "Modules/Category/Render/PortalESP.h"
#include "Modules/Category/Render/StorageESP.h"
#include "Modules/Category/Render/GlintColor.h"
#include "Modules/Category/Render/ESP.h"
#include "Modules/Category/Render/Tracer.h"
#include "Modules/Category/Render/OreESP.h"
#include "Modules/Category/Render/CustomSky.h"

// WORLD
#include "Modules/Category/World/TpMine.h"
#include "Modules/Category/World/Hub.h"
#include "Modules/Category/World/AutoHub.h"
#include "Modules/Category/World/OreMiner.h"
#include "Modules/Category/World/Scaffold.h"
#include "Modules/Category/World/ChestTP.h"
#include "Modules/Category/World/AutoEZ.h"
#include "Modules/Category/World/NoFall.h"
#include "Modules/Category/World/WaterTP.h"


//MISC
#include "Modules/Category/Misc/DisablerNew.h"
#include "Modules/Category/Misc/AntiBot.h"
#include "Modules/Category/Misc/NoPacket.h"
#include "Modules/Category/Misc/Timer.h"
#include "Modules/Category/Misc/Chat.h"
#include "Modules/Category/Misc/Twerk.h"
#include "Modules/Category/Misc/Disabler.h"
#include "Modules/Category/Misc/PlayerJoin.h"
#include "Modules/Category/Misc/ModWarning.h"
#include "Modules/Category/Misc/Spammer.h"
#include "Modules/Category/Misc/DeviceSpoofer.h"
class ModuleManager {
   public:
    static inline std::vector<Module*> moduleList;

    static void init();
    static void shutdown();

    template <typename TRet>
    static TRet* getModule() {
        for(Module* mod : moduleList) {
            TRet* result = dynamic_cast<TRet*>(mod);
            if(result == nullptr)
                continue;
            return result;
        }
        return nullptr;
    }

    static void onKeyUpdate(int key, bool isDown);
    static void onClientTick();
    static void onNormalTick(LocalPlayer* localPlayer);
    static void onLevelTick(Level* level);
    static void onUpdateRotation(LocalPlayer* localPlayer);
    static void onSendPacket(Packet* packet);
    static void onReceivePacket(Packet* packet, bool* cancel);
    static void onD2DRender();
    static void onMCRender(MinecraftUIRenderContext* renderCtx);
    static void onLevelRender();
    static void onRenderActorBefore(Actor* actor, Vec3<float>* camera, Vec3<float>* pos);
    static void onRenderActorAfter(Actor* actor);
    static void onChestScreen(ContainerScreenController* csc);
    static void onLoadConfig(void* conf);
    static void onSaveConfig(void* conf);
};
