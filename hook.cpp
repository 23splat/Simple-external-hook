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

	grenade_health hook:
	when a grenade is fired and a player is in close enough distance to take damage, the instructions mov[edx + 0000000EC], eax is executed
	the 'edx' register being the local player, and edx + EC being the address of player health
	we dereference this address, and move the 'eax' register (which stores a value) to it 
	now [edx + 0000000EC] is equal to eax, therefore eax must be the new value of our health
	right before this instruction at address 00484495(static), the instruction: mov    eax,DWORD PTR [esp+x] is executed, 'x' being the amount of damage we took
	we then hook this, and change 'x' to any value that we want: {0x8B, 0x44, 0x24, new_value}
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
	DWORD health = 0x00484495;
	BYTE health_shell_code[] = {0x8B, 0x44, 0x24, 0x70}; //  mov    eax,DWORD PTR [esp+0x70]
	BYTE shell_code[] = { 0xFF, 0x00 }; // FF 08 //  *(DWORD*)eax-- (BULLET FIRE) inc[eax] *(DWORD*)eax++
	BYTE nop[] = { 0x90, 0x90 };// nop (break reload)
	WriteProcessMemory(get_handle(), (LPVOID)instructions, &shell_code, sizeof(shell_code), NULL);
	WriteProcessMemory(get_handle(), (LPVOID)nopcode, &nop, sizeof(nop), NULL);
	WriteProcessMemory(get_handle(), (LPVOID)health, &health_shell_code, sizeof(health_shell_code), NULL);
	return true;
}

int main()
{
	hook();
}
