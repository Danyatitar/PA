

#include "Polyphase_merge.h"
#include <ctime>
#define input_name "input.txt"
#define output_name "output.txt"
const int amount_of_numbers = 300000;															
const int amount_of_files = 5;																					
using namespace std;

/*�������� ������ ����� */
void show_output_files(polyphase& sorting)
{
	for (int i = 0; i < amount_of_files - 1; i++)
	{
		string file_name = "file_output_" + to_string(i) + ".txt";
		cout << file_name << ": ";
		sorting.show_binary_file_length(file_name);
	}
}

/* ������� ���������� */
void sort_function(filemanager& manager, polyphase& sorting, bool show_output, clock_t& start_dist, clock_t& end_dist, clock_t& start_merge, clock_t& end_merge)
{
	start_dist = clock();
	sorting.first_distribution(input_name, manager);
	end_dist = clock();
	if (show_output)
	{
		cout << "����� ������ (������ �������): " << endl;
		show_output_files(sorting);
	}
	/* �������� � �������� ������ */
	int max = manager.output[0].empty_series;
	for (int i = 1; i < amount_of_files - 1; i++)
	{
		if (max > manager.output[i].empty_series)
		{
			max = manager.output[i].empty_series;						//�������� ����������� �-�� ������� ����
		}
	}
	if (max != 0)														//���� 0 - �� ����������, ������ ����������
	{
		for (int i = 0; i < amount_of_files - 1; i++)
		{
			manager.output[i].empty_series -= max;
		}
		manager.input[0].empty_series += max;
	}
	manager.fileswap();
	cout << endl << "����������... ";
	start_merge = clock();
	sorting.merge(manager);
	end_merge = clock();
	cout << "��������" << endl;
	manager.fileswap();
}

/* ���� */
void view_debug_info(polyphase& sorting)
{
	cout << "���� " << input_name << " ������: " << endl;
	int quantity = sorting.show_binary_file_eof(input_name);
	cout << "�-�� ���������� �������:" << quantity << endl << endl;
	cout << "�������� ����: " << endl;
	quantity = sorting.amount_of_series(input_name);
	cout << "�-�� ����: " << quantity << endl << endl;
}

int main()
{
	setlocale(LC_ALL, "Russian");
	filemanager manager(1, amount_of_files - 1);
	polyphase sorting;
	clock_t start_generate, end_generate, start_dist, end_dist, start_merge, end_merge;
	start_generate = clock();
	sorting.generate(input_name, amount_of_numbers);														
	end_generate = clock();
	sort_function(manager, sorting, 0, start_dist, end_dist, start_merge, end_merge);						

	/* ����� ���������� ���������� � ����*/
	fstream f;
	f.open(output_name, ios::out);
	int length = manager.read(0);
	for (int i = 0; i < length; i++)
	{
		f << " " << manager.read(0);																				
	}
	f.close();
	cout << endl;

	/* ���� �� ����� */
	sorting.check_sort(output_name);
	float gen_t = ((double)end_generate - start_generate) / ((double)CLOCKS_PER_SEC);
	float merge_t = ((double)end_merge - start_merge) / ((double)CLOCKS_PER_SEC);
	cout << "�-�� ��������: " << amount_of_numbers << endl;
	cout << "�-�� �����: " << amount_of_files << endl;
	cout << "��� ���������: " << gen_t << " ���." << endl;
	cout << "��� ����������: " << merge_t << " ���." << endl << endl;

}
