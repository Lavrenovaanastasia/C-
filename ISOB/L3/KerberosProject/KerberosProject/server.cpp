#include <iostream>
#include <string>
#include <winsock2.h>
#include <ws2tcpip.h>

using namespace std;

#pragma comment(lib, "Ws2_32.lib")

int main() {
    // Инициализация WinSock
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        cerr << "Ошибка инициализации WinSock" << endl;
        return -1;
    }

    SOCKET server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == INVALID_SOCKET) {
        cerr << "Ошибка создания сокета" << endl;
        WSACleanup();
        return -1;
    }

    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(8081);

    if (bind(server_socket, (sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        cerr << "Ошибка привязки сокета" << endl;
        closesocket(server_socket);
        WSACleanup();
        return -1;
    }

    listen(server_socket, SOMAXCONN);
    cout << "Сервер запущен. Ожидание подключения клиента..." << endl;

    SOCKET client_socket = accept(server_socket, nullptr, nullptr);
    if (client_socket == INVALID_SOCKET) {
        cerr << "Ошибка принятия соединения" << endl;
        closesocket(server_socket);
        WSACleanup();
        return -1;
    }

    char buffer[1024] = { 0 };
    recv(client_socket, buffer, sizeof(buffer), 0);
    string tgs(buffer);

    // Проверка TGS
    if (tgs.find("_TGS_") != string::npos) {
        cout << "Доступ разрешен!" << endl;
    }
    else {
        cout << "Доступ запрещен!" << endl;
    }

    closesocket(client_socket);
    closesocket(server_socket);
    WSACleanup();
    return 0;
}