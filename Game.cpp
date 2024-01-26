#include <iostream>
#include <thread>
#include <vector>
#include <string>

#include "AntiCheat.h"
#include "Data.h"
#include "GUI.h"

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

void keysThread() {
    while (isRunning) {
        THREAD_SLEEP(100);
        if (GetAsyncKeyState(VK_MENU) & 0x8000)
            data.turnDebug = !data.turnDebug;
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

void update() noexcept {
    while (isRunning) {
        THREAD_SLEEP(32);
        Lock lock;
        data.globalVars->tickCount = static_cast<float>(GetTickCount64() / 1000.f);
        const static float tickCountStatic = data.globalVars->tickCount;
        data.globalVars->currentTime = static_cast<float>(GetTickCount64() / 1000.f) - tickCountStatic;

        if (!localPlayer)
            continue;

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
            GUI::printOnScreen(32, 12, "YOU DIED!");

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
    std::thread(GUI::render).detach();

    SetConsoleTitle(" ");
    
    while(GUI::stage != Stage::GAME)
        THREAD_SLEEP(10);

    std::thread(userMove, localPlayer.get()).detach();
    std::thread(keysThread).detach();

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
    return EXIT_SUCCESS;
}