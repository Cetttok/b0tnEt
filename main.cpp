#include "globaltransporter.h"
#include "thread"
#include <iostream>

using namespace std;

int main()
{//std::this_thread::sleep_for(std::chrono::seconds(3));
    GlobalTransporter * global  = new GlobalTransporter ();
    global->load();
    //std::this_thread::sleep_for(std::chrono::seconds(3));
    global->save();
     std::thread * thread = new std::thread(&GlobalTransporter::startListening,global);
    //     //global->startListening();
    cout << "start" << endl;
    global->eventLoop();
    // //global->accept("updateResponse{hostList{<host>192.168.0.11:3456</host>\n<host>192.168.0.13:3312</host>\n}hostListcommands{<command><id>12</id>echo 0</command>}commands}updateResponse");
    // cout << "end" << endl;


    // Executor exec(time(0));
    // exec.startExecute("echo 0", 32);
    // std::this_thread::sleep_for(std::chrono::seconds(5));
    // std::cout << "Executed - " << exec.getFirstExecutedId();


    return 0;
}
