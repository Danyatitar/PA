#pragma once
#pragma once
#include <vector>
#include <string>
#include <fstream>
#include <iostream>
using namespace std;

/*Filemanager для бінарного файлу*/

class filemanager
{
private:
	struct file_definition								//Файл через структуру
	{
		fstream file_object;							//Файл
		string filename;								//Імя
		int real_series;								//К-ть справжніх серій
		int empty_series;								//К-ть порожніх серій
		file_definition(string _filename);				//Початкова ініціалізація
		void swap(file_definition& rhs);				//Свій свап
	};
public:
	vector <file_definition> input, output;				//Заводимо вектор пар обєкт-імя
	int in_count, out_count;							//лічильник для векторів
	filemanager(int in_count, int out_count);			//Конструктор
	void fileswap();								
	void index_swap(int index_in, int index_out);		
	int read(int index);								//Прочитати элемент з вектора
	void write(int index, int value);					//Записати элемент до вектора
	bool eof(int index);								//Перевірка на кінець файлу
	~filemanager();										//Деструктор
	int get_in();										//Повернути in лічильник
	int get_out();										//Повернути out лічильник
};