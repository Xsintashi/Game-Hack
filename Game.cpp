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
#include "Classes/Macros.h"
#include "Classes/Mine.h"
#include "Classes/Player.h"
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
        if (!static_cast<Player*>(entityList->getEntity(0)))
            continue;

        if (!data.enemy)
            continue;

        const auto& lpOrigin = static_cast<Player*>(entityList->getEntity(0))->origin;

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

        setBoth( player->userMove->buttons, player->userMove->backup, buttons);
    }
}

void render() noexcept {
    while (isRunning) {
        THREAD_SLEEP(32);

        Lock lock;
//#if defined(_DEBUG)
        if(turnDebug)
            setWindow(data.globalVars->width + 768, data.globalVars->height);
        else
//#endif
            setWindow(data.globalVars->width, data.globalVars->height);

        if (!static_cast<Player*>(entityList->getEntity(0)))
            continue;

        showConsoleCursor(false);

        if (static_cast<Player*>(entityList->getEntity(0))->health < 1)
            continue;
        clearScreen();
        printDaText(0, 0, std::string("Nick: " + static_cast<Player*>(entityList->getEntity(0))->nickname).c_str());
        if(data.enemy)
            printDaText(data.enemy->origin.x, data.enemy->origin.y, "@");

        const auto& mines = entityList->findEntitiesByClassName("entity_mine");
        for (const auto& it : mines)
            printDaText(it->origin.x, it->origin.y, "o");

        const auto& bullets = entityList->findEntitiesByClassName("entity_bullet");
        for (const auto& it : bullets)
            printDaText(it->origin.x, it->origin.y, "*");

        if (GetAsyncKeyState(VK_TAB)) {
            printDaText(0, 25, std::string("Score: " + std::to_string(static_cast<Player*>(entityList->getEntity(0))->statistics.score)).c_str());
        }
#if defined(_DEBUG)
        if (turnDebug) {

            const std::string byteArrayBackup = std::bitset<8>(static_cast<Player*>(entityList->getEntity(0))->userMove->backup).to_string();
            const std::string byteArrayButtons = std::bitset<8>(static_cast<Player*>(entityList->getEntity(0))->userMove->buttons).to_string();

            for (int i = 0; i < 27; i++)
                printDaText(75, i, "|                                            ");

            printDaText(77, 0, "Debug:");
            printDaText(77, 2, "UserMode:");
            printDaText(77, 3, std::string("  Backup:  ").append(byteArrayBackup).append(" | ").append(std::to_string(static_cast<Player*>(entityList->getEntity(0))->userMove->backup)).c_str());
            printDaText(77, 4, std::string("  Buttons: ").append(byteArrayButtons).append(" |  ").append(std::to_string(static_cast<Player*>(entityList->getEntity(0))->userMove->buttons)).c_str());
            printDaText(77, 6, "GlobalVars:");
            printDaText(77, 7, std::string("  TickCount: ").append(std::to_string(data.globalVars->tickCount)).c_str());
            printDaText(77, 8, std::string("  CurrentTime: ").append(std::to_string(data.globalVars->currentTime)).c_str());
            printDaText(77, 9, std::string("  Width: ").append(std::to_string(data.globalVars->width)).c_str());
            printDaText(77, 10, std::string("  Height: ").append(std::to_string(data.globalVars->height)).c_str());
            printDaText(77, 12, std::string("LocalPlayer: 0x").append(std::format("{:x}", static_cast<Player*>(entityList->getEntity(0))->address)).c_str());
            printDaText(77, 13, std::string("  Entity Classname: ").append(static_cast<Player*>(entityList->getEntity(0))->identify->classname).c_str());
            printDaText(77, 14, std::string("  Entity Name: ").append(static_cast<Player*>(entityList->getEntity(0))->identify->name).c_str());
            printDaText(77, 15, std::string("  Nick: ").append(static_cast<Player*>(entityList->getEntity(0))->nickname).c_str());
            printDaText(77, 16, std::string("  Origin: x: ").append(std::to_string(static_cast<Player*>(entityList->getEntity(0))->origin.x)).append(" y: ").append(std::to_string(static_cast<Player*>(entityList->getEntity(0))->origin.y)).c_str());
            printDaText(77, 17, std::string("  Behaviour: 0x").append(std::format("{:x}", (uintptr_t)static_cast<Player*>(entityList->getEntity(0))->behavior)).c_str());
            printDaText(77, 18, std::string("    Trusted: ").append(static_cast<Player*>(entityList->getEntity(0))->behavior->trusted ? "Yes (" : "No (").append(std::to_string(static_cast<Player*>(entityList->getEntity(0))->behavior->level)).append(")").c_str());
            printDaText(77, 19, std::string("    Time: ").append(std::to_string(static_cast<Player*>(entityList->getEntity(0))->behavior->time)).c_str());
            printDaText(77, 20, std::string("    Reason: ").append(std::to_string((int)static_cast<Player*>(entityList->getEntity(0))->behavior->reason)).c_str());

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
        printDaText(0, 26, std::string("HP: " + std::to_string(static_cast<Player*>(entityList->getEntity(0))->health)).c_str());
        printDaText(static_cast<Player*>(entityList->getEntity(0))->origin.x, static_cast<Player*>(entityList->getEntity(0))->origin.y, "#");

        if (static_cast<Player*>(entityList->getEntity(0))->userMove->buttons & UserMoveButtons::FORWARD)
            static_cast<Player*>(entityList->getEntity(0))->origin.y -= 1;
        if (static_cast<Player*>(entityList->getEntity(0))->userMove->buttons & UserMoveButtons::LEFT)
            static_cast<Player*>(entityList->getEntity(0))->origin.x -= 1;
        if (static_cast<Player*>(entityList->getEntity(0))->userMove->buttons & UserMoveButtons::BACKWARD)
            static_cast<Player*>(entityList->getEntity(0))->origin.y += 1;
        if (static_cast<Player*>(entityList->getEntity(0))->userMove->buttons & UserMoveButtons::RIGHT)
            static_cast<Player*>(entityList->getEntity(0))->origin.x += 1;

    }
}

void update() noexcept {
    while (isRunning) {
        THREAD_SLEEP(32);
        data.globalVars->tickCount = static_cast<float>(GetTickCount64() / 1000.f);
        const static float tickCountStatic = data.globalVars->tickCount;
        data.globalVars->currentTime = static_cast<float>(GetTickCount64() / 1000.f) - tickCountStatic;

        if (!static_cast<Player*>(entityList->getEntity(0)))
            continue;


        const auto mines = entityList->findEntitiesByClassName("entity_mine");
        for (const auto& it_ : mines) {
            Mine* it = static_cast<Mine*>(it_);
            if (static_cast<Player*>(entityList->getEntity(0))->origin == it->origin) {
                static_cast<Player*>(entityList->getEntity(0))->health -= it->damage;
                it->recreate(static_cast<int>(time(0) - 1));
            }
        }

        static int cooldown = 32;
        if (GetAsyncKeyState(VK_SPACE) && cooldown == 0) {
            entityList->addEntity(new Bullet{ true,  static_cast<Player*>(entityList->getEntity(0))->userMove->direction, static_cast<Player*>(entityList->getEntity(0))->origin });
            cooldown = 32;
        }
        
        if (cooldown > 0)
            cooldown--;

        const auto& bullets = entityList->findEntitiesByClassName("entity_bullet");
        for (const auto& it_ : bullets) {
            Bullet* it = static_cast<Bullet*>(it_);
            it->update();
        }
        
        if (static_cast<Player*>(entityList->getEntity(0))->health < 1)
            printDaText(32, 12, "YOU DIED!");

        const auto& origin = static_cast<Player*>(entityList->getEntity(0))->origin;
        static_cast<Player*>(entityList->getEntity(0))->origin.x = std::clamp(origin.x, 0, 74);
        static_cast<Player*>(entityList->getEntity(0))->origin.y = std::clamp(origin.y, 0, 26);

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
    entityList->addEntity(new Player{ buffname, { 37,13 } });
    
    std::thread(userMove, static_cast<Player*>(entityList->getEntity(0))).detach();
    std::thread(keysThread).detach();
    //std::thread(enemyAI).detach();

    srand(static_cast<int>(time(0)));
    for (size_t i = 0; i < NUMBER_OF_MINES; i++) // We add 10 Mines to game
        entityList->addEntity(new Mine{ rand()});
#if !defined(_DEBUG)
    AntiCheat ac;
    std::thread(&AntiCheat::thread, AntiCheat()).detach();
    std::thread(&AntiCheat::overSleep, AntiCheat()).detach();
#endif // defined(_DEBUG)

    while (!GetAsyncKeyState(VK_INSERT) && isRunning)
        THREAD_SLEEP(10);
    
    isRunning = false;
    THREAD_SLEEP(100);
#if !defined(_DEBUG)
    ac.~AntiCheat();
#endif // defined(_DEBUG)
    if (!static_cast<Player*>(entityList->getEntity(0))->behavior->trusted) {
        printDaText(32, 12, "You have been Banned!");
        while (!GetAsyncKeyState(VK_SPACE))
            THREAD_SLEEP(10);
    }
    return EXIT_SUCCESS;
}