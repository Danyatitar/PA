#pragma once
#pragma once
#include "Filemanager.h"
#include "Fibonacci.h"
#include <string>
#include <ctime>
#include <iostream>
#include <fstream>
#include <map>
using namespace std;


class polyphase
{
public:
	vector <int> position_series;											//Вектор, який зберігає позиції "обривів" серій
	polyphase() {};
	void generate(string filename, int amount);			//Генератор файлу
	int show_binary_file_eof(string filename);								//Вывод бінарного файла (до кінця файлу)
	int show_binary_file_length(string filename);							//Вивід бінарного файлу (на основі довжини серій)
	void show_txt_file(string filename);									//Вивід текстового файлу
	int check_sort(string filename);										//Перевірка сортування
	void first_distribution(string filename, filemanager& manager);			//Перше розділення
	void merge(filemanager& manager);										//Сортування
	bool eof(string filename);				                                //Перевірка на порожність
	int amount_of_series(string filename);									//Повернення к-ті серій
};

bool polyphase::eof(string filename)
{
	fstream f;
	f.open(filename, ios::in | ios::binary | ios::ate);
	bool eof = !f.tellg();
	f.close();
	return eof;
}

void polyphase::merge(filemanager& manager)
{
	multimap <int, int> buffer;													//контейнер: <значення, номер файлу>
	int count = manager.get_in();												//К-ть файлів на вході
	int* length_of_series = new int[count];										//Масив довжин серій
	int sum_of_series = 2;														//Загальна сума серій
	int current_length;															//Поточна довжина

	while (sum_of_series > 1)
	{
		current_length = 0;
		for (int i = 0; i < count; i++)
		{
			if (manager.input[i].empty_series)									//Якщо є порожні серії
			{
				length_of_series[i] = 0;
				manager.input[i].empty_series--;
			}
			else
			{
				if (!manager.input[i].real_series)								//Якщо нема справжніх серій
				{
					manager.index_swap(i, 0);
					if (manager.input[i].empty_series)							//Дивимся наявність порожніх серій в новому файлі
					{
						i--;
						continue;												//Починаємо заново, ігноруючи подальші операції
					}
				}
				length_of_series[i] = manager.read(i);
				manager.input[i].real_series--;
				buffer.insert(pair <int, int>(manager.read(i), i));
			}
			current_length += length_of_series[i]--;
		}

		manager.write(0, current_length);
		manager.output[0].real_series++;

		auto buf = buffer.begin();							
		while (!buffer.empty())
		{
			buf = buffer.begin();
			manager.write(0, buf->first);
			if (length_of_series[buf->second]--)
			{
				buffer.insert(pair <int, int>(manager.read(buf->second), buf->second));
			}
			buffer.erase(buf);
		}

		/* Рахуємо суму серій */
		sum_of_series = 0;
		for (int i = 0; i < count; i++)
		{
			sum_of_series += manager.input[i].real_series + manager.input[i].empty_series;
		}
		sum_of_series += manager.output[0].real_series + manager.output[0].empty_series;
	}
}

void polyphase::first_distribution(string filename, filemanager& manager)
{
	fstream f;																	//Відкриваємо сгенерований файл
	f.open(filename, ios::in | ios::binary);
	int counter = manager.get_out();											//Забираємо число вихідних файлів
	fibonacci fib_vector(counter);												//Робимо ряд фібоначчі порядку counter
	fib_vector.make_order_vector();												//Робимо початковий ряд фібоначчі
	fib_vector.make_dist_mass();
	int current = 0;															//Поточна позиція
	int next = 0;																//позиція після поточної
	int current_file = 0;														//Поточний файл
	int length = 0;																//Довжина серії
	streampos start_pos = manager.output[current_file].file_object.tellg();		//Початкова позиція(тут 0)
	streampos end_pos;															//Кінцева позиція
	manager.output[current_file].real_series++;									//Збільшуємо серію на 1 (т.к. начинаем)
	manager.write(current_file, 0);												//Резервуємо місце під запис серій
	f.read((char*)&next, sizeof(int));

	int* diff = new int[counter];												//Різниця між поточним та розподіленням фібоначчі
	diff[0] = 0;
	for (int i = 1; i < counter; i++)
	{
		diff[i] = 1;															//[0 1 1 1 1 1 1 1 ...]
	}
	bool flag = 0;																//Прапор виходу
	while (true)
	{
		/* запись серии */
		while (current <= next)
		{
			manager.write(current_file, next);									//Пишемо в поточний файл обраний елемент
			length++;															//Збільшуємо довжину серії
			current = next;														
			if (!f.eof())
			{
				//f >> next;													//Якщо файл не скінчився беремо наступний
				f.read((char*)&next, sizeof(int));
			}
			else
			{
				flag = !flag;													//Виходимо з циклів
				break;
			}
		}
		if (flag == 1) break;

		/* Змінюємо файли */
		end_pos = manager.output[current_file].file_object.tellg();				//Запамятовуємо поточну(кінццеву позицію в файлі)
		manager.output[current_file].file_object.seekg(start_pos);				//Переходимо на початок файлу
		manager.write(current_file, length);									//Записуємо довжину замість 0
		manager.output[current_file].file_object.seekg(end_pos);				//Повертаємося на збережену позицію 

		/* Для розподілу */
		if ((current_file < counter - 1) && ((diff[current_file] + 1) == diff[current_file + 1]))
		{
			current_file++;
			diff[current_file]--;
			manager.output[current_file].real_series++;
		}
		else
		{
			if (!diff[counter - 1])
			{
				fib_vector.make_order_vector();
				fib_vector.make_dist_mass();
				for (int i = 0; i < counter; i++)
				{
					diff[i] = fib_vector.mass[i] - manager.output[i].real_series;
				}
			}
			current_file = 0;
			diff[current_file]--;
			manager.output[current_file].real_series++;
		}

		/* Зберігаємо місце для запису довжини серій */
		start_pos = manager.output[current_file].file_object.tellg();			//Робимо стартову позицію поточною
		manager.write(current_file, 0);											//Резервуємо місце під довжину серій
		length = 0;																//Обнуляем длину серии

		manager.write(current_file, next);										//Пишемо елемент з наступної серії
		length++;
		current = next;
		if (!f.eof())															//Якщо файл не закінчився зчитуємо наступний елемент
		{
			f.read((char*)&next, sizeof(int));
		}
		else
		{
			break;
		}
	}
	end_pos = manager.output[current_file].file_object.tellg();				
	manager.output[current_file].file_object.seekg(start_pos);				
	manager.write(current_file, length);									
	manager.output[current_file].file_object.seekg(end_pos);			
	for (int i = 0; i < counter; i++)
	{
		manager.output[i].empty_series = diff[i];							//Пишемо к-ть порожніх серій в масив
	}
	f.close();
}

 int polyphase::show_binary_file_length(string filename)
{
	if (eof(filename))
	{
		cout << "[пустой]" << endl;
	}
	else
	{
		fstream f;
		f.open(filename, ios::in | ios::binary);		
		int temp;
		int length;
		while (!f.eof())
		{
			f.read((char*)&length, sizeof(int));
			if (length)
			{
				cout << "[len: " << length << "] ";
				while (length)
				{
					int temp;
					f.read((char*)&temp, sizeof(int));
					cout << temp << " ";
					length--;
				}
			}
		}
		cout << endl;
		f.close();
	}
	return 0;
}

 void polyphase::generate(string filename, int amount)
{
	fstream f;
	srand(time(NULL));
	f.open(filename, ios::out | ios::binary);
	for (int i = 0; i < amount - 1; i++)
	{
		int value = rand() % (i+1);
		f.write((char*)&value, sizeof(int));
	}
	f.close();
}

 int polyphase::check_sort(string filename)
{
	bool flag = 1;
	int counter = 1;
	int temp1, temp2;
	fstream f;
	f.open(filename, ios::in);
	f >> temp1;
	while (!f.eof())
	{
		f >> temp2;
		counter++;
		if (temp1 > temp2)
		{
			cout << "Помилка сортування: " << counter << " позиція";
			flag = 0;
		}
		temp1 = temp2;
	}
	if (flag = true)
	{
		cout << "Сортування правильне: " << counter << " Елементів було відсортовано";
	}
	cout << endl;
	return flag;
}

 int polyphase::show_binary_file_eof(string filename)
{
	fstream f;
	f.open(filename, ios::in | ios::binary);		
	int temp;
	int quantity = 0;
	while (!f.eof())
	{
		int temp;
		f.read((char*)&temp, sizeof(int));
		cout << temp << " ";
		quantity++;
	}
	cout << endl;
	f.close();
	return quantity;							
}

 void polyphase::show_txt_file(string filename)
{
	fstream f;
	f.open(filename);
	int temp;
	while (!f.eof())
	{
		f >> temp;
		cout << temp << " ";
	}
	cout << endl;
	f.close();
}
 int polyphase::amount_of_series(string filename)
 {
	 fstream f;
	 f.open(filename, ios::in | ios::binary);
	 int quantity = 1;
	 int current = 0;
	 int next = 0;
	 position_series.push_back(f.tellg());
	 cout << "Текущая позиция: " << 1 + f.tellg() << " :: ";
	 f.read((char*)&current, sizeof(int));
	 cout << "Серия №" << quantity << " начинается со значения: " << current << endl;
	 f.read((char*)&next, sizeof(int));
	 while (!f.eof())
	 {
		 /*если всё верно*/
		 if (next > current)
		 {
			 f.read((char*)&current, sizeof(int));
		 }
		 else
		 {
			 quantity++;
			 cout << "Текущая позиция: " << f.tellg() / sizeof(int) << " :: ";
			 position_series.push_back(f.tellg());
			 cout << "Серия №" << quantity << " начинается со значения: " << next << endl;
			 f.read((char*)&current, sizeof(int));
		 }
		 if (current > next)
		 {
			 f.read((char*)&next, sizeof(int));
		 }
		 else
		 {
			 quantity++;
			 cout << "Текущая позиция: " << f.tellg() / sizeof(int) << " :: ";
			 position_series.push_back(f.tellg());
			 cout << "Серия №" << quantity << " начинается со значения: " << current << endl;
			 f.read((char*)&next, sizeof(int));
		 }
	 }
	 f.close();
	 return quantity;
 }





