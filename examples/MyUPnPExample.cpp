#include "tcpconnect.h"
#include "udpworker.h"

#include <iostream>
#include "my_upnp.h"

int main()
{
    MyUPnP test_upnp_class;
    test_upnp_class.openPort();//open port 4671 on max time from "Telegram"
    test_upnp_class.openPort(4567,30, "WireShark");
    return  0;
}
