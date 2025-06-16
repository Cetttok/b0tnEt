#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#undef _CRT_SECURE_NO_WARNINGS
#include<stdlib.h>

#define file_name_for_ip "ipaddr.txt"
#define file_for_id_name "idescommands.txt"
#define max_ip_len 15
#define len_id 8
#define _abort() {printf("crash this programm");abort();}


extern FILE* file_for_ip;
extern FILE* id_file;
extern int num_savers;

class saver {
public:
	saver();
	~saver();

	// num_ip - количество ip адресов
	// возвращает количество успешно записанных символов
	unsigned int ip_write(int num_ip, const char* buffer);

	// str_len - длина строки содержащей ip адреса
	// возвращает количество успешно записанных символов
	unsigned int ip_write(const char* buffer, int str_len);

	// num_ip - количество ip адресов
	// длина buffer должна быть >= (max_ip_len+1)*num_ip+1
	// возвращает количество успешно считанных ip адресов
	unsigned int ip_read(int num_ip, char* buffer);


	// num_id - количество id-OB
	void write_id(int num_id, unsigned int* list);

	// num_id - количество id-OB
	// возвращает количество успешно считанных id
	unsigned int read_id(int num_id, unsigned int* list);
};

