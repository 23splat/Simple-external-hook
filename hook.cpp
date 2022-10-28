#include <windows.h>
#include <iostream>


/*
	simple explanation:
	bullet hook:  
	when a bullet is fired, eax is derefenced and decremented, meaning [eax] stores the value of the ammount of bullets we have
	the instructions in asm is 0xFF, 0x08, we simply hook this and replace it with an increment instruction (0xFF, 0x00)
	now every time a bullet is fired, [eax] is incremented, so it is impossible to run out of bullets
	in other words, we gain a bullet each time we fire 
	however, pressing 'r' (reloading) will set the amount of bullets we have to 30
	so we hook this and set each instruction to 0x90, which performs nothing (noop)

*/


HANDLE get_handle()
{
	HWND hwnd = FindWindowA(NULL, "AssaultCube");
	DWORD processID;
	GetWindowThreadProcessId(hwnd, &processID);
	HANDLE handle = OpenProcess(PROCESS_ALL_ACCESS, false, processID);
	return handle;
}

bool hook()
{
	DWORD instructions = 0x004C73EF; // dec[eax] (BULLET FIRE) FF 08
	DWORD nopcode = 0x004C8F90;
	BYTE shell_code[] = { 0xFF, 0x00 }; // FF 08 //  *(DWORD*)eax-- (BULLET FIRE) inc[eax] *(DWORD*)eax++
	BYTE nop[] = { 0x90, 0x90 };// nop (break reload)
	WriteProcessMemory(get_handle(), (LPVOID)instructions, &shell_code, sizeof(shell_code), NULL);
	WriteProcessMemory(get_handle(), (LPVOID)nopcode, &nop, sizeof(nop), NULL);
	return true;
}

int main()
{
	hook();
}
