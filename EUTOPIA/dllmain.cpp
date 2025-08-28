#include <Windows.h>
#include "Client/Client.h"
#include "Client/Managers/HooksManager/HookManager.h"
#include "Client/Managers/CommandManager/CommandManager.h"

// stuff to set the window title
#include "winrt/Windows.UI.Core.h"
#include "winrt/windows.system.h"
#include <winrt/Windows.UI.Notifications.h>
#include <winrt/Windows.Data.Xml.Dom.h>
#include "winrt/windows.applicationmodel.core.h"
#include "winrt/Windows.UI.ViewManagement.h"
#include "winrt/Windows.Foundation.h"

//#include "Client/Client.h"
//#include "Client/Managers/CommandManager/CommandManager.h"
//#include "Client/Managers/HooksManager/HookManager.h"
//#include "Client/Managers/ModuleManager/ModuleManager.h"
//#include "Utils/RenderUtil.h"

void setWindowTitle(const std::wstring& title) {
    winrt::Windows::ApplicationModel::Core::CoreApplication::MainView()
        .CoreWindow()
        .DispatcherQueue()
        .TryEnqueue([title]() {
            winrt::Windows::UI::ViewManagement::ApplicationView::GetForCurrentView().Title(title);
        });
}


DWORD WINAPI initClient(LPVOID lpParameter) {
    Client::init();

    setWindowTitle(L"Boostv3 1.21.9X"); // set the window title

    while(Client::isInitialized()) {
        ModuleManager::onClientTick();
        Sleep(50);
    }

    Sleep(25);

    FreeLibraryAndExitThread((HMODULE)lpParameter, 1);
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    switch(ul_reason_for_call) {
        case DLL_PROCESS_ATTACH:
            CreateThread(0, 0, (LPTHREAD_START_ROUTINE)initClient, hModule, 0, 0);
            break;
        case DLL_PROCESS_DETACH:
                Client::shutdown();
                Sleep(50);
                RenderUtil::Clean();
                HookManager::shutdown();
                ModuleManager::shutdown();
                CommandManager::shutdown();
                setWindowTitle(L"Minecraft"); // boost ejected, so put the old window name
                Client::DisplayClientMessage("%sEjected", MCTF::RED);

            break;
    }
    return TRUE;
}
