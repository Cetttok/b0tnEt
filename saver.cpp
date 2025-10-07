#include "saver.h"
#include <fstream>

//const int MAX_IP_ADDRES_LEN = 23;
//const int MAX_COMMAND_ID_LEN = 10; // установил случайно
const int MIN_HOST_SIZE = 9;
const char IP_PORT_DELIMER = ':';

const char * FILE_FOR_IP_NAME = "ipaddr.txt";
const char * FILE_FOR_COMMAND_ID_NAME =  "idcommands.txt";
const char * FILE_FOR_MAIN_PORT = "mainport.txt";
const char * FILE_FOR_PING_PORT = "pingport.txt";
const char * COMMANDS_DIRECORY = "commands";

const char * COMMAND_END_NAME = "_c.txt"; // типо <id>_c.txt
//const int COMMAND_READ_BUFFER = 32; перешел на новую систему считывания теперь тут не нужен буфер

Saver * Saver::_saver = nullptr;
Saver::Saver() {
     _countHostInFile = recountHostsInFile();
     _countCommandIdInFile = recountCommandIdInFile();
     std::filesystem::create_directories(COMMANDS_DIRECORY);
}

Saver::~Saver() {

}

void Saver::rewriteFile(std::string fileName, std::string data)
{
    std::ofstream out;          // поток для записи
    out.open(fileName);      // открываем файл для записи
    if (out.is_open())
    {
        out << data;
    }
    else{
        std::cout << "Saver. Cant rewrite file" << std::endl;
    }
    out.close();
    // FILE * _file = fopen(fileName.c_str(), "w");// плохо на запись чтение открывается
    // if (nullptr == _file) {
    //     std::cout << "Saver. cant write to file " <<fileName << std::endl;
    // }
    // fprintf(_file,"%s", data.c_str());
    // fclose(_file);
}

void Saver::createFile(std::string fileName)
{
    rewriteFile(fileName,"");
}


int Saver::recountHostsInFile()
{
    int countIp = 0;

    int countId = 0;
    std::string line = "";
    std::ifstream in(FILE_FOR_IP_NAME);
    if (in.is_open())
    {
        while (std::getline(in, line))
        {
            if (line != "" && line.size() >= 9){
                countIp++;
            }
            else{
                break;
            }
        }
    }
    else{
        createFile(FILE_FOR_IP_NAME);
        return 0;
    }
    in.close();


    // FILE * file = fopen(FILE_FOR_IP_NAME, "r+");
    // if (file == nullptr){
    //     createFile(FILE_FOR_IP_NAME);
    //     return 0;
    // }
    // char lineBuffer[MAX_IP_ADDRES_LEN]; // Буфер для хранения строки
    // while(fgets(lineBuffer, MAX_IP_ADDRES_LEN, file)!=nullptr) {
    //     std::string line(lineBuffer);
    //     if (line != "" && line.size() >= 9){
    //         countIp++;
    //     }
    //     else{
    //         break;
    //     }
    // }
    // fclose(file);
    return countIp;

}
int Saver::recountCommandIdInFile()
{
    int countId = 0;
    std::string line = "";
    std::ifstream in(FILE_FOR_COMMAND_ID_NAME);
    if (in.is_open())
    {
        while (std::getline(in, line))
        {
            if (line != ""){
                countId++;
            }
            else{
                break;
            }
        }
    }
    else{
        createFile(FILE_FOR_COMMAND_ID_NAME);
        return 0;
    }
    in.close();

    // FILE * file = fopen(FILE_FOR_COMMAND_ID_NAME, "r");
    // if (file == nullptr){
    //     createFile(FILE_FOR_COMMAND_ID_NAME);
    //     return 0;
    // }
    // char lineBuffer[MAX_COMMAND_ID_LEN]; // Буфер для хранения строки
    // while(fgets(lineBuffer, MAX_COMMAND_ID_LEN, file)!=nullptr) {
    //     std::string line(lineBuffer);
    //     if (line != ""){
    //         countId++;
    //     }
    //     else{
    //         break;
    //     }
    // }
    // fclose(file);
    //
    return countId;

}

std::string Saver::getCommandById(int id)
{
    //FILE * file = fopen(genFileNameFromCommandId(id).c_str(), "r");
    std::string result = "";
    std::string line = "";
    std::ifstream in(genFileNameFromCommandId(id).c_str());
    if (in.is_open())
    {
        while (std::getline(in, line))
        {
            result+= line + "\n";
        }
    }
    in.close();
    // if (file == nullptr){
    //     std::cout << "Saver. No file for id - " << id << " or simple cant open this file." << std::endl;
    //     return "";
    // }


    // char lineBuffer[COMMAND_READ_BUFFER]; // Буфер для хранения строки
    // while(fgets(lineBuffer, COMMAND_READ_BUFFER, file)!=nullptr) {
    //     result.append(lineBuffer,COMMAND_READ_BUFFER);
    // }

    // fclose(file);
    //
    return result;

}

std::string Saver::genFileNameFromCommandId(int id)
{
    return std::string(COMMANDS_DIRECORY) + "/" + std::to_string(id) + COMMAND_END_NAME;
}

int Saver::parseCommandIdFromFileName(std::string filename)
{
    std::string stringWithInt = filename.substr(0,filename.find(COMMAND_END_NAME));
    if (stringWithInt.find("/")!=stringWithInt.npos){
        stringWithInt = stringWithInt.substr(stringWithInt.find("/")+1,stringWithInt.size() - stringWithInt.find("/")); // опасная и неудобная реализации но похер
    }
    try{
        int result = std::stoi(stringWithInt);
        return result;
    }
    catch(std::invalid_argument){
        std::cout << "Svaer. Warn! Cant parse id from " << stringWithInt << std::endl;
        return 0;
    }
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
        if (size < _countHostInFile){
            std::cout << "Saver:Warning! invalid ip count" << std::endl;
        }
        else{
            size = _countHostInFile; // нужно ли это????

        }
    }
    //FILE * file = fopen(FILE_FOR_IP_NAME, "r");
    std::string line = "";
    std::ifstream outputStream(FILE_FOR_IP_NAME);
    int lineCount = 0;

    if (outputStream.is_open())
    {
        for (int lineCount = 0; lineCount < size; lineCount++){
            if (!std::getline(outputStream, line)){
                std::cout << "Saver:Warning! file is end, but count not done";
                break;
            }
            if (line != "" && line.size() >= MIN_HOST_SIZE){ //последняя строка пустая
                  result[lineCount] = Host(line); // Host сам парсит себя
            }
            else{
                break;
            }

        }
    }
    outputStream.close();
    // char lineBuffer[MAX_IP_ADDRES_LEN] = {0}; // Буфер для хранения строки
    // int lineCount = 0;
    // for(int lineCount = 0; lineCount < size; lineCount++) {
    //     if(fgets(lineBuffer, MAX_IP_ADDRES_LEN, file)==nullptr){
    //         std::cout << "Saver:Warning! file is end, but count not done";
    //         break;
    //     }
    //     std::string line(lineBuffer);
    //     if (line != "" && line.size() >= MIN_HOST_SIZE){ //последняя строка пустая
    //         result[lineCount] = Host(line); // Host сам парсит себя
    //     }
    //     else{
    //         break;
    //     }
    // }
    // fclose(file);
    //
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
       // size = _countCommandIdInFile;
    }
    //FILE * file = fopen(FILE_FOR_COMMAND_ID_NAME, "r");
    std::string line = "";
    std::ifstream outputStream(FILE_FOR_COMMAND_ID_NAME);
    int lineCount = 0;

    if (outputStream.is_open())
    {
        for (int lineCount = 0; lineCount < size; lineCount++){
            if (!std::getline(outputStream, line)){
                std::cout << "Saver:Warning! file is end, but count not done" << std::endl;
                break;
            }
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
    }
    outputStream.close();

    // char lineBuffer[MAX_COMMAND_ID_LEN]= {0}; ; // Буфер для хранения строки
    // int lineCount = 0;
    // for(int lineCount = 0; lineCount < size; lineCount++) {
    //     if(fgets(lineBuffer, MAX_COMMAND_ID_LEN, file)==nullptr){
    //         std::cout << "Saver:Warning! file is end, but count not done";
    //         break;
    //     }
    //     std::string line(lineBuffer);
    //     if (line != "" ){ //последняя строка пустая
    //         try {
    //             result[lineCount] = std::stoi(line);
    //         } catch (std::invalid_argument) {
    //             std::cout << "Saver. cant convert to int. badId '" << line << "'" << std::endl;
    //             break;
    //         }
    //     }
    //     else{
    //         break;
    //     }
    // }
    // fclose(file);
    return lineCount;
}

std::map<int, std::string> Saver::loadCommands()
{
    std::string path = COMMANDS_DIRECORY;
    std::map<int, std::string> result;
    for (const auto & entry : std::filesystem::directory_iterator(path)){

        int id = parseCommandIdFromFileName(entry.path());
        if (id!=0){
            result[id] = getCommandById(id);
        }
    }
    return result;
}

bool Saver::clearCommandOnDisk(int id)
{
    if(remove(genFileNameFromCommandId(id).c_str())==0){
        return true;
    }
    else{
        std::cout << "Saver. Cant clear command! id - " << id <<std::endl;
        return false;
    }

}

void Saver::saveCommandOnDisk(std::string command, int id)
{
    rewriteFile(genFileNameFromCommandId(id),command);
}

int Saver::getMainPort()
{
    std::string line = "";
    std::ifstream outputStream(FILE_FOR_MAIN_PORT);
    if (std::getline(outputStream, line)){
        return std::stoi(line);
    }
    else{
        std::cout << "Saver. Cant read main port. Return 0"<< std ::endl;
        return 0;
    }
}

void Saver::setMainPort(int port)
{
    rewriteFile(FILE_FOR_MAIN_PORT,std::to_string(port));
}

int Saver::getPingPort()
{
    std::string line = "";
    std::ifstream outputStream(FILE_FOR_PING_PORT);
    if (std::getline(outputStream, line)){
        return std::stoi(line);
    }
    else{
        std::cout << "Saver. Cant read ping port. Return 0"<< std ::endl;
        return 0;
    }
}

void Saver::setPingPort(int port)
{
    rewriteFile(FILE_FOR_PING_PORT,std::to_string(port));
}

Host::Host(std::string ip, int mainPort, int pingPort):_ip(ip), _mainPort(mainPort), _pingPort(pingPort)
{// зачем этот конструктор
    //существует
    _isNull = false;
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


        _ip = parseFrom.substr(0, positionOfDelimer);
        parseFrom = parseFrom.substr(_ip.size()+1, parseFrom.size());
        std::string forMainPort = parseFrom.substr(0, parseFrom.find(IP_PORT_DELIMER));
        _mainPort = std::stoi(forMainPort);
        std::string forPingPort = parseFrom.substr(parseFrom.find(IP_PORT_DELIMER)+1, parseFrom.size());
        _pingPort = std::stoi(forPingPort);

    }
    catch (std::invalid_argument) {
        std::cout << "Host. Error while parsing port (to int)" << std::endl;
        return;
        }
    _isNull = false;

}

Host::Host()
{
}

std::string Host::ip() const
{
    return _ip;
}

int Host::mainPort() const
{
    return _mainPort;
}

std::string Host::toString()
{
    return _ip + ":" + std::to_string(_mainPort) + ":"  + std::to_string(_pingPort);
}

bool Host::isNull()
{
    if (_isNull){
        return _isNull;
    }
    else{
        if (mainPort()== 0 | pingPort() == 0){
            return true;
        }
        return false;
    }

}

bool Host::operator==(const Host &b)
{
    return (_mainPort==b.mainPort() && _pingPort==b.pingPort() && _mainPort==b.mainPort() &&  _ip == b.ip());
}

int Host::pingPort() const
{
    return _pingPort;
}
