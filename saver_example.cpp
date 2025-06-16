#define _CRT_SECURE_NO_WARNINGS

//#include<errno.h>
#include<stdlib.h>
#include<stdio.h>


#include"saver.h"


int main() {
	// пример сохранения ip:
	// проверка правильности ip не делается, в некоторых случаях ошибочных аргументов вызывается макро _abort(), объявленный в saver.h
	
	saver sa;
	
	char*str = "192.168.1.12\n255.255.255.255\n"; // пример требуемой строки
	int len_str = 29; // длина строки

	
	// метод ip_write записывает адреса из строки в файл, объявленный в saver.h как file_name_for_ip; данная перегрузка принимает количество ip адресов в строке и саму C-ую строку; возвращается количество записанных в файл символов
	unsigned int n = sa.ip_write(1, str);
	printf("%d\n", n);

	// следующая перегрузка принимает количество ip адресов в строке вместо длины строки
	n = sa.ip_write(str, len_str);
	printf("%d\n", n);
	
	
	char readed[256]{0};

	// ip_read(int num_ip, char* buffer) считывает последние num_ip адресов в buffer, возвращает количество успешно считанных ip адресов
	n = sa.ip_read(4, readed);

	printf(readed);
	printf("\n%d\n", n);
	
	//*/

	// пример сохранения id:

	saver* ga = new saver; // можно создать 2-ой объект, но это необязательно 

	unsigned int list[5], list2[5];
	for (int i = -1; i < 4; i++) list[i+1] = (unsigned int)i;
	// элемементы массива: 0xffffffff; 0; 1; 2; 3.

	// write_id принимает массив и количество id-ов в массиве
	ga->write_id(5, list);
	
	// read_id принимает количество id-ов и массив, куда их считывать. возвращает количество успешно считанных id-ов
	n = ga->read_id(5, list2);
	
	delete ga;
	//*/

	return(0);
}

