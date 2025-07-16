#define _CRT_SECURE_NO_WARNINGS

//#include<errno.h>
#include<stdlib.h>
#include<stdio.h>


#include"saver.h"


int main() {

    Saver * save = Saver::getSaver();
    Host hosts[save->countHostInFile()];
    save->readHostsFromFile(hosts,save->countHostInFile());
    for (int i = 0; i < save->countHostInFile(); i++){
        std::cout << hosts[i].toString() << std::endl;
    }
	return(0);
}

