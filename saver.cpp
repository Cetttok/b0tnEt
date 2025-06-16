#include "saver.h"


FILE* file_for_ip=0;
FILE* id_file=0;
int num_savers=0;

saver::saver() {
	num_savers++;
	if (0 == file_for_ip) {
        file_for_ip=fopen(file_name_for_ip, "ab+");
		if (0 == file_for_ip) {
			_abort();
		}
	}
	if (0 == id_file) {
		id_file=fopen(file_for_id_name, "ab+");
		if (0 == id_file) {
			_abort();
		}
	}
}

saver::~saver() {
	num_savers--;
	if (num_savers==0) {
		if (file_for_ip) {
			fclose(file_for_ip);
			file_for_ip = 0;
		}
		if (id_file) {
			fclose(id_file);
			id_file = 0;
		}
	}
}

unsigned int saver::ip_write(int n, const char* b) {
	char*b2;
	unsigned int n_res;
	int len2 = (max_ip_len + 2)*n, i2 = -1, i = 1, iter = 0, d = 0, j, nd = 0;
	if (n < 1)_abort();
	if (b2 = (char*)malloc(len2)) {
		while (d < n) {
			if (i >= len2)_abort();
			if ((b[i] == '.') || (b[i] == '\n')) {
				for (j = 4 - i + i2; j > 0; j--) {
					b2[iter] = '0';
					iter++;
				}
				for (j = 1; j < i - i2; j++) {
					b2[iter] = b[i2 + j];
					iter++;
				}
				if (b[i] == '\n') {
					b2[iter] = '\r';
					d++;
					iter++;
				}
				else nd++;
				b2[iter] = b[i];
				iter++;
				i2 = i;
				i++;
			}
			i++;
		}
		n_res = (unsigned int)fwrite(b2, 1, len2, file_for_ip);
		free(b2);
		if (nd % 3 || nd / 3 != n)_abort();
		return(n_res);
	}
	else _abort();
	return 0;
}

unsigned int saver::ip_write(const char* b, int l) {
	int n = 0;
	for (int i = 7; i < l; i++) {
		if (b[i] == '\n') {
			n++;
			i = i + 7;
		}
		else if (b[i] == 0)break;
	}
	return(ip_write(n, b));
}

unsigned int saver::ip_read(int num_ip, char* buffer) {
	if (num_ip < 1)_abort();
	int len2 = (max_ip_len + 2)*num_ip;
	char* b2 = (char*)malloc(len2 + 1);
	fseek(file_for_ip, 0, SEEK_END);
	if (ftell(file_for_ip) >= len2) fseek(file_for_ip, -len2, SEEK_END);
	else fseek(file_for_ip, 0, SEEK_SET);
	unsigned int res = (unsigned int)fread(b2, 1, len2, file_for_ip),
		i = 0,
		j = 0,
		n = 0;
	for (i = 0; i < res; i++) {
		if (b2[i] == '\r') {
			i++;
			n++;
		}
		if ((buffer[j] = b2[i]) == '.') {
			if (b2[i + 1] == '0')i++;
			if (b2[i + 1] == '0')i++;
		}
		j++;
	}
	buffer[j] = 0;
	free(b2);
	return(n);
}

void saver::write_id(int n, unsigned int* list) {
	if (n < 1)_abort();
	fseek(id_file, 0, SEEK_END);
	for (int i = 0; i < n; i++)
		if (EOF == fprintf(id_file, "%08X\r\n", list[i]))_abort();
	fflush(id_file);
}

unsigned int saver::read_id(int n, unsigned int* list) {
	if (n < 1)_abort();
	char* b2 = (char*)malloc((len_id + 2)*n);
	int len2 = (len_id + 2)*n;
	fseek(id_file, -len2, SEEK_END);
	unsigned int res = (unsigned int)fread(b2, 1, len2, id_file), i;
	for (i = 0; i < res / (len_id + 2); i++) {
		if (!sscanf(&b2[i*(len_id + 2)], "%8X", &list[i]))
			_abort();
	}
	free(b2);
	return(i);
}
