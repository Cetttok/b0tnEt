#include <iostream>
#include "commandtrasporter.h"
using namespace std;

int main()
{
    CommandTrasporter transport;
    transport.addCommand(16,"echo 'second command'\n");
    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::cout << transport.shareCommands().size()<< std::endl;
    for (auto & pair : transport.shareCommands()){
        std::cout << "id - " << pair.first << " command - " << pair.second << std::endl;
    }

    return 0;
}
