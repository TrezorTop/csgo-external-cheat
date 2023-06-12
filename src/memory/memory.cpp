#include "memory.h"

Memory::Memory(const char* processName) {
	// contains information about a running process
	PROCESSENTRY32 currentProcess;
	// for compatibility and versioning purposes
	currentProcess.dwSize = sizeof(PROCESSENTRY32);

	// handle to snapshot of information about process
	// includes all processes in the system because no process id specified
	const HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	if (snapshot == INVALID_HANDLE_VALUE) {
		throw std::runtime_error("Failed to create process snapshot");
	}

	// iterating through the list of processes in a system snapshot
	while (Process32Next(snapshot, &currentProcess)) {
		// string compare (specified name and exe file of current process)
		if (strcmp(processName, currentProcess.szExeFile) == 0) {
			// saving process id
			this->processId = currentProcess.th32ProcessID;

			// windows api function to open a handle to a process with full access and save it
			this->process = OpenProcess(PROCESS_ALL_ACCESS, FALSE, this->processId);
			break;
		}
	}

	if (snapshot) CloseHandle(snapshot);

	if (!this->process) {
		throw std::runtime_error("Failed to find the specified process");
	}
}

Memory::~Memory() {
	if (this->process) CloseHandle(this->process);
}

DWORD Memory::GetProcessId() {
	return this->processId;
}

HANDLE Memory::GetProcess() {
	return this->process;
}

// very similar to seeking process
uintptr_t Memory::GetModuleAddress(const char* moduleName) {
	// contains information about a process module (DLL or executable)
	MODULEENTRY32 currentModule;
	// for compatibility and versioning purposes
	currentModule.dwSize = sizeof(MODULEENTRY32);

	// handle to snapshot of information about modules
	const HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, this->processId);

	if (snapshot == INVALID_HANDLE_VALUE) {
		throw std::runtime_error("Failed to create module snapshot");
	}

	uintptr_t module = 0;

	// iterating through the modules of process
	while (Module32Next(snapshot, &currentModule)) {
		// string compare (specified name and name of current module)
		if (strcmp(moduleName, currentModule.szModule) == 0) {
			module = reinterpret_cast<uintptr_t>(currentModule.modBaseAddr);
			break;
		}
	}

	if (snapshot) CloseHandle(snapshot);

	if (module == 0) {
		throw std::runtime_error("Failed to find the specified module");
	}

	return module;
}