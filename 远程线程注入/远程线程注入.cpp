// Զ���߳�ע��.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include<windows.h>
BOOL LoadDll(DWORD dwProcessID, char *szDllPathName) {//��һ����������ID������dll������·��
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
	//1.��ȡ���̾��
	hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwProcessID);
	if (hProcess == NULL) {
		OutputDebugString(L"OpenProcess Error!\n");
		return FALSE;
	}
	//2. ����DLL·�����ֳ��ȣ�����Ҫ����0��β�ĳ���
	dwLength = strlen(szDllPathName) + 1;
	//3. ��Ŀ����̷����ڴ�
	lpAllocAddr = VirtualAllocEx(hProcess, NULL, dwLength, MEM_COMMIT, PAGE_READWRITE);
	if (lpAllocAddr == NULL) {
		OutputDebugString(L"VirtualAllocEx Error!\n");
		CloseHandle(hProcess);
		return FALSE;
	}
	//4.����DLL·�����ֵ�Ŀ����̵��ڴ�
	bRet = WriteProcessMemory(hProcess, lpAllocAddr, szDllPathName, dwLength, NULL);
	if (!bRet) {
		OutputDebugString(L"WriteProcessMemory Error!\n");
		CloseHandle(hProcess);
		return FALSE;
	}
	//5.��ΪKernel32����Ҷ��У����Դ��Լ��������ȡ���У������ⲽ�ǻ���ģ���ַ
	hModule = GetModuleHandle(L"kernel32.dll");
	if (!hModule) {
		OutputDebugString(L"GetModuleHandle Error!\n");
		CloseHandle(hProcess);
		return FALSE;
	}
	//6.��ȡLoadLibraryA������ַ
	dwLoadAddr = (DWORD)GetProcAddress(hModule, "LoadLibraryA");
	if (!dwLoadAddr) {
		OutputDebugString(L"GetProcAddress Error!\n");
		CloseHandle(hProcess);
		return FALSE;
	}
	//7. ����Զ���̣߳�����Dll
	hThread = CreateRemoteThread(hProcess, NULL,0, (LPTHREAD_START_ROUTINE)dwLoadAddr, lpAllocAddr, 0, NULL);
	if (!hThread) {
		OutputDebugString(L"CreateRemoteThread Error!\n");
		CloseHandle(hProcess);
		CloseHandle(hModule);
		return FALSE;
	}
	//8.�رվ��
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
	//1.��ȡ���̾��
	hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwProcessID);
	if (hProcess == NULL) {
		OutputDebugString(L"OpenProcess Error!\n");
		return FALSE;
	}
	//2.����Զ���߳�
	hThread = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)dwProcAddr, NULL, 0, &dwThreadID);
	if (NULL == hThread) {
		OutputDebugString(L"CreateRemoteThread Error!\n");
		CloseHandle(hProcess);
		return FALSE;
	}
	//3.�ر�
	CloseHandle(hThread);
	CloseHandle(hProcess);
	return true;

}

int main()
{
	LoadDll(16588, "D:\\yangruiqiDll.dll");//��һ��������Ҫע��dll�Ľ��̣��ڶ�����dllĿ¼
	return 0;
}



