#include "stunipgetter.h"
const std::string GOOGLE_SERVER_ADDRESS = "74.125.250.129"; // гугл стун сервер
const int GOOGLE_SERVER_PORT = 19302; // порт для stun запрососв на этот сервер
const int TRANSACTION_ID_LEN = 12; // там все четко и регламентировано так что пусть будет так
const int MAIN_HEADER_LEN = 8;
const int BASE_STUN_ANSWER_LEN = 32;
const char MAIN_HEADER[] = "\x00\x01\x00\x00\0x21\0x12\0xA4\0x42"; // устойчивая чать запроса
// 0001 - тип запроса (обычный) 0000 - длина атрибутов (нету) 2112a442 - magic coockie константа которую трубет протокол


StunIpGetter::StunIpGetter():_udp(), _stunServerAddress(GOOGLE_SERVER_ADDRESS), _stunServerPort(GOOGLE_SERVER_PORT)
{
    _udp.start(0);
    _serverPort =_udp.getSocketPort();
}

StunIpGetter::StunIpGetter(int port):_udp(), _stunServerAddress(GOOGLE_SERVER_ADDRESS), _stunServerPort(GOOGLE_SERVER_PORT),_serverPort(port)
{
    _udp.start(_serverPort);
}

StunIpGetter::StunIpGetter(int port, std::string stunServerIp, int stunServerPort):_udp(), _stunServerAddress(stunServerIp), _stunServerPort(stunServerPort),_serverPort(port)
{
    _udp.start(_serverPort);
    if (port == 0){
        _stunServerPort = _udp.getSocketPort();
    }
}

std::string StunIpGetter::getStringIp()
{
    u_int8_t ip[4] = {0};
    if (getIp(ip)){
        std::string result = "";
        for (int i = 0; i< 4; i++){
            result.append(std::to_string(int(ip[i])));
            if (i != 3){
                result.append(".");
            }
        }
        return result;
    }
    else{
        std::cout << "StunIpGetter:cant get ip" << std::endl;
        return "";
    }

}

u_int32_t StunIpGetter::get32tIp()
{
    u_int8_t  ip[4] = {0};
    if (getIp(ip)){
        return parse32IpFromNums(ip[0],ip[1],ip[2],ip[3]);
    }
    else{
        std::cout << "StunIpGetter:cant get ip" << std::endl;
        return 0;
    }
}

u_int32_t StunIpGetter::parse32IpFromNums(u_int8_t first, u_int8_t second, u_int8_t third, u_int8_t fourth)
{
    u_int16_t one =    u_int16_t(  ((u_int16_t)(first << 8)) | ((u_int16_t)(second)));
    u_int16_t two =    u_int16_t(  ((u_int16_t)(third << 8)) | ((u_int16_t)(fourth)));
    u_int32_t result = u_int32_t(  ((u_int32_t)( one << 16)) | ((u_int32_t)(two)));
    return result;
}

bool StunIpGetter::getIp(u_int8_t * result)
{

    _udp.send(genStunResponse().data(), MAIN_HEADER_LEN + TRANSACTION_ID_LEN, _stunServerPort, _stunServerAddress);
    std::string cdata = _udp.listen();

    if (cdata.size() != BASE_STUN_ANSWER_LEN){
        std::cout << "StunIpGetter::cant parse ip. Bad answer. Her len - " << cdata.size() << "but normal len - " << BASE_STUN_ANSWER_LEN << std::endl;
        for (int i = 0; i < 4; i ++){
            result[i] = 0;
        }
        return false;

    }
    char * data = cdata.data();
    u_int8_t resultData[cdata.size()];
    for (int i = 0 ; i < cdata.size(); i ++ ){
        resultData[i] = (u_int8_t)(data[i]);
    }
    for (int i = 0; i < 4; i ++){
        result[i] = resultData[cdata.size() - (4-i)];
    }
    return true;

}

void StunIpGetter::generateTransactionId(int seed, char *result)
{
    srand(seed);
    for (int i = 0; i < 12; i++){
        result[i] = (char)(u_int8_t(rand())); // глупо можно поправить если очень хочется а так от 0-255. Я тут просто первые биты беру
    }
}

std::string StunIpGetter::genStunResponse()
{
    std::string hex(MAIN_HEADER,MAIN_HEADER_LEN); // message type len magic coockie
    char result[TRANSACTION_ID_LEN]={0};
    generateTransactionId(time(0), result); // тут seed - время. Не рекомендуют, но хрен с этим
    hex+=(std::string(result,TRANSACTION_ID_LEN));
    return hex;
}

