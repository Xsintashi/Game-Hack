#include <iostream>
#include <Windows.h>
#include <thread>
#include <vector>
#include <string>
#include <bitset>

#include "AntiCheat.h"
#include "Data.h"
#include "SDK/Entity.h"
#include "SDK/GlobalVars.h"
#include "SDK/Mine.h"
#include "SDK/Player.h"
#include "SDK/UserMove.h"
#include <format>

void showConsoleCursor(bool flag) {
    CONSOLE_CURSOR_INFO     cursorInfo;
    GetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
    if (cursorInfo.bVisible == (BOOL)flag)
        return;
    cursorInfo.bVisible = flag; // set the cursor visibility
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
}


void printDaText(int x, int y, const char* str) noexcept {
    COORD pos = { static_cast<short>(x), static_cast<short>(y) };
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
    printf(str);
}

void clearScreen() noexcept {
    for (size_t i = 0; i < 27; i++){
        printDaText(0, i, "                                                                           ");
    }
}

void setWindow(DWORD width, DWORD height) noexcept {
    HWND console = GetConsoleWindow();
    RECT r;
    GetWindowRect(console, &r);

    MoveWindow(console, r.left, r.top, width, height, TRUE);
}

void userMove(Player* player) noexcept {
    while (isRunning) {
        THREAD_SLEEP(32);
        if (!player)
            continue;
        if (player->health < 1)
            continue;

        static auto setBoth = [](int &v1, int &v2, int value) {
            constexpr unsigned mask = 0xFFFFFFFF;
            v1 = value;
            v2 = value ^ mask;
        };

        int buttons = 0;
        player->userMove.buttons = 0;
        player->userMove.backup = -1;
        if (GetKeyState('W') & 0x8000)
            buttons |= UserMoveButtons::FORWARD; player->userMove.rotation = Rotations::LEFT;
        if (GetKeyState('A') & 0x8000)
            buttons |= UserMoveButtons::LEFT; player->userMove.rotation = Rotations::UP;
        if (GetKeyState('S') & 0x8000)
            buttons |= UserMoveButtons::BACKWARD; player->userMove.rotation = Rotations::DOWN;
        if (GetKeyState('D') & 0x8000)
            buttons |= UserMoveButtons::RIGHT; player->userMove.rotation = Rotations::RIGHT;

        setBoth( player->userMove.buttons, player->userMove.backup, buttons);
    }
}

void render() noexcept {
    while (isRunning) {
        THREAD_SLEEP(32);
#if defined(_DEBUG)
        if(GetAsyncKeyState(VK_MENU))
            setWindow(data.globalVars->width + 360, data.globalVars->height);
        else
#endif
            setWindow(data.globalVars->width, data.globalVars->height);

        if (!data.localPlayer)
            continue;

        showConsoleCursor(false);

        if (data.localPlayer->health < 1)
            continue;
        clearScreen();
        printDaText(0, 0, std::string("Nick: " + data.localPlayer->nickname).c_str());
        if(data.mine)
            printDaText(data.mine->origin.x, data.mine->origin.y, "@");

        if (GetAsyncKeyState(VK_TAB)) {
            printDaText(0, 25, std::string("Score: " + std::to_string(data.localPlayer->statistics.score)).c_str());
        }
#if defined(_DEBUG)
        if (GetAsyncKeyState(VK_MENU)) {

            const std::string byteArrayBackup = std::bitset<8>(data.localPlayer->userMove.backup).to_string();
            const std::string byteArrayButtons = std::bitset<8>(data.localPlayer->userMove.buttons).to_string();

            for (size_t i = 0; i < 27; i++)
            {
                printDaText(75, i, "|                                            ");
            }

            printDaText(77, 0, "Debug:");
            printDaText(77, 2, "UserMode:");
            printDaText(77, 3, std::string("  Backup:  ").append(byteArrayBackup).append(" | ").append(std::to_string(data.localPlayer->userMove.backup)).c_str());
            printDaText(77, 4, std::string("  Buttons: ").append(byteArrayButtons).append(" |  ").append(std::to_string(data.localPlayer->userMove.buttons)).c_str());
            printDaText(77, 6, "GlobalVars:");
            printDaText(77, 7, std::string("  TickCount: ").append(std::to_string(data.globalVars->tickCount)).c_str());
            printDaText(77, 8, std::string("  CurrentTime: ").append(std::to_string(data.globalVars->currentTime)).c_str());
            printDaText(77, 9, std::string("  Width: ").append(std::to_string(data.globalVars->width)).c_str());
            printDaText(77, 10, std::string("  Height: ").append(std::to_string(data.globalVars->height)).c_str());
            printDaText(77, 12, std::string("LocalPlayer: 0x").append(std::format("{:x}", data.localPlayer->address)).c_str());
            printDaText(77, 13, std::string("  Entity Classname: ").append(data.localPlayer->identify->classname).c_str());
            printDaText(77, 14, std::string("  Entity Name: ").append(data.localPlayer->identify->name).c_str());
            printDaText(77, 15, std::string("  Nick: ").append(data.localPlayer->nickname).c_str());
            printDaText(77, 16, std::string("  Origin: x: ").append(std::to_string(data.localPlayer->origin.x)).append(" y: ").append(std::to_string(data.localPlayer->origin.y)).c_str());
            printDaText(77, 17, std::string("  Behaviour: 0x").append(std::format("{:x}", (uintptr_t)data.localPlayer->behavior)).c_str());
            printDaText(77, 18, std::string("    Trusted: ").append(data.localPlayer->behavior->trusted ? "Yes (" : "No (").append(std::to_string(data.localPlayer->behavior->level)).append(")").c_str());
            printDaText(77, 19, std::string("    Time: ").append(std::to_string(data.localPlayer->behavior->time)).c_str());
            printDaText(77, 20, std::string("    Reason: ").append(std::to_string((int)data.localPlayer->behavior->reason)).c_str());

        }
#endif
        printDaText(0, 26, std::string("HP: " + std::to_string(data.localPlayer->health)).c_str());
        printDaText(data.localPlayer->origin.x, data.localPlayer->origin.y, "#");

        if (data.localPlayer->userMove.buttons & UserMoveButtons::FORWARD)
            data.localPlayer->origin.y -= 1;
        if (data.localPlayer->userMove.buttons & UserMoveButtons::LEFT)
            data.localPlayer->origin.x -= 1;
        if (data.localPlayer->userMove.buttons & UserMoveButtons::BACKWARD)
            data.localPlayer->origin.y += 1;
        if (data.localPlayer->userMove.buttons & UserMoveButtons::RIGHT)
            data.localPlayer->origin.x += 1;

    }
}

void update() noexcept {
    while (isRunning) {
        THREAD_SLEEP(32);
        data.globalVars->tickCount = static_cast<float>(GetTickCount64() / 1000.f);
        const static float tickCountStatic = data.globalVars->tickCount;
        data.globalVars->currentTime = static_cast<float>(GetTickCount64() / 1000.f) - tickCountStatic;

        if (!data.localPlayer)
            continue;

        if (data.mine){
            if (data.localPlayer->origin == data.mine->origin) {
                data.localPlayer->health -= data.mine->damage;
                data.mine->recreate(static_cast<int>(time(0) - 1));
            }
        }

        if (data.localPlayer->health < 1)
            printDaText(32, 12, "YOU DIED!");

        const auto& origin = data.localPlayer->origin;
        data.localPlayer->origin.x = std::clamp(origin.x, 0, 74);
        data.localPlayer->origin.y = std::clamp(origin.y, 0, 26);
    }
}

int main() {
    data.globalVars = new GlobalVars;
    isRunning = true;
    data.globalVars->width = 640;
    data.globalVars->height = 480;

    std::thread(update).detach();
    std::thread(render).detach();

    SetConsoleTitle(" ");

    std::cout << "Hello World!\nProvide your username: ";
    char buffname[16];
    std::cin >> buffname;

    data.localPlayer = new Player{ buffname };

    std::thread(userMove, data.localPlayer).detach();

    srand(static_cast<int>(time(0)));
    data.mine = new Mine{ rand() };

#if defined(_DEBUG)
    AntiCheat ac;
    std::thread(&AntiCheat::thread, AntiCheat()).detach();
#endif // defined(_DEBUG)

    while (!GetAsyncKeyState(VK_END) && isRunning)
        THREAD_SLEEP(10);
    
    isRunning = false;
    THREAD_SLEEP(100);
#if defined(_DEBUG)
    ac.~AntiCheat();
#endif // defined(_DEBUG)
    if (!data.localPlayer->behavior->trusted) {
        printDaText(32, 12, "You have been Banned!");
        while (!GetAsyncKeyState(VK_SPACE))
            THREAD_SLEEP(10);
    }
    return EXIT_SUCCESS;
}