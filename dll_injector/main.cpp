#include <windows.h>
#include <iostream>
#include <TlHelp32.h>

void LoadDll(HANDLE process, const wchar_t* dllPath)
{
	int namelen = wcslen(dllPath) + 1;																								//Gets the length of the string so we can know how much mem we need wcslen gets the length of a wchar_T
	LPVOID remoteString = VirtualAllocEx(process, nullptr, namelen * 2, MEM_COMMIT, PAGE_EXECUTE);									//We Allocate new Memory for our dll to be loaded into the memory  check out the msdn documentation it is trivial https://docs.microsoft.com/en-us/windows/win32/api/memoryapi/nf-memoryapi-virtualallocex 
	WriteProcessMemory(process, remoteString, dllPath, namelen * 2, nullptr);														//Simple WPM to write our dll path into memory from csgo

	HMODULE kernel32 = GetModuleHandle("kernel32.dll");																				//We get the handle from kernel32.dll so we can call then the function LoadLibraryW which is in kernel32 included
	std::cout << "Kernal Module Handle: " << kernel32 << '\n';																		

	LPVOID funcAdr = GetProcAddress(kernel32, "LoadLibraryW");																		//We get the address for the function LoadLibraryW which is importatent because we will call witht his function our dll.
	std::cout << "LoadLibraryW function address: " << funcAdr << '\n';

	HANDLE thread = CreateRemoteThread(process, NULL, NULL, (LPTHREAD_START_ROUTINE)funcAdr, remoteString, NULL, NULL);				//We Create a new Remote Thread which is calling the LoadLibrary Address and is Loading our dll into csgo process.                         https://docs.microsoft.com/en-us/windows/win32/api/processthreadsapi/nf-processthreadsapi-createremotethread
	

	WaitForSingleObject(thread, INFINITE);
	CloseHandle(thread);
}

int main()
{
	HWND myWindow = FindWindowA(nullptr, "Counter-Strike: Global Offensive");														//Searching for Window name and creates Handle which is stored in myWindow  https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-findwindowa
	if (myWindow == NULL)																											//Checking if Window found...
	{
		MessageBox(nullptr, "Couldn't found game Window", "Error" ,MB_OK | MB_ICONERROR);
		exit(1);
	}

	DWORD PID;																														//Process ID stored as DWORD
	GetWindowThreadProcessId(myWindow, &PID);																						//Function obtains PID by taking window handle as first arguemnt and as second rference to variable to safe PID    https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-getwindowthreadprocessid
	
	HANDLE proc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, PID);																		//Opens a Process with ALL Access u can change to write only too    https://docs.microsoft.com/en-us/windows/win32/api/processthreadsapi/nf-processthreadsapi-openprocess
	
	LoadDll(proc, L"c:\\CSGO-Internal-Cheat.dll");  // <--- Change DLL Path to whatever you want hardcoded xD

	return 0;
}