#include <Windows.h>
#include <process.h>
#include <sstream>

HANDLE hThread1;
HANDLE hThread2;

// Deadlock happens in ThreadOneTask and ThreadTwoTask. It doesn't happen every time, but it should be reproducable by running the application for 5 times.

unsigned __stdcall ThreadTwoTask(void*)
{
	// Deadlock happens before this line.
	_configthreadlocale(_ENABLE_PER_THREAD_LOCALE);
	printf("Thread2 started");

	return 0;
}

unsigned __stdcall ThreadOneTask(void*)
{
	_configthreadlocale(_ENABLE_PER_THREAD_LOCALE);
	hThread2 = (HANDLE)_beginthreadex(
		0,
		0,
		&ThreadTwoTask,
		NULL,
		0,
		0);

	std::wistringstream wis(L"00A7");
	std::char_traits<wchar_t>::int_type c;
	wis >> std::hex >> c; // Deadlock happens this line.
	
	return 0;
}

void main()
{
	// If uncomment this line, deadlock will not happen.
	// It is not clearly documented on https://msdn.microsoft.com/en-us/library/26c0tb7x.aspx that multiple-thread applications should always enable per-thread locale.
	// But we are going to fix search projects by doing this.
	//_configthreadlocale(_ENABLE_PER_THREAD_LOCALE);
	
	setlocale(LC_ALL, ""); // If comment this line, deadlock will not happen.
	hThread1 = (HANDLE)_beginthreadex(
		0,
		0,
		&ThreadOneTask,
		NULL,
		0,
		0);

	if (hThread1)
	{
		WaitForSingleObject(hThread1, INFINITE);
		CloseHandle(hThread1);
	}

	if (hThread2)
	{
		WaitForSingleObject(hThread2, INFINITE);
		CloseHandle(hThread2);
	}
}