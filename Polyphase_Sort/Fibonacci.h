#pragma once
#pragma once
#include <vector>
#include <iostream>
using namespace std;

/* ���� Գ�������*/

class fibonacci
{
private:
	vector <int> fib_series;								//������ ��� ��������� ���� ��������� ����-����� ������
	int current_pos;										//������� ������� ��� ���������
	int index_end;											//������-������� �������
public:
	int* mass;
	int order;												//������� ��� ���� Գ�������
	fibonacci(int _order);									//�����������
	void show_fib_vector();									//������� ���������
	void show_dist_mass();									//������� �������� �����
	void make_order_vector();								//������� ����������� ��������� �������� �������
	void make_dist_mass();									//������� ����������� �����(�� ����� order-vector)
};