#include "ModuleManager.h"
#include "../../Client.h"
#include "Modules/Category/Movement/Airjump.h"


void ModuleManager::init() {

	//Client
	moduleList.push_back(new Arraylist());
	moduleList.push_back(new ClickGUI());
	moduleList.push_back(new BGAnimation());
	moduleList.push_back(new CustomFont());
	moduleList.push_back(new NeteaseMusicGUI());
    moduleList.push_back(new HUD());
    moduleList.push_back(new Colors());
    moduleList.push_back(new Editor());
    moduleList.push_back(new AirJump());


	//Test
	//moduleList.push_back(new TestModule());
	//Combat
    moduleList.push_back(new KillAura());
    moduleList.push_back(new Hitbox());
  //  moduleList.push_back(new DamageText());
    moduleList.push_back(new Surround());
    moduleList.push_back(new TPAura());
    moduleList.push_back(new BowSpam());
    moduleList.push_back(new Criticals());
    moduleList.push_back(new Reach());
   // moduleList.push_back(new Aura());  
	moduleList.push_back(new TriggerBot()); 
	moduleList.push_back(new AutoClicker()); 
	//moduleList.push_back(new CriticalsLB()); 
	
;  	//Movement
    moduleList.push_back(new FlyLB());
	moduleList.push_back(new Velocity());
	moduleList.push_back(new NoSlow());
    moduleList.push_back(new Fly());
    moduleList.push_back(new Phase());
    moduleList.push_back(new AutoSneak());
    //moduleList.push_back(new AutoJump());
    moduleList.push_back(new AutoSprint());
    moduleList.push_back(new Step());
    moduleList.push_back(new Jetpack());
    moduleList.push_back(new Spider());
    //moduleList.push_back(new Jesus());
	//Render
	moduleList.push_back(new NoRender());
    //moduleList.push_back(new Keystrokes());
    moduleList.push_back(new NameTag());
	moduleList.push_back(new Swing());
	moduleList.push_back(new NoSwing());
	moduleList.push_back(new CustomFov());
	moduleList.push_back(new CameraTweaks());
	moduleList.push_back(new Fullbright());
	moduleList.push_back(new ChunkBorders());
	moduleList.push_back(new NoHurtCam());
	moduleList.push_back(new BlockHighlight());
	moduleList.push_back(new SetColor());
    moduleList.push_back(new CrystalChams());
    moduleList.push_back(new PortalESP());
    moduleList.push_back(new StorageESP());
    moduleList.push_back(new GlintColor());
    moduleList.push_back(new ESP());
    moduleList.push_back(new OreESP());
    moduleList.push_back(new CustomSky());
    //moduleList.push_back(new Tracer());
    //moduleList.push_back(new TargetHUD());

    moduleList.push_back(new ViewModel());
	//Player
	moduleList.push_back(new BlockReach());
    moduleList.push_back(new Offhand());
    moduleList.push_back(new AntiCrystal());
  
    moduleList.push_back(new PacketMine());
    moduleList.push_back(new PopCounter());
    moduleList.push_back(new Clip());  
    moduleList.push_back(new AutoEat());  
	


	// World
    moduleList.push_back(new TpMine());
    moduleList.push_back(new Hub());
    moduleList.push_back(new WaterTP());
    moduleList.push_back(new AutoHub());  
    moduleList.push_back(new OreMiner());  
	moduleList.push_back(new Scaffold());  
	moduleList.push_back(new AutoEZ());  
	moduleList.push_back(new NoFall());  
    //moduleList.push_back(new ChestTP()); 
	//Misc
	moduleList.push_back(new NoPacket());
    moduleList.push_back(new AntiBot());
    moduleList.push_back(new Timer());
    moduleList.push_back(new Twerk());
    moduleList.push_back(new Disabler());
    moduleList.push_back(new PlayerJoin());
    moduleList.push_back(new Spammer());
    moduleList.push_back(new ModWarning());
    moduleList.push_back(new DeviceSpoofer());
    moduleList.push_back(new DisablerNew());
   
    moduleList.push_back(new Chat());
	std::sort(moduleList.begin(), moduleList.end(), [](Module* lhs, Module* rhs) {
		return lhs->getModuleName() < rhs->getModuleName();
		});
	getModule<ClickGUI>()->InitClickGUI();
	getModule<Arraylist>()->setEnabled(true);
    getModule<HUD>()->setEnabled(true);



}

void ModuleManager::shutdown() {
	for (auto& mod : moduleList) {
		mod->setEnabled(false);
	}
	for (auto& mod : moduleList) {
		delete mod;
		mod = nullptr;
	}
	moduleList.clear();
}

void ModuleManager::onKeyUpdate(int key, bool isDown) {
	if (!Client::isInitialized())
		return;

	for (auto& mod : moduleList) {
		mod->onKeyUpdate(key, isDown);
	}
}

void ModuleManager::onClientTick() {
	if (!Client::isInitialized())
		return;

	for (auto& mod : moduleList) {
		if (mod->isEnabled() || mod->runOnBackground()) {
			mod->onClientTick();
		}
	}
}

void ModuleManager::onNormalTick(LocalPlayer* localPlayer) {
	if (!Client::isInitialized())
		return;

	for (auto& mod : moduleList) {
		if (mod->isEnabled() || mod->runOnBackground()) {
			mod->onNormalTick(localPlayer);
		}
	}
}

void ModuleManager::onLevelTick(Level* level) {
	if (!Client::isInitialized())
		return;

	for (auto& mod : moduleList) {
		if (mod->isEnabled() || mod->runOnBackground()) {
			mod->onLevelTick(level);
		}
	}
}

void ModuleManager::onUpdateRotation(LocalPlayer* localPlayer) {
	if (!Client::isInitialized())
		return;

	for (auto& mod : moduleList) {
		if (mod->isEnabled() || mod->runOnBackground()) {
			mod->onUpdateRotation(localPlayer);
		}
	}
}

void ModuleManager::onSendPacket(Packet* packet) {
	if (!Client::isInitialized())
		return;

	for (auto& mod : moduleList) {
		if (mod->isEnabled() || mod->runOnBackground()) {
			mod->onSendPacket(packet);
		}
	}
}

void ModuleManager::onReceivePacket(Packet* packet,bool* cancel) {
    if(!Client::isInitialized())
        return;

    for(auto& mod : moduleList) {
        if(mod->isEnabled() || mod->runOnBackground()) {
            mod->onReceivePacket(packet, cancel);
        }
    }
}



void ModuleManager::onRenderActorBefore(Actor* actor,Vec3<float>* camera,Vec3<float>* pos) {
    if(!Client::isInitialized())
        return;

    for(auto& mod : moduleList) {
        if(mod->isEnabled() || mod->runOnBackground()) {
            mod->onRenderActorBefore(actor,camera,pos);
        }
    }
}
void ModuleManager::onRenderActorAfter(Actor* actor) {
    if(!Client::isInitialized())
        return;

    for(auto& mod : moduleList) {
        if(mod->isEnabled() || mod->runOnBackground()) {
            mod->onRenderActorAfter(actor);
        }
    }
}

void ModuleManager::onChestScreen(ContainerScreenController* csc) {
    if(!Client::isInitialized())
        return;

    for(auto& mod : moduleList) {
        if(mod->isEnabled() || mod->runOnBackground()) {
            mod->onChestScreen(csc);
        }
    }
}

void ModuleManager::onD2DRender() {
    if(!Client::isInitialized())
        return;

    for(auto& mod : moduleList) {
        if(mod->isEnabled() || mod->runOnBackground()) {
            mod->onD2DRender();
        }
    }
}

void ModuleManager::onMCRender(MinecraftUIRenderContext* renderCtx) {
	if (!Client::isInitialized())
		return;

	for (auto& mod : moduleList) {
		if (mod->isEnabled() || mod->runOnBackground()) {
			mod->onMCRender(renderCtx);
		}
	}
}

void ModuleManager::onLevelRender() {
	if (!Client::isInitialized())
		return;

	for (auto& mod : moduleList) {
		if (mod->isEnabled() || mod->runOnBackground()) {
			mod->onLevelRender();
		}
	}
}

void ModuleManager::onLoadConfig(void* conf) {
	if (!Client::isInitialized())
		return;

	for (auto& mod : moduleList) {
		mod->onLoadConfig(conf);
	}
}

void ModuleManager::onSaveConfig(void* conf) {
	if (!Client::isInitialized())
		return;

	for (auto& mod : moduleList) {
		mod->onSaveConfig(conf);
	}
}