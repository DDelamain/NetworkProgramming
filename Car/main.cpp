#include <Windows.h>
#include <iostream>
#include <conio.h>
#include <chrono>
#include<thread>
using std::cin;
using std::cout;
using std::endl;
using namespace std::literals;

#define Escape	27
#define Enter	13

#define MIN_TANK_CAPACITY 20
#define MAX_TANK_CAPACITY 120
class Tank
{
	const int CAPACITY;
	double fuel_level;
public:
	Tank(int capacity):
		CAPACITY(
			capacity < MIN_TANK_CAPACITY ? MIN_TANK_CAPACITY:
			capacity > MAX_TANK_CAPACITY ? MAX_TANK_CAPACITY:
			capacity
		)
	{
		//this->CAPACITY = capacity;
		this->fuel_level = 0;
		cout << "Tank is ready " << this << endl;
	}
	~Tank()
	{
		cout << "Tank is over " << this << endl;
	}
	double get_fuel_level()const
	{
		return fuel_level;
	}
	void fill(double amount)
	{
		if (amount < 0) return;
		fuel_level += amount;
		if (fuel_level > CAPACITY)fuel_level = CAPACITY;
	}
	double give_fuel(double amount)
	{
		if (amount < 0)return fuel_level;
		fuel_level -= amount;
		if (fuel_level < 0) fuel_level = 0;
		return fuel_level;
	}

	void info()const
	{
		cout << "Capacity:\t" << CAPACITY << " liters.\n";
		cout << "Fuel level: " << fuel_level << " liters.\n";
	}
};
#define MIN_ENGINE_CONSUPTION	4
#define MAX_ENGINE_CONSUPTION	30
class Engine
{
	const double CONSUMPTION;
	double consumption_per_second;
	bool is_started;
public:
	Engine(double consuption) :CONSUMPTION
	(
		consuption < MIN_ENGINE_CONSUPTION ? MIN_ENGINE_CONSUPTION :
		consuption > MAX_ENGINE_CONSUPTION ? MAX_ENGINE_CONSUPTION :
		consuption
	)
	{
		consumption_per_second = CONSUMPTION * 3e-5;
		is_started = false;
		cout << "Engine is ready:\t" << this << endl;
	}
	~Engine()
	{
		cout << "Engine is over:\t" << this << endl;
	}
	double get_consumption_per_second()
	{
		return consumption_per_second;
	}
	void start()
	{
		is_started = true;
	}
	void stop()
	{
		is_started = false;
	}
	bool started()const
	{
		return is_started;
	}
	void info()const
	{
		cout << "Consumption:\t\t" << CONSUMPTION << " liters/km.\n";
		cout << "Consuption per sec: " << consumption_per_second << " liters/sec.\n";
	}
};
class Car
{
	Engine engine;
	Tank tank;
	bool driver_inside;
	double speed;
	bool breaks_is_on;
	struct CarThreads
	{
		std::thread panel_thread;
		std::thread engine_idle_thread;
		std::thread friction_thread;
	}car_threads;
public:
	Car(double consumtion, int capacity = 50) : engine(consumtion), tank(capacity), speed(0)
	{
		driver_inside = false;
		breaks_is_on = false;
		cout << "Your car is ready to go, press Enter to get in " << this << endl;
	}
	~Car()
	{
		cout << "Car is over: " << this << endl;
	}
	void get_in()
	{
		driver_inside = true;
		//panel();
		if (!car_threads.panel_thread.joinable())
			car_threads.panel_thread = std::thread(&Car::panel, this);
	}
	void get_out()
	{
		driver_inside = false;
		if (!car_threads.panel_thread.joinable())
			car_threads.panel_thread.join();
		system("CLS");
		cout << "You are out of the car" << endl;
	}
	void startup()
	{
		if (tank.give_fuel(0))
		{
			engine.start();
			if (!car_threads.engine_idle_thread.joinable())
				car_threads.engine_idle_thread = std::thread(&Car::engine_idle, this);
			if (!car_threads.friction_thread.joinable())
				car_threads.friction_thread = std::thread(&Car::friction, this);
		}
	}
	void shutdown()
	{
		engine.stop();
		if (car_threads.engine_idle_thread.joinable())
			car_threads.engine_idle_thread.join();
	}
	double get_current_consumption()
	{
		if (speed >= 1 && speed <= 60)
			return 0.0020;

		if (speed >= 61 && speed <= 100)
			return 0.0014;

		if (speed >= 101 && speed <= 140)
			return 0.0020;

		if (speed >= 141 && speed <= 200)
			return 0.0025;

		if (speed >= 201 && speed <= 250)
			return 0.0030;

		return engine.get_consumption_per_second();
	}
	void control()
	{
		char key;
		do
		{
			key = 0;
			if(_kbhit())
				key = _getch();
			switch (key)
			{
			case Enter:
				if (driver_inside)get_out();
				else get_in();
				break;
			case 'F':
			case 'f':
				if (!driver_inside && !engine.started())
				{
					double amount;
					cout << "Введите объем топлива: "; cin >> amount;
					tank.fill(amount);
				}
				else cout << "Нужно заглушить двигатель и выйти из машины, у нас только самообслуживание" << endl;
				break;
			case 'I':
			case 'i':
				if (driver_inside && !engine.started())startup();
				else if (driver_inside)shutdown();
				break;
			case 'W':
			case 'w':
				if (driver_inside && engine.started())
				{
					breaks_is_on = false;
					speed += 10;
					if (speed > 250) speed = 250;
				}
				break;
			case 'S':
			case 's':
				if (driver_inside && engine.started())
				{
					breaks_is_on = true;
					speed -= 10;
					if (speed < 0) speed = 0;
				}
				break;
			case Escape:
				shutdown();
				get_out();
			}
			if (tank.get_fuel_level() == 0 && engine.started())shutdown();
		} while (key != Escape);
	}
	void friction()
	{
		while (driver_inside)
		{
			if (speed > 0)
			{
				speed--;
				if (engine.started())
					tank.give_fuel(get_current_consumption());
			}
			std::this_thread::sleep_for(1s);
		}
	}
	void engine_idle()
	{
		while (engine.started() && tank.give_fuel(get_current_consumption()))
			std::this_thread::sleep_for(1s);
	}
	void panel()
	{
		HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
		while (driver_inside)
		{
			system("CLS");
			cout << "Fuel level: " << tank.get_fuel_level() << " liters.\t";
			if (tank.get_fuel_level() < 5)
			{
				SetConsoleTextAttribute(hConsole, 0x4F);
				cout << " LOW FUEL ";
				SetConsoleTextAttribute(hConsole, 0x07);
			}
			cout << "\nSpeed: " << speed << " km/h\n";
			cout << endl;
			cout << "\nConsumption:  " << get_current_consumption() << endl;
			cout << "Engine is " << (engine.started() ? "started" : "stoped") << endl;
			std::this_thread::sleep_for(100ms);
		}
	}
};
//#define TANK_CHECK
//#define ENGINE_CHECK
void main()
{
	setlocale(LC_ALL, "");
#ifdef TANK_CHECK
	Tank tank(40);
	int amount;
	while (true)
	{
		cout << "Введите объем топлива: "; cin >> amount;
		tank.fill(amount);
		tank.info();
	}
#endif // TANK_CHECK
#ifdef ENGINE_CHECK
	Engine engine(10);
	engine.info();
#endif // ENGINE_CHECK
	Car bmw(10, 70);
	bmw.control();
}