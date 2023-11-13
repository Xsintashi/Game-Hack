#include <iostream>
#include <Windows.h>
#include <thread>
#include <vector>
#include <string>
#include <bitset>
#include <format>

#include "AntiCheat.h"
#include "Data.h"
#include "Classes/Bullet.h"
#include "Classes/Entity.h"
#include "Classes/EntityList.h"
#include "Classes/Directions.h"
#include "Classes/GlobalVars.h"
#include "Classes/LocalPLayer.h"
#include "Classes/Macros.h"
#include "Classes/Mine.h"
#include "Classes/Player.h"
#include "Classes/Point.h"
#include "Classes/UserMove.h"

constexpr int NUMBER_OF_MINES = 10;

bool turnDebug = false;

void showConsoleCursor(bool flag) {
    CONSOLE_CURSOR_INFO     cursorInfo;
    GetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
    if (cursorInfo.bVisible == (BOOL)flag)
        return;
    cursorInfo.bVisible = flag; // set the cursor visibility
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
}

void keysThread() {
    while (isRunning) {
        THREAD_SLEEP(100);
        if (GetAsyncKeyState(VK_MENU) & 0x8000)
            turnDebug = !turnDebug;
    }
}

void printDaText(const int &x, const int &y, const char* str) noexcept {
    COORD pos = { static_cast<short>(x), static_cast<short>(y) };
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
    printf(str);
}

void clearScreen() noexcept {
    for (int i = 0; i < 27; i++){
        printDaText(0, i, "                                                                                                                                                                                         ");
    }
}

void setWindow(const DWORD &width, const DWORD &height) noexcept {
    HWND console = GetConsoleWindow();
    RECT r;
    GetWindowRect(console, &r);

    MoveWindow(console, r.left, r.top, width, height, TRUE);
}

void enemyAI() noexcept {
    while (isRunning) {
        THREAD_SLEEP(64);
        if (!localPlayer)
            continue;

        if (!data.enemy)
            continue;

        const auto& lpOrigin = localPlayer->origin;

        const auto& origin = data.enemy->origin;

        if(lpOrigin.x != origin.x)
            data.enemy->origin.x += origin.x < lpOrigin.x ? 1 : -1;
        if (lpOrigin.y != origin.y)
            data.enemy->origin.y += origin.y < lpOrigin.y ? 1 : -1;
    }
}


void userMove(Player* player) noexcept {
    while (isRunning) {
        THREAD_SLEEP(32);
        if (!player)
            continue;
        if (player->health < 1)
            continue;

        static auto setBoth = [](int& v1, int& v2, int value) {
            constexpr unsigned mask = 0xFFFFFFFF;
            v1 = value;
            v2 = value ^ mask;
            };

        int buttons = 0;
        player->userMove->buttons = 0;
        player->userMove->backup = -1;
        if (GetKeyState('W') & 0x8000){
            buttons |= UserMoveButtons::FORWARD; player->userMove->direction = Directions::UP;
        }
        if (GetKeyState('A') & 0x8000){
            buttons |= UserMoveButtons::LEFT; player->userMove->direction = Directions::LEFT;
        }
        if (GetKeyState('S') & 0x8000) {
            buttons |= UserMoveButtons::BACKWARD; player->userMove->direction = Directions::DOWN;
        }
        if (GetKeyState('D') & 0x8000) {
            buttons |= UserMoveButtons::RIGHT; player->userMove->direction = Directions::RIGHT;
        }
        if (GetKeyState(VK_SPACE) & 0x8000) {
            buttons |= UserMoveButtons::ATTACK;
        }

        setBoth( player->userMove->buttons, player->userMove->backup, buttons);
    }
}

void render() noexcept {
    while (isRunning) {
        THREAD_SLEEP(32);

        Lock lock;
#if defined(_DEBUG)
        if(turnDebug)
            setWindow(data.globalVars->width + 768, data.globalVars->height);
        else
#endif
            setWindow(data.globalVars->width, data.globalVars->height);

        if (!localPlayer)
            continue;

        if (localPlayer->health < 1)
            continue;

        showConsoleCursor(false);

        clearScreen();
        printDaText(0, 0, std::string("Nick: " + localPlayer->nickname).c_str());
        if(data.enemy)
            printDaText(data.enemy->origin.x, data.enemy->origin.y, "@");

        const auto& mines = entityList->findEntitiesByClassName("entity_mine");
        for (const auto& it : mines)
            printDaText(it->origin.x, it->origin.y, &it->charDraw);

        const auto& bullets = entityList->findEntitiesByClassName("entity_bullet");
        for (const auto& it : bullets)
            printDaText(it->origin.x, it->origin.y, &it->charDraw);

        const auto& points = entityList->findEntitiesByClassName("entity_scorepoint");
        for (const auto& it : points)
            printDaText(it->origin.x, it->origin.y, &it->charDraw);

        if (GetAsyncKeyState(VK_TAB)) {
            printDaText(0, 25, std::string("Score: " + std::to_string(localPlayer->statistics.score)).c_str());
        }
#if defined(_DEBUG)
        if (turnDebug) {

            const std::string byteArrayBackup = std::bitset<8>(localPlayer->userMove->backup).to_string();
            const std::string byteArrayButtons = std::bitset<8>(localPlayer->userMove->buttons).to_string();

            for (int i = 0; i < 27; i++)
                printDaText(75, i, "|                                            ");

            printDaText(77, 0, "Debug:");
            printDaText(77, 2, "UserMode:");
            printDaText(77, 3, std::string("  Backup:  ").append(byteArrayBackup).append(" | ").append(std::to_string(localPlayer->userMove->backup)).c_str());
            printDaText(77, 4, std::string("  Buttons: ").append(byteArrayButtons).append(" |  ").append(std::to_string(localPlayer->userMove->buttons)).c_str());
            printDaText(77, 6, "GlobalVars:");
            printDaText(77, 7, std::string("  TickCount: ").append(std::to_string(data.globalVars->tickCount)).c_str());
            printDaText(77, 8, std::string("  CurrentTime: ").append(std::to_string(data.globalVars->currentTime)).c_str());
            printDaText(77, 9, std::string("  Width: ").append(std::to_string(data.globalVars->width)).c_str());
            printDaText(77, 10, std::string("  Height: ").append(std::to_string(data.globalVars->height)).c_str());
            printDaText(77, 12, std::string("LocalPlayer: 0x").append(std::format("{:x}", localPlayer.getPtr())).c_str());
            printDaText(77, 13, std::string("  Entity Classname: ").append(localPlayer->identify->classname).c_str());
            printDaText(77, 14, std::string("  Entity Name: ").append(localPlayer->identify->name).c_str());
            printDaText(77, 15, std::string("  Nick: ").append(localPlayer->nickname).c_str());
            printDaText(77, 16, std::string("  Origin: x: ").append(std::to_string(localPlayer->origin.x)).append(" y: ").append(std::to_string(localPlayer->origin.y)).c_str());
            printDaText(77, 17, std::string("  Behaviour: 0x").append(std::format("{:x}", (uintptr_t)localPlayer->behavior)).c_str());
            printDaText(77, 18, std::string("    Trusted: ").append(localPlayer->behavior->trusted ? "Yes (" : "No (").append(std::to_string(localPlayer->behavior->level)).append(")").c_str());
            printDaText(77, 19, std::string("    Time: ").append(std::to_string(localPlayer->behavior->time)).c_str());
            printDaText(77, 20, std::string("    Reason: ").append(std::to_string((int)localPlayer->behavior->reason)).c_str());

            printDaText(77 + 36, 0, "Entity List:");
            int highest = entityList->getHighestEntityIndex();
            const auto& entitylist = entityList->get();
            highest = std::clamp(highest, 0, 26);
            for (int i = 0; i < highest /* 26 is max we can render : ( */ ; i++) {
                std::string text = " ";
                text += std::to_string(i);
                if(i < 10)
                text += "  -> [0x";
                else
                text += " -> [0x";
                text += std::format("{:x}", entitylist[i]->address );
                text += "] -> ";
                text += entitylist[i]->identify->classname;

                printDaText(77 + 36, i+1, text.c_str());

            }
                

        }
#endif
        printDaText(0, 26, std::string("HP: " + std::to_string(localPlayer->health)).c_str());
        printDaText(localPlayer->origin.x, localPlayer->origin.y, &localPlayer->charDraw);
        static int cooldown = 32;
        if (cooldown > 0)
            cooldown--;

        if (localPlayer->userMove->buttons & UserMoveButtons::FORWARD)
            localPlayer->origin.y -= 1;
        if (localPlayer->userMove->buttons & UserMoveButtons::LEFT)
            localPlayer->origin.x -= 1;
        if (localPlayer->userMove->buttons & UserMoveButtons::BACKWARD)
            localPlayer->origin.y += 1;
        if (localPlayer->userMove->buttons & UserMoveButtons::RIGHT)
            localPlayer->origin.x += 1;
        if (localPlayer->userMove->buttons & UserMoveButtons::ATTACK && cooldown == 0) {
            entityList->addEntity(new Bullet{ true,  localPlayer->userMove->direction, localPlayer->origin });
            cooldown = 32;
        }

    }
}

void update() noexcept {
    while (isRunning) {
        THREAD_SLEEP(32);
        Lock lock;
        data.globalVars->tickCount = static_cast<float>(GetTickCount64() / 1000.f);
        const static float tickCountStatic = data.globalVars->tickCount;
        data.globalVars->currentTime = static_cast<float>(GetTickCount64() / 1000.f) - tickCountStatic;

        if (!localPlayer)
            continue;


        const auto mines = entityList->findEntitiesByClassName("entity_mine");
        for (const auto& it_ : mines) {
            Mine* it = static_cast<Mine*>(it_);
            if (localPlayer->origin == it->origin) {
                localPlayer->health -= it->damage;
                it->recreate(rand());
            }
        }

        const auto points = entityList->findEntitiesByClassName("entity_scorepoint");
        for (const auto& it_ : points) {
            Point* it = static_cast<Point*>(it_);
            if (localPlayer->origin == it->origin) {
                localPlayer->statistics.score++;
                it->recreate(rand());
            }
        }

        const auto& bullets = entityList->findEntitiesByClassName("entity_bullet");
        for (const auto& it_ : bullets) {
            Bullet* it = static_cast<Bullet*>(it_);
            it->update();
        }
        
        if (localPlayer->health < 1)
            printDaText(32, 12, "YOU DIED!");

        const auto& origin = localPlayer->origin;
        localPlayer->origin.x = std::clamp(origin.x, 0, 74);
        localPlayer->origin.y = std::clamp(origin.y, 0, 26);

    }
}

int main() {
    data.globalVars = new GlobalVars;
    entityList = new EntityList;
    isRunning = true;
    data.globalVars->width = 640;
    data.globalVars->height = 480;

    std::thread(update).detach();
    std::thread(render).detach();

    SetConsoleTitle(" ");

    std::cout << "Hello World!\nProvide your username: ";
    char buffname[16];
    std::cin >> buffname;
    localPlayer.init(new Player{ buffname, {37,13} });
    entityList->addEntity(localPlayer.get());
    
    std::thread(userMove, localPlayer.get()).detach();
    std::thread(keysThread).detach();
    //std::thread(enemyAI).detach();

    srand(static_cast<int>(time(0)));
    for (size_t i = 0; i < NUMBER_OF_MINES; i++) // We add 10 Mines to game
        entityList->addEntity(new Mine{ rand()});
    entityList->addEntity(new Point{ rand() });
#if !defined(_DEBUG)
    AntiCheat ac;
    std::thread(&AntiCheat::thread, AntiCheat(localPlayer.get())).detach();
    std::thread(&AntiCheat::overSleep, AntiCheat(localPlayer.get())).detach();
#endif // defined(_DEBUG)

    while (!GetAsyncKeyState(VK_INSERT) && isRunning)
        THREAD_SLEEP(10);
    
    isRunning = false;
    THREAD_SLEEP(100);
#if !defined(_DEBUG)
    ac.~AntiCheat();
#endif // defined(_DEBUG)
    if (!localPlayer->behavior->trusted) {
        printDaText(32, 12, "You have been Banned!");
        while (!GetAsyncKeyState(VK_SPACE))
            THREAD_SLEEP(10);
    }
    return EXIT_SUCCESS;
}