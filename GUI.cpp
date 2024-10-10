#include "GUI.h"
#include "Data.h"
#include "Lock.h"

#include <iostream>
#include <bitset>
#include <format>
#include <fstream>
#include <string>
#include <sstream>

#include "Classes/Entity.h"
#include "Classes/EntityList.h"
#include "Classes/GlobalVars.h"
#include "Classes/Macros.h"
#include "Classes/Mine.h"
#include "Classes/Player.h"
#include "Classes/Point.h"
#include "Classes/LocalPLayer.h"
#include <cassert>

void mainMenu() noexcept {
    constexpr const char* leftArrow = "->";
    constexpr const char* rightArrow = "<-";
    static int item = 1;
    if(!data.username.empty())
        GUI::printOnScreen(0, 0, std::string("Logged as: \033[32m").append(data.username).append("\033[0m").c_str());
    GUI::printOnScreenCentered(7, "The Game :)");

    if (GetAsyncKeyState('S') & 0x8000)
        item++;
    if (GetAsyncKeyState('W') & 0x8000)
        item--;
    if(!data.username.empty())
        GUI::printOnScreenCentered(9, "Play!");
    GUI::printOnScreenCentered(10, "Login");
    GUI::printOnScreenCentered(11, "Register");
    GUI::printOnScreenCentered(12, "Quit!");

    item = std::clamp(item, data.username.empty() ? 1 : 0, 3);

    GUI::printOnScreen(26, 9 + item, leftArrow);
    GUI::printOnScreen(46, 9 + item, rightArrow);

    system("pause >nul");
    if (!(GetAsyncKeyState(VK_RETURN) & 0x8000))
        return;

    switch (item) {
    case 0: 
        localPlayer.init(new Player{ data.username.c_str(), {37,13}});
        entityList->addEntity(localPlayer.get());

        srand(static_cast<int>(time(0)));
        for (size_t i = 0; i < NUMBER_OF_MINES; i++) // We add 10 Mines to game
            entityList->addEntity(new Mine{ rand() });
        entityList->addEntity(new Point{ rand() });

        GUI::stage = Stage::GAME;
        break;
    default:
    case 1:
        GUI::stage = Stage::LOGIN;
        break;
    case 2:
        GUI::stage = Stage::REGISTER;
        break;
    case 3:
        isRunning = false;
        break;
    }

}

void registerWindow() noexcept {
    constexpr const char* leftArrow = "->";
    static std::string login;
    static std::string password;
    static std::string invite;
    static int item = 0;
    GUI::printOnScreenCentered(7, "Register Form:");

    if (GetAsyncKeyState('S') & 0x8000)
        item++;
    if (GetAsyncKeyState('W') & 0x8000)
        item--;

    item = std::clamp(item, 0, 4);

    static std::string hiddenPass;
    hiddenPass.clear();

    for (size_t i = 0; i < password.size(); i++)
        hiddenPass += "*";

    GUI::printOnScreen(26, 9, std::string("Login: ").append(login).c_str());
    GUI::printOnScreen(26, 10, std::string("Password: ").append(hiddenPass).c_str());
    GUI::printOnScreen(26, 11, std::string("Invite: ").append(invite).c_str());
    GUI::printOnScreenCentered(13, "Register!");
    GUI::printOnScreenCentered(14, "Back");

    if(item < 3)
        GUI::printOnScreen(23, 9 + item, leftArrow);
    else
        GUI::printOnScreen(23, 10 + item, leftArrow);

    system("pause >nul");
    if (!(GetAsyncKeyState(VK_RETURN) & 0x8000))
        return;

    HANDLE handle = GetStdHandle(STD_INPUT_HANDLE);
    DWORD mode;

    switch (item) {
    default:
    case 0: {
        GetConsoleMode(handle, &mode);
        mode |= ENABLE_ECHO_INPUT;
        SetConsoleMode(handle, mode);

        GUI::printOnScreen(33, 9, "________________");
        GUI::printOnScreen(33, 9, "");
        std::cin >> login;
        std::string::iterator end_pos = std::remove(login.begin(), login.end(), ' ');
        login.erase(end_pos, login.end());
        break;
    }
    case 1: {
        GetConsoleMode(handle, &mode);
        mode &= ~ENABLE_ECHO_INPUT;
        SetConsoleMode(handle, mode);
        GUI::printOnScreen(36, 10, "___________");
        GUI::printOnScreen(36, 10, "");
        std::cin >> password;
        std::string::iterator end_pos = std::remove(password.begin(), password.end(), ' ');
        password.erase(end_pos, password.end());
        break;
    }
    case 2: {
        GetConsoleMode(handle, &mode);
        mode |= ENABLE_ECHO_INPUT;
        SetConsoleMode(handle, mode);
        GUI::printOnScreen(34, 11, "______________");
        GUI::printOnScreen(34, 11, "");
        std::cin >> invite;
        std::string::iterator end_pos = std::remove(invite.begin(), invite.end(), ' ');
        invite.erase(end_pos, invite.end());
        break;
    }
    case 3: {

        //if(invite == "TRIAL_TEST_30")
        std::fstream file;
        file.open("loginbase", std::ios_base::app);
        if (file.good()) {
            file << login << ' ' << password << ' ' << invite << ' ';
        }
        file.close();
        GUI::stage = Stage::MAIN_MENU;
        break;
    }
    case 4:
        GUI::stage = Stage::MAIN_MENU;
        break;
    }
}

void login() noexcept {
    static int sucessLogin = -1;
    constexpr const char* leftArrow = "->";
    static std::string login;
    static std::string password;
    static int item = 0;
    HANDLE handle = GetStdHandle(STD_INPUT_HANDLE);
    DWORD mode;

    GUI::printOnScreenCentered(7, "Login Form:");

    if (GetAsyncKeyState('S') & 0x8000)
        item++;
    if (GetAsyncKeyState('W') & 0x8000)
        item--;

    item = std::clamp(item, 0, 3);

    static std::string hiddenPass;
    hiddenPass.clear();

    for (size_t i = 0; i < password.size(); i++)
        hiddenPass += "*";

    GUI::printOnScreen(26, 9, std::string("Login: ").append(login).c_str());
    GUI::printOnScreen(26, 10, std::string("Password: ").append(hiddenPass).c_str());
    GUI::printOnScreenCentered(12, "Log in!");
    GUI::printOnScreenCentered(13, "Back");

    if (item < 2)
        GUI::printOnScreen(23, 9 + item, leftArrow);
    else
        GUI::printOnScreen(23, 10 + item, leftArrow);

    if (sucessLogin == 0) {
        GUI::printOnScreenCentered(15, "\033[91mInvalid Login or password\033[0m");
        GUI::printOnScreenCentered(16, "\033[91mTry Again!\033[0m");
    }

    system("pause >nul");
    if (!(GetAsyncKeyState(VK_RETURN) & 0x8000))
        return;

    switch (item) {
    default:
    case 0:
        GetConsoleMode(handle, &mode);
        mode |= ENABLE_ECHO_INPUT;
        SetConsoleMode(handle, mode);

        GUI::printOnScreen(33, 9, "________________");
        GUI::printOnScreen(33, 9, "");
        std::cin >> login;
        break;
    case 1:
        GetConsoleMode(handle, &mode);
        mode &= ~ENABLE_ECHO_INPUT;
        SetConsoleMode(handle, mode);
        GUI::printOnScreen(36, 10, "___________");
        GUI::printOnScreen(36, 10, "");
        std::cin >> password;
        break;
    case 2: {
        //if(invite == "TRIAL_TEST_30")
        std::fstream file;
        file.open("loginbase");
        if (file.good()) {
            std::string loginbase;
            while (std::getline(file, loginbase)) {
                std::stringstream ss(loginbase);
                std::istream_iterator<std::string> begin(ss);
                std::istream_iterator<std::string> end;
                std::vector<std::string> slices(begin, end);

                int i = 0;
                do {
                    if (slices[i] == login && slices[i + 1] == password) {
                        data.username = login.c_str();
                        sucessLogin = 1;
                    }
                    else
                        sucessLogin = 0;
                    i++;
                } while (i < slices.size() - 2);
            }
        }
        file.close();

        

        if (sucessLogin == 1)
            GUI::stage = Stage::MAIN_MENU;
        break;
    }
    case 3:
        GUI::stage = Stage::MAIN_MENU;
        break;
    }
}

void gameplay() noexcept {

    if (!localPlayer)
        return;

    if (localPlayer->health < 1)
        return;

    GUI::printOnScreen(0, 0, std::string("Nick: \033[32m").append(data.username).append("\033[0m").c_str());

    const auto& mines = entityList->findEntitiesByClassName("entity_mine");
    for (const auto& it : mines)
        GUI::printOnScreen(it->origin.x, it->origin.y, std::string("\033[31m").append(&it->charDraw).append("\033[37m").c_str());

    const auto& bullets = entityList->findEntitiesByClassName("entity_bullet");
    for (const auto& it : bullets)
        GUI::printOnScreen(it->origin.x, it->origin.y, &it->charDraw);

    const auto& points = entityList->findEntitiesByClassName("entity_scorepoint");
    for (const auto& it : points)
        GUI::printOnScreen(it->origin.x, it->origin.y, std::string("\033[33m").append(&it->charDraw).append("\033[37m").c_str());

    if (GetAsyncKeyState(VK_TAB)) {
        GUI::printOnScreen(0, 25, std::string("Score: " + std::to_string(localPlayer->statistics.score)).c_str());
    }
#if defined(_DEBUG)
    if (data.turnDebug) {

        const std::string byteArrayBackup = std::bitset<8>(localPlayer->userMove->backup).to_string();
        const std::string byteArrayButtons = std::bitset<8>(localPlayer->userMove->buttons).to_string();

        for (int i = 0; i < 27; i++)
            GUI::printOnScreen(75, i, "|                                            ");

        GUI::printOnScreen(77, 0, "Debug:");
        GUI::printOnScreen(77, 2, "UserMode:");
        GUI::printOnScreen(77, 3, std::string("  Backup:  ").append(byteArrayBackup).append(" | ").append(std::to_string(localPlayer->userMove->backup)).c_str());
        GUI::printOnScreen(77, 4, std::string("  Buttons: ").append(byteArrayButtons).append(" |  ").append(std::to_string(localPlayer->userMove->buttons)).c_str());
        GUI::printOnScreen(77, 6, "GlobalVars:");
        GUI::printOnScreen(77, 7, std::string("  TickCount: ").append(std::to_string(data.globalVars->tickCount)).c_str());
        GUI::printOnScreen(77, 8, std::string("  CurrentTime: ").append(std::to_string(data.globalVars->currentTime)).c_str());
        GUI::printOnScreen(77, 9, std::string("  Width: ").append(std::to_string(data.globalVars->width)).c_str());
        GUI::printOnScreen(77, 10, std::string("  Height: ").append(std::to_string(data.globalVars->height)).c_str());
        GUI::printOnScreen(77, 12, std::string("LocalPlayer: 0x").append(std::format("{:x}", localPlayer.getPtr())).c_str());
        GUI::printOnScreen(77, 13, std::string("  Entity Classname: ").append(localPlayer->identify->classname).c_str());
        GUI::printOnScreen(77, 14, std::string("  Entity Name: ").append(localPlayer->identify->name).c_str());
        GUI::printOnScreen(77, 15, std::string("  Nick: ").append(localPlayer->nickname).c_str());
        GUI::printOnScreen(77, 16, std::string("  Origin: x: ").append(std::to_string(localPlayer->origin.x)).append(" y: ").append(std::to_string(localPlayer->origin.y)).c_str());
        GUI::printOnScreen(77, 17, std::string("  Behaviour: 0x").append(std::format("{:x}", (uintptr_t)localPlayer->behavior)).c_str());
        GUI::printOnScreen(77, 18, std::string("    Trusted: ").append(localPlayer->behavior->trusted ? "Yes (" : "No (").append(std::to_string(localPlayer->behavior->level)).append(")").c_str());
        GUI::printOnScreen(77, 19, std::string("    Time: ").append(std::to_string(localPlayer->behavior->time)).c_str());
        GUI::printOnScreen(77, 20, std::string("    Reason: ").append(std::to_string((int)localPlayer->behavior->reason)).c_str());

        GUI::printOnScreen(77 + 36, 0, "Entity List:");
        int highest = entityList->getHighestEntityIndex();
        const auto& entitylist = entityList->get();
        highest = std::clamp(highest, 0, 26);
        for (int i = 0; i < highest /* 26 is max we can render : ( */; i++) {
            std::string text = " ";
            text += std::to_string(i);
            if (i < 10)
                text += "  -> [0x";
            else
                text += " -> [0x";
            text += std::format("{:x}", entitylist[i]->address);
            text += "] -> ";
            text += entitylist[i]->identify->classname;

            GUI::printOnScreen(77 + 36, i + 1, text.c_str());

        }

        if (!localPlayer->behavior->trusted) {
            GUI::printOnScreen(32, 12, "You have been Banned!");
            while (!GetAsyncKeyState(VK_SPACE))
                THREAD_SLEEP(10);
            isRunning = false;
        }

    }
#endif
    const char* color = "";
    if (localPlayer->health > 75)
        color = "\033[32m";
    else if(localPlayer->health > 50)
        color = "\033[33m";
    else if(localPlayer->health > 25)
        color = "\033[31m";

    GUI::printOnScreen(0, 26, std::string(color).append("HP: " + std::to_string(localPlayer->health)).append("\033[37m").c_str());
    GUI::printOnScreen(localPlayer->origin.x, localPlayer->origin.y, std::string("\033[34m").append(&localPlayer->charDraw).append("\033[37m").c_str());
}

void GUI::showConsoleCursor(bool flag) noexcept {
    CONSOLE_CURSOR_INFO     cursorInfo;
    GetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
    if (cursorInfo.bVisible == (BOOL)flag)
        return;
    cursorInfo.bVisible = flag; // set the cursor visibility
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
}

void GUI::printOnScreenCentered(const int& y, const char* str) noexcept {
    int lengthOfText = strlen(str);
    COORD pos = { static_cast<short>((RIGHTBORDER / 2) - (lengthOfText / 2)), static_cast<short>(y) };
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
    printf(str);
}

void GUI::printOnScreen(const int& x, const int& y, const char* str) noexcept {
    COORD pos = { static_cast<short>(x), static_cast<short>(y) };
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
    printf(str);
}

void GUI::clearScreen() noexcept {
    for (int i = 0; i < 27; i++) {
        GUI::printOnScreen(0, i, "                                                                                                                                                                                         ");
    }
}

void GUI::setWindow(const DWORD& width, const DWORD& height) noexcept {
    HWND console = GetConsoleWindow();
    RECT r;
    GetWindowRect(console, &r);

    MoveWindow(console, r.left, r.top, width, height, TRUE);
}

void GUI::render() noexcept {
    //stage = Stage::PLAYERNAME;

    while (isRunning) {
        THREAD_SLEEP(32);

        Lock lock;
#if defined(_DEBUG)
        if (data.turnDebug)
            setWindow(data.globalVars->width + 768, data.globalVars->height);
        else
#endif
            setWindow(data.globalVars->width, data.globalVars->height);

        showConsoleCursor(false);

        clearScreen();
        
        switch (GUI::stage) {
        default:
        case Stage::MAIN_MENU:
            mainMenu();
            break;
        case Stage::LOGIN:
            login();
            break;
        case Stage::REGISTER:
            registerWindow();
            break;
        case Stage::GAME:
            gameplay();
            break;
        }
    }
}