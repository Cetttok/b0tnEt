#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include "executor.h"
using namespace std;
// просто все  но трудно было это писать и отлаживать
int main()
{
    Executor exec(time(0)); // тут seed это время. В принцыпе можно без него
    for (int i = 1; i < 10; i++){
        exec.startExecute("sleep 1 && echo 'num "+std::to_string(i)+"'", i);
    }
    std::cout << exec.stopExecuting(3) << std::endl;
    sleep(2);
    return 0;
}
