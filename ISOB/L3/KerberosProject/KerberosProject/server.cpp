#include <iostream>
#include <string>
#include <winsock2.h>
#include <ws2tcpip.h>

using namespace std;

#pragma comment(lib, "Ws2_32.lib")

int main() {
    // ������������� WinSock
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        cerr << "������ ������������� WinSock" << endl;
        return -1;
    }

    SOCKET server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == INVALID_SOCKET) {
        cerr << "������ �������� ������" << endl;
        WSACleanup();
        return -1;
    }

    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(8081);

    if (bind(server_socket, (sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        cerr << "������ �������� ������" << endl;
        closesocket(server_socket);
        WSACleanup();
        return -1;
    }

    listen(server_socket, SOMAXCONN);
    cout << "������ �������. �������� ����������� �������..." << endl;

    SOCKET client_socket = accept(server_socket, nullptr, nullptr);
    if (client_socket == INVALID_SOCKET) {
        cerr << "������ �������� ����������" << endl;
        closesocket(server_socket);
        WSACleanup();
        return -1;
    }

    char buffer[1024] = { 0 };
    recv(client_socket, buffer, sizeof(buffer), 0);
    string tgs(buffer);

    // �������� TGS
    if (tgs.find("_TGS_") != string::npos) {
        cout << "������ ��������!" << endl;
    }
    else {
        cout << "������ ��������!" << endl;
    }

    closesocket(client_socket);
    closesocket(server_socket);
    WSACleanup();
    return 0;
}