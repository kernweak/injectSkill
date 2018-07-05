// 远程线程注入.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include<windows.h>
BOOL LoadDll(DWORD dwProcessID, char *szDllPathName) {//第一个参数进程ID，加载dll的完整路径
	BOOL bRet;
	HANDLE hProcess;
	HANDLE hThread;
	DWORD dwLength;
	DWORD dwLoadAddr;
	LPVOID lpAllocAddr;
	DWORD dwThreadID;
	HMODULE hModule;
	bRet = 0;
	dwLoadAddr = 0;
	hProcess = 0;
	//1.获取进程句柄
	hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwProcessID);
	if (hProcess == NULL) {
		OutputDebugString(L"OpenProcess Error!\n");
		return FALSE;
	}
	//2. 计算DLL路径名字长度，并且要加上0结尾的长度
	dwLength = strlen(szDllPathName) + 1;
	//3. 在目标进程分配内存
	lpAllocAddr = VirtualAllocEx(hProcess, NULL, dwLength, MEM_COMMIT, PAGE_READWRITE);
	if (lpAllocAddr == NULL) {
		OutputDebugString(L"VirtualAllocEx Error!\n");
		CloseHandle(hProcess);
		return FALSE;
	}
	//4.拷贝DLL路径名字到目标进程的内存
	bRet = WriteProcessMemory(hProcess, lpAllocAddr, szDllPathName, dwLength, NULL);
	if (!bRet) {
		OutputDebugString(L"WriteProcessMemory Error!\n");
		CloseHandle(hProcess);
		return FALSE;
	}
	//5.因为Kernel32，大家都有，所以从自己进程这获取就行，所以这步是火区模块地址
	hModule = GetModuleHandle(L"kernel32.dll");
	if (!hModule) {
		OutputDebugString(L"GetModuleHandle Error!\n");
		CloseHandle(hProcess);
		return FALSE;
	}
	//6.获取LoadLibraryA函数地址
	dwLoadAddr = (DWORD)GetProcAddress(hModule, "LoadLibraryA");
	if (!dwLoadAddr) {
		OutputDebugString(L"GetProcAddress Error!\n");
		CloseHandle(hProcess);
		return FALSE;
	}
	//7. 创建远程线程，加载Dll
	hThread = CreateRemoteThread(hProcess, NULL,0, (LPTHREAD_START_ROUTINE)dwLoadAddr, lpAllocAddr, 0, NULL);
	if (!hThread) {
		OutputDebugString(L"CreateRemoteThread Error!\n");
		CloseHandle(hProcess);
		CloseHandle(hModule);
		return FALSE;
	}
	//8.关闭句柄
	CloseHandle(hProcess);
	return TRUE;
}



BOOL MyCreateRemoteThread(DWORD dwProcessID, DWORD dwProcAddr) {
	HANDLE hProcess;
	HANDLE hThread;
	DWORD dwThreadID;
	hProcess = 0;
	dwThreadID = 0;
	hThread = 0;
	//1.获取进程句柄
	hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwProcessID);
	if (hProcess == NULL) {
		OutputDebugString(L"OpenProcess Error!\n");
		return FALSE;
	}
	//2.创建远程线程
	hThread = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)dwProcAddr, NULL, 0, &dwThreadID);
	if (NULL == hThread) {
		OutputDebugString(L"CreateRemoteThread Error!\n");
		CloseHandle(hProcess);
		return FALSE;
	}
	//3.关闭
	CloseHandle(hThread);
	CloseHandle(hProcess);
	return true;

}

int main()
{
	LoadDll(16588, "D:\\yangruiqiDll.dll");//第一个参数是要注入dll的进程，第二个是dll目录
	return 0;
}



