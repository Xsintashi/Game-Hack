#pragma once
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <TlHelp32.h>
#include <cstdint>
#include <string_view>

class Memory {
public:
	Memory(const char* target) {
		HANDLE snapshotHandle = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

		if (snapshotHandle == INVALID_HANDLE_VALUE) {
			return;
		}

		PROCESSENTRY32 processEntry = { };
		processEntry.dwSize = sizeof(PROCESSENTRY32);

		if (Process32First(snapshotHandle, &processEntry)) {

			do {

				if (strcmp(processEntry.szExeFile, target) == 0) {
					CloseHandle(snapshotHandle);
					process = processEntry.th32ProcessID;
					handle = ::OpenProcess(PROCESS_ALL_ACCESS, FALSE, process);
					return;
				}



			} while (Process32Next(snapshotHandle, &processEntry));



		}

		CloseHandle(snapshotHandle);
		return;
	}

	template <typename type>
	type Read(uintptr_t pointerStatic) {
		type value = { };
		ReadProcessMemory(handle, (LPVOID)pointerStatic, &value, sizeof(type), NULL);
		return value;
	}

	uintptr_t GetModuleBaseAddress(const char* moduleName) {

		HANDLE snapshotHandle = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, process);

		if (snapshotHandle == INVALID_HANDLE_VALUE) {
			return NULL;
		}

		MODULEENTRY32 moduleEntry = { };
		moduleEntry.dwSize = sizeof(MODULEENTRY32);

		if (Module32First(snapshotHandle, &moduleEntry)) {

			do {

				if (strcmp(moduleEntry.szModule, moduleName) == 0) {
					CloseHandle(snapshotHandle);
					return reinterpret_cast<uintptr_t>(moduleEntry.modBaseAddr);
				}



			} while (Module32Next(snapshotHandle, &moduleEntry));



		}

		CloseHandle(snapshotHandle);
		return NULL;

	}

	template <typename type>
	bool Write(uintptr_t pointerStatic, type value) {
		return WriteProcessMemory(handle, (LPVOID)pointerStatic, &value, sizeof(type), NULL);
	}

	DWORD process;
	HANDLE handle;
};