#ifndef MYUPNP_MY_UPNP_H
#define MYUPNP_MY_UPNP_H

#include <iostream>

std::string getLocalIp();
const char* obrezka(std::string str_with_xml);

class Path
{
public:
    Path(std::string data){
        std::string location =     data.substr(data.find("LOCATION"), data.find("OPT:")-data.find("LOCATION"));
        std::string ipPathAndPort = location.substr(location.find("192" ) , location.size()-location.find("192"));
        mIp = ipPathAndPort.substr(0,ipPathAndPort.find(":"));
        mPort = ipPathAndPort.substr(ipPathAndPort.find(":")+1, ipPathAndPort.find("/")-ipPathAndPort.find(":")-1);
        mPath = ipPathAndPort.substr(ipPathAndPort.find("/"), ipPathAndPort.size() - ipPathAndPort.find("/"));
        //std::cout << ipPathAndPort << std::endl << ip << std::endl << port << std::endl << path << std::endl;
    }
    std::string mIp;
    std::string mPort;
    std::string mPath;

};

class MyUPnP{
public:
    MyUPnP();
    ~MyUPnP(){
        if(path)delete path;
        path=nullptr;
    }
    int openPort(unsigned int PortNumber = 4671, unsigned int LeaseDuration = 0, const char* ProgramName = "Telegram");
private:
    Path* path;
    std::string ctrUrl;
    void GetRoutersAddress();
    const char* Get_xml();
    void Parse_xml_to_ctrUrl(const char* m_xml);
    std::string GetText(unsigned int PortNumber, unsigned int LeaseDuration, const char*ProgramName);
};


#endif //MYUPNP_MY_UPNP_H
