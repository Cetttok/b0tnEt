//
// Created by sb on 17.07.2025.
//
#include <iostream>
#include "my_upnp.h"

#include "tcpconnect.h"
#include "udpworker.h"
#include "tinyxml2.h"

std::string getLocalIp()
{
    const char* google_dns_server = "8.8.8.8";
    int dns_port = 53;

    struct sockaddr_in serv;

    int sock = socket ( AF_INET, SOCK_DGRAM, 0);

    //Socket could not be created
    if(sock < 0)
    {
        perror("Socket error");
    }

    memset( &serv, 0, sizeof(serv) );
    serv.sin_family = AF_INET;
    serv.sin_addr.s_addr = inet_addr( google_dns_server );
    serv.sin_port = htons( dns_port );
    int err = connect( sock , (const struct sockaddr*) &serv , sizeof(serv) );
    struct sockaddr_in name;
    socklen_t namelen = sizeof(name);
    err = getsockname(sock, (struct sockaddr*) &name, &namelen);
    char buffer[16];
    const char* p = inet_ntop(AF_INET, &name.sin_addr, buffer, 16);
    close(sock);
    return std::string(buffer);
}

const char* obrezka(std::string str_with_xml){
    const char*  cstr_with_xml=  str_with_xml.c_str();
    while(! (
            cstr_with_xml[0]=='<' &&
            cstr_with_xml[1]=='?' &&
            cstr_with_xml[2]=='x' &&
            cstr_with_xml[3]=='m' &&
            cstr_with_xml[4]=='l' &&
            cstr_with_xml[5]==' '
    ))  cstr_with_xml=(char*)( (long long)cstr_with_xml + 1);
    return (const char*)cstr_with_xml;
}


void MyUPnP::GetRoutersAddress() {
    UdpWorker udp;

    std::string request =
            "M-SEARCH * HTTP/1.1\r\n"
            "Host: 239.255.255.250:1900\r\n"
            "Man: \"ssdp:discover\"\r\n"
            "MX: 3\r\n"
            "ST: urn:schemas-upnp-org:service:WANIPConnection:1\r\n"
            "\r\n";
    udp.send(request,1900,"239.255.255.250");

    std::string firstAns = udp.listen();
    //std::cout << udp.getLocalIp() << " - ip" << endl;
    std::cout << "first:\n\n---------------\n" <<firstAns<<"\n-------------\n"<<std::endl;
    path=new Path(firstAns );// deleting into ~MyUPnP
    std::cout << "path(" << path->mIp << " " << path->mPort << " " << path->mPath << ");" <<std::endl;
}

const char *MyUPnP::Get_xml() {
    TcpConnect*  connect = new TcpConnect(path->mIp,std::stoi(path->mPort));
    std::string request = std::string("GET ") + path->mPath + " HTTP/1.1\r\n"
                          +"Host: " + path->mIp+ "\r\n"
                          + "Connection: keep-alive\r\n"
                          + "\r\n";
    std::cout << "fixed" << connect->fixConnection() << std::endl;

    std::string answer= connect->recieveAnswer(request,60000);

    connect->closeConnection();
    delete connect;

    const char* _cstr_answer = obrezka( answer );
    char* cstr_answer = new char[answer.size()+1];//deleting into constructor MyUPnP

    if(cstr_answer) {
        for (unsigned int i = 0; i < answer.size() + 1; i++) cstr_answer[i] = _cstr_answer[i];
        std::cout << "xml--------------------------\n" << cstr_answer << "\n---------------------xml\n" << std::endl;
    }
    return (const char*)cstr_answer;
}

void MyUPnP::Parse_xml_to_ctrUrl(const char* m_xml) {

    tinyxml2::XMLDocument xml;

    std::cout<<"error id parse = "<< xml.Parse(m_xml) <<std::endl;
    const tinyxml2::XMLElement* service = xml.RootElement()->FirstChildElement("device")
            ->FirstChildElement("deviceList")
            ->FirstChildElement("device")
            ->FirstChildElement("deviceList")->FirstChildElement("device")->FirstChildElement("serviceList")
            ->FirstChildElement("service")->FirstChildElement("controlURL");

    std::cout<< "perfect\n" <<service->GetText()<<std::endl;
    ctrUrl = std::string(service->GetText());
}

std::string MyUPnP::GetText(unsigned int PortNumber, unsigned int LeaseDuration, const char* ProgramName) {

    std::string soapBody =
            "<?xml version=\"1.0\"?>"
            "<s:Envelope xmlns:s=\"http://schemas.xmlsoap.org/soap/envelope/\" "
            "s:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\">"
            "<s:Body>"
            "<u:AddPortMapping xmlns:u=\"urn:schemas-upnp-org:service:WANIPConnection:1\">"
            "<NewRemoteHost></NewRemoteHost>"
            "<NewExternalPort>" + std::to_string(PortNumber) + "</NewExternalPort>"
            "<NewProtocol>" + "TCP"+ "</NewProtocol>"
            "<NewInternalPort>" + std::to_string(PortNumber) + "</NewInternalPort>"
            "<NewInternalClient>" + getLocalIp()+ "</NewInternalClient>"
            "<NewEnabled>1</NewEnabled>"
            "<NewPortMappingDescription>" + ProgramName + "</NewPortMappingDescription>"
            "<NewLeaseDuration>" + std::to_string(LeaseDuration) + "</NewLeaseDuration>"
            "</u:AddPortMapping>"
            "</s:Body>"
            "</s:Envelope>";
    std::string headers =
            "POST " + ctrUrl + " HTTP/1.1\r\n"
            "Host: " + path->mIp+ ":"  + path->mPort+"\r\n"
            "Content-Type: text/xml\r\n"
            "SOAPAction: \"urn:schemas-upnp-org:service:WANIPConnection:1#AddPortMapping\"\r\n"
            "Content-Length: " + std::to_string(soapBody.size()) + "\r\n"
            "\r\n";

    return headers+soapBody;
}

int MyUPnP::openPort(unsigned int PortNumber, unsigned int LeaseDuration, const char* ProgramName) {

    TcpConnect connect(path->mIp, std::stoi(path->mPort));

    std::string message = GetText(PortNumber, LeaseDuration, ProgramName);

    std::cout << "answer ________________" <<std::endl;

    connect.fixConnection();
    std::cout << connect.recieveAnswer(message,60000) << std::endl; // 6000 - таймаут ожидания в микросекундах перед чтением
    connect.closeConnection();

    std::cout << "________________answer " <<std::endl;

    std::cout << "Your local ip - "<< getLocalIp() << std::endl;

    return 0;
}

MyUPnP::MyUPnP() {
    GetRoutersAddress();
    const char* xml = Get_xml();
    Parse_xml_to_ctrUrl(xml);
    delete[] xml;
}
