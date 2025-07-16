#include "saver.h"


const int MAX_IP_ADDRES_LEN = 23;
const int MAX_COMMAND_ID_LEN = 10; // установил случайно
const int MIN_HOST_SIZE = 9;
const char IP_PORT_DELIMER = ':';

const char * FILE_FOR_IP_NAME = "ipaddr.txt";
const char * FILE_FOR_COMMAND_ID_NAME =  "idcommands.txt";

Saver * Saver::_saver = nullptr;
Saver::Saver() {
     _countHostInFile = recountHostsInFile();
     _countCommandIdInFile = recountCommandIdInFile();
}

Saver::~Saver() {

}

void Saver::rewriteFile(std::string fileName, std::string data)
{
    FILE * _file = fopen(fileName.c_str(), "w");// плохо на запись чтение открывается
    if (nullptr == _file) {
        std::cout << "Saver. cant write to file " <<fileName << std::endl;
    }
    fprintf(_file,"%s", data.c_str());
    fclose(_file);
}

void Saver::createFile(std::string fileName)
{
    FILE * file = fopen(fileName.c_str(), "w");
    fclose(file);
}


int Saver::recountHostsInFile()
{
    int countIp = 0;

    FILE * file = fopen(FILE_FOR_IP_NAME, "r+");
    if (file == nullptr){
        createFile(FILE_FOR_IP_NAME);
        return 0;
    }
    char lineBuffer[MAX_IP_ADDRES_LEN]; // Буфер для хранения строки
    while(fgets(lineBuffer, MAX_IP_ADDRES_LEN, file)!=nullptr) {
        std::string line(lineBuffer);
        if (line != "" && line.size() >= 9){
            countIp++;
        }
        else{
            break;
        }
    }
    fclose(file);
    return countIp;

}
int Saver::recountCommandIdInFile()
{
    int countId = 0;
    FILE * file = fopen(FILE_FOR_COMMAND_ID_NAME, "r+");
    if (file == nullptr){
        createFile(FILE_FOR_COMMAND_ID_NAME);
        return 0;
    }
    char lineBuffer[MAX_COMMAND_ID_LEN]; // Буфер для хранения строки
    while(fgets(lineBuffer, MAX_COMMAND_ID_LEN, file)!=nullptr) {
        std::string line(lineBuffer);
        if (line != ""){
            countId++;
        }
        else{
            break;
        }
    }
    fclose(file);
    return countId;

}



Saver * Saver::getSaver()
{
    if (_saver == nullptr){
        _saver = new Saver(); //суть паттерна одиночка может быть только один обьект
    }
    return _saver;

}

int Saver::countCommandIdInFile() const
{
    return _countCommandIdInFile;
}
//стандартный гетер
int Saver::countHostInFile() const
{
    return _countHostInFile;
}



void Saver::writeHostsToFile(Host *list, int size)
{
    std::string dataToFile = "";
    for (int i = 0; i < size; i++){
        dataToFile+= list[i].toString() + "\n";
    }
    rewriteFile(FILE_FOR_IP_NAME, dataToFile);
    _countHostInFile = size;
}

int Saver::readHostsFromFile(Host *result, int size)
{
    if (size != _countHostInFile){
        std::cout << "Saver:Warning! invalid ip count" << std::endl;
        size = _countHostInFile; // нужно ли это????
    }
    FILE * file = fopen(FILE_FOR_IP_NAME, "r");

    char lineBuffer[MAX_IP_ADDRES_LEN]; // Буфер для хранения строки
    int lineCount = 0;
    for(int lineCount = 0; lineCount < size; lineCount++) {
        if(fgets(lineBuffer, MAX_IP_ADDRES_LEN, file)==nullptr){
            std::cout << "Saver:Warning! file is end, but count not done";
            break;
        }
        std::string line(lineBuffer);
        if (line != "" && line.size() >= MIN_HOST_SIZE){ //последняя строка пустая
            result[lineCount] = Host(line); // Host сам парсит себя
        }
        else{
            break;
        }
    }
    fclose(file);
    return lineCount;

}

void Saver::writeCommandsIdToFile(int *list, int size)
{

    std::string dataToFile = "";
    for (int i = 0 ; i < size; i ++){
        dataToFile += std::to_string(list[i]) +  "\n";
    }
    rewriteFile(FILE_FOR_COMMAND_ID_NAME, dataToFile);
}

int Saver::readCommandsIdFromFile(int *result, int size)
{
    if (size != _countCommandIdInFile){
        std::cout << "Saver:Warning! invalid command id count" << std::endl;
        size = _countCommandIdInFile;
    }
    FILE * file = fopen(FILE_FOR_COMMAND_ID_NAME, "r");

    char lineBuffer[MAX_COMMAND_ID_LEN]; // Буфер для хранения строки
    int lineCount = 0;
    for(int lineCount = 0; lineCount < size; lineCount++) {
        if(fgets(lineBuffer, MAX_COMMAND_ID_LEN, file)==nullptr){
            std::cout << "Saver:Warning! file is end, but count not done";
            break;
        }
        std::string line(lineBuffer);
        if (line != "" ){ //последняя строка пустая
            try {
                result[lineCount] = std::stoi(line);
            } catch (std::invalid_argument) {
                std::cout << "Saver. cant convert to int. badId '" << line << "'" << std::endl;
                break;
            }
        }
        else{
            break;
        }
    }
    fclose(file);
    return lineCount;
}

Host::Host(std::string ip, int port):_ip(ip), _port(port)
{// зачем этот конструктор
    //существует
}

Host::Host(std::string parseFrom)
{
    try{
        if (parseFrom.find("\n")!=parseFrom.npos){
            parseFrom = parseFrom.substr(0,parseFrom.find("\n"));
        }
        int positionOfDelimer  = parseFrom.find(IP_PORT_DELIMER);
        if (positionOfDelimer == parseFrom.npos){
            std::cout << "Host. No delimer in string ("<<IP_PORT_DELIMER<<")" << " from '" << parseFrom <<"'"<< std::endl;
            return;
        }

        _port = std::stoi(parseFrom.substr(positionOfDelimer+1, parseFrom.size()-positionOfDelimer));
        _ip = parseFrom.substr(0, positionOfDelimer);

    }
    catch (std::invalid_argument) {
        std::cout << "Host. Error while parsing port (to int)" << std::endl;
        }

}

Host::Host()
{
}

std::string Host::ip() const
{
    return _ip;
}

int Host::port() const
{
    return _port;
}

std::string Host::toString()
{
    return _ip + ":" + std::to_string(_port);
}
