#include <iostream>
#include <Windows.h>
#include<thread>
#include <mutex>
#include<chrono>
using std::cin;
using std::cout;
using std::endl;
using namespace std::chrono_literals;

bool finish = false;
std::mutex mtx;
HANDLE ghMutex = NULL;

VOID Function()
{
	while (!finish)
	{
		cout << "Hello Threads" << GetCurrentThreadId() << endl;
		system("PAUSE");
	}
}
struct Point
{
	Point(int x,int y):x(x),y(y){}
	int x;
	int y;
};
VOID Collision(Point* point)
{
	while (point->x != point->y)
	{
		cout << "X= " << point->x++ << "\tY = " << point->y-- << endl;
		Sleep(10);
	}
}
VOID Decrement(int i)
{
	while (i)cout << i-- << "\t";
}
void Plus()
{
	while (!finish)
	{
		//mtx.lock();
		WaitForSingleObject(ghMutex, INFINITE);
		cout << "+";
		Sleep(100);
		ReleaseMutex(ghMutex);
		//std::this_thread::sleep_for(100ms);
		//mtx.unlock();
	}
	//std::this_thread::
}
void Minus()
{
	while (!finish)
	{
		//mtx.lock();
		WaitForSingleObject(ghMutex, INFINITE);
		cout << "-";
		Sleep(100);
		ReleaseMutex(ghMutex);
		//std::this_thread::sleep_for(100ms);
		//mtx.unlock();
	}
}
//#define WINDOWS_THREADS_1
//#define WINDOWS_THREADS_2
//#define CPP_THREADS

void main()
{
	setlocale(LC_ALL, "");
#ifdef WINDOWS_THREADS_1
	DWORD dwID = 0;
	HANDLE hThread = CreateThread
	(
		NULL,
		0,
		(LPTHREAD_START_ROUTINE)Function,
		NULL,
		NULL,
		&dwID
	);
	cin.get();
	finish = true;
	cout << "ThreadID from main()" << dwID << endl;
	WaitForSingleObject(hThread, INFINITE);
#endif // WINDOWS_THREADS_1

#ifdef WINDOWS_THREADS_2
	Point A(0, 10000);
	int i = 10000;
	DWORD dwThreadID = 0;
	HANDLE hThread = CreateThread
	(
		NULL,
		NULL,
		(LPTHREAD_START_ROUTINE)Decrement,
		(LPVOID)i,
		NULL,
		&dwThreadID
	);
	WaitForSingleObject(hThread, INFINITE);
#endif // WINDOWS_THREADS_2

#ifdef CPP_THREADS
	//Plus();
//Minus();

	std::thread plus_thread = std::thread(Plus);
	std::thread minus_thread = std::thread(Minus);
	cout << "start\n";
	cin.get();
	finish = true;
	cout << "\nfinish";

	if (plus_thread.joinable())plus_thread.join();
	if (minus_thread.joinable())minus_thread.join();
#endif // CPP_THREADS
	ghMutex = CreateMutex(NULL, FALSE, NULL);
	HANDLE hThreads[2] = {};
	hThreads[0] = CreateThread
	(
		NULL,
		NULL,
		(LPTHREAD_START_ROUTINE)Plus,
		NULL,
		NULL,
		0
	);
	hThreads[1] = CreateThread
	(
		NULL,
		NULL,
		(LPTHREAD_START_ROUTINE)Minus,
		NULL,
		NULL,
		0
	);
	WaitForMultipleObjects(2, hThreads, TRUE, INFINITE);
}