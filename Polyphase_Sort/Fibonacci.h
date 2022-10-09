#pragma once
#pragma once
#include <vector>
#include <iostream>
using namespace std;

/* Клас Фібоначчі*/

class fibonacci
{
private:
	vector <int> fib_series;								//Вектор для генерації ряду фібоначчі будь-якого порядк
	int current_pos;										//Поточна позиція для підрахунку
	int index_end;											//індекс-позиція вектора
public:
	int* mass;
	int order;												//Порядок для ряду Фібоначчі
	fibonacci(int _order);									//Конструктор
	void show_fib_vector();									//Вивести фібоначчі
	void show_dist_mass();									//Вивести основний масив
	void make_order_vector();								//Зробити послідовність фібоначчі заданого порядку
	void make_dist_mass();									//Зробити розподільчий масив(на основі order-vector)
};