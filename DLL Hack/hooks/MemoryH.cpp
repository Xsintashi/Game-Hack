#include <algorithm>
#include <array>
#include <cstring>
#include <limits>
#include <string_view>
#include <utility>
#include <Windows.h>
#include <Psapi.h>
#include <cassert>
#include <string>
#include "MemoryH.h"
#include "../pch.h"

template <typename T>
static constexpr auto relativeToAbsolute(uintptr_t address) noexcept {
    return (T)(address + 4 + *reinterpret_cast<std::int32_t*>(address));
}

static std::pair<void*, std::size_t> getModuleInformation(const char* name) noexcept {
    if (HMODULE handle = GetModuleHandleA(name)) {
        if (MODULEINFO moduleInfo; GetModuleInformation(GetCurrentProcess(), handle, &moduleInfo, sizeof(moduleInfo)))
            return std::make_pair(moduleInfo.lpBaseOfDll, moduleInfo.SizeOfImage);
    }
    return {};
}

[[nodiscard]] static auto generateBadCharTable(std::string_view pattern) noexcept {
    assert(!pattern.empty());

    std::array<std::size_t, (std::numeric_limits<std::uint8_t>::max)() + 1> table;

    auto lastWildcard = pattern.rfind('?');
    if (lastWildcard == std::string_view::npos)
        lastWildcard = 0;

    const auto defaultShift = (std::max)(std::size_t(1), pattern.length() - 1 - lastWildcard);
    table.fill(defaultShift);

    for (auto i = lastWildcard; i < pattern.length() - 1; ++i)
        table[static_cast<std::uint8_t>(pattern[i])] = pattern.length() - 1 - i;

    return table;
}

static std::uintptr_t findPattern(const char* moduleName, std::string_view pattern, bool reportNotFound = true) noexcept
{
    static auto id = 0;
    ++id;

    const auto [moduleBase, moduleSize] = getModuleInformation(moduleName);

    if (moduleBase && moduleSize) {
        const auto lastIdx = pattern.length() - 1;
        const auto badCharTable = generateBadCharTable(pattern);

        auto start = static_cast<const char*>(moduleBase);
        const auto end = start + moduleSize - pattern.length();

        while (start <= end) {
            size_t i = lastIdx;
            while (i >= 0 && (pattern[i] == '?' || start[i] == pattern[i]))
                --i;

            if (i < 0)
                return reinterpret_cast<std::uintptr_t>(start);

            start += badCharTable[static_cast<std::uint8_t>(start[lastIdx])];
        }
    }

    if (reportNotFound)
        MessageBoxA(NULL, ("Failed to find pattern #" + std::to_string(id) + '!').c_str(), "DLL Example", MB_OK | MB_ICONWARNING);
    return 0;
}

MemoryH::MemoryH() noexcept {
    base = (uintptr_t)GetModuleHandle(__GAME__);
    update = 0x6140;
    userMove = 0x1AE0;

}
