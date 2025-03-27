#include <iostream>
#include <string>
#include <sstream>
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

    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) {
        cerr << "������ �������� ������" << endl;
        WSACleanup();
        return -1;
    }

    sockaddr_in serv_addr;
    string username, password;

    cout << "������� ��� ������������: ";
    cin >> username;
    cout << "������� ������: ";
    cin >> password;

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(8080);
    inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr);

    // ����������� � KDC
    if (connect(sock, (sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        cerr << "������ ����������� � KDC" << endl;
        closesocket(sock);
        WSACleanup();
        return -1;
    }

    // �������� ������� ������ � KDC
    string credentials = username + " " + password;
    send(sock, credentials.c_str(), credentials.size(), 0);

    // ��������� ������ �� KDC
    char buffer[1024] = { 0 };
    recv(sock, buffer, sizeof(buffer), 0);
    string response(buffer);

    // ���������� TGT � TGS
    stringstream ss(response);
    string tgt, tgs;
    getline(ss, tgt);
    getline(ss, tgs);

    cout << "������� TGT: " << tgt << endl;
    cout << "������� TGS: " << tgs << endl;

    // ����������� � ������� � TGS
    SOCKET server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock == INVALID_SOCKET) {
        cerr << "������ �������� ������ ��� �������" << endl;
        closesocket(sock);
        WSACleanup();
        return -1;
    }

    serv_addr.sin_port = htons(8081);

    // ����������� � �������
    if (connect(server_sock, (sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        cerr << "������ ����������� � �������" << endl;
        closesocket(sock);
        closesocket(server_sock);
        WSACleanup();
        return -1;
    }

    // �������� TGS �� ������
    send(server_sock, tgs.c_str(), tgs.size(), 0);

    // �������� �������
    closesocket(sock);
    closesocket(server_sock);
    WSACleanup();
    return 0;
}