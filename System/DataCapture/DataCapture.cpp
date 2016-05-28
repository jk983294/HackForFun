#include <winsock2.h>
#include <stdio.h>
#include <iostream>
#include <ws2tcpip.h>
#include <fstream>
#include <windows.h>

#pragma comment (lib, "WS2_32.lib")

#define IO_RCVALL _WSAIOW(IOC_VENDOR,1)
#define MY_PORT 8023

typedef struct IP_HEADER {
    union {
        unsigned char Version;
        unsigned char HeadLen;
    };
    unsigned char ServiceType;
    unsigned short TotalLen;
    unsigned short Identifier;
    union {
        unsigned short Flags;
        unsigned short FragOffset;
    };
    unsigned char TimeToLive;
    unsigned char Protocol;
    unsigned short HeadChecksum;
    unsigned int SourceAddr;
    unsigned int DestinAddr;
    unsigned char Options;
} IP_HEADER;

using namespace std;

ofstream outfile("C://logfile.txt",ios::out);

void print_data(char szBuf[], int len){
    dec(cout);
    IP_HEADER ip= *(IP_HEADER *)szBuf;
    cout<<"-----------------------"<<endl;
    cout<<"version: "<<(ip.Version>>4)<<endl;
    cout<<"len:"<<((ip.HeadLen & 0x0f)*4)<<endl;
    cout<<"ServiceType:Priority"<<(ip.ServiceType>>5)<<", Service"<<((ip.ServiceType>>1)&0x0f)<<endl;
    cout<<"TotalLen:"<<ntohs(ip.TotalLen)<<endl;
    cout<<"Identifier:"<<ntohs(ip.Identifier)<<endl;
    cout<<"DF:"<<((ip.Flags>>15)&0x01)<<",DF= "<<((ip.Flags>>14)&0x01)<<",Mf="<<((ip.Flags>>13)&0x01)<<endl;
    cout<<"FragOffset:"<<(ip.FragOffset&0x1fff)<<endl;
    cout<<"TTL:"<<(int)ip.TimeToLive<<endl;
    cout<<"Protocol:"<<(int)ip.Protocol<<endl;
    cout<<"HeadChecksum:"<<hex<<ntohs(ip.HeadChecksum)<<hex<<endl;
    cout<<"SourceAddr:"<<inet_ntoa(*(in_addr *)&ip.SourceAddr)<<endl;
    cout<<"DestinAddr:"<<inet_ntoa(*(in_addr *)&ip.DestinAddr)<<endl;

    outfile<<"-----------------------"<<endl;
    dec(outfile);
    outfile<<"-----------------------"<<endl;
    outfile<<"version: "<<(ip.Version>>4)<<endl;
    outfile<<"len:"<<((ip.HeadLen & 0x0f)*4)<<endl;
    outfile<<"ServiceType:Priority"<<(ip.ServiceType>>5)<<", Service"<<((ip.ServiceType>>1)&0x0f)<<endl;
    outfile<<"TotalLen:"<<ntohs(ip.TotalLen)<<endl;
    outfile<<"Identifier:"<<ntohs(ip.Identifier)<<endl;
    outfile<<"DF:"<<((ip.Flags>>15)&0x01)<<",DF= "<<((ip.Flags>>14)&0x01)<<",Mf="<<((ip.Flags>>13)&0x01)<<endl;
    outfile<<"FragOffset:"<<(ip.FragOffset&0x1fff)<<endl;
    outfile<<"TTL:"<<(int)ip.TimeToLive<<endl;
    outfile<<"Protocol:"<<(int)ip.Protocol<<endl;
    outfile<<"HeadChecksum:"<<hex<<ntohs(ip.HeadChecksum)<<hex<<endl;
    outfile<<"SourceAddr:"<<inet_ntoa(*(in_addr *)&ip.SourceAddr)<<endl;
    outfile<<"DestinAddr:"<<inet_ntoa(*(in_addr *)&ip.DestinAddr)<<endl;
}

int capture_data(){
    WSADATA stwsa;  // open dll
    if(WSAStartup(MAKEWORD(2,2), &stwsa) != 0){
        printf("WSA startup error.\n");
        return -1;
    }

    char szHostName[256];
    if(gethostname(szHostName, 256) != 0){
        printf("gethostbyname error.\n");
        WSACleanup();
        return -1;
    }

    printf("hostName: %s\n", szHostName);
    HOSTENT *pHost = gethostbyname(szHostName);

    printf("my IP: %d.%d.%d.%d\n", pHost->h_addr_list[0][1] & 0xff,
        pHost->h_addr_list[0][1] & 0xff,
        pHost->h_addr_list[0][2] & 0xff,
        pHost->h_addr_list[0][3] & 0xff);

    sockaddr_in stAddr;
    stAddr.sin_addr = *(in_addr *)pHost->h_addr_list[0]; 
    stAddr.sin_family = AF_INET;
    stAddr.sin_port = htons(MY_PORT);

    SOCKET iFd = socket(AF_INET, SOCK_RAW, IPPROTO_IP);      // receive raw data stream, listen to all port
    if(iFd < 0){
        printf("open raw socket failed.\n");
        WSACleanup();
        return -1;
    }

    if(bind(iFd, (SOCKADDR*)&stAddr, sizeof(SOCKADDR)) != 0){
        printf("bind socket failed.\n");
        WSACleanup();
        closesocket(iFd);
        return -1;
    }

    DWORD dwInBuff = 1, dwOutBuff, dwBytesRet;
    int iRet = WSAIoctl(iFd, IO_RCVALL, &dwInBuff, sizeof(dwInBuff), &dwOutBuff, sizeof(dwOutBuff), &dwBytesRet, NULL, NULL);    // setup receieve rule, receieve all
    if(iRet != 0){
        printf("WSAIoctl failed.\n");
        WSACleanup();
        closesocket(iFd);
        return -1;
    }

    char szBuf[65536];
    while(true){
        memset(szBuf, 0, 65536);
        iRet = recv(iFd, szBuf, 65536, 0);
        if(iRet < 0){
            break;
        } else {
            print_data(szBuf, iRet);
        }
    }
    WSACleanup();
    return 0;
}

int main(){
    capture_data();
    system("pause");
    return 0;
}