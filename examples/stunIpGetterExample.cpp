#include "stunipgetter.h"
// пример получения ip адреса с stun сервером гугла
int main(){
    StunIpGetter ipGetter;
    std::cout <<  ipGetter.getStringIp() << " = " << ipGetter.get32tIp() << std::endl;
    return 0;
    // все пряи просто :)
};

