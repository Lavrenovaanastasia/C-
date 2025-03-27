#include <iostream>
#include <string>
#include <sstream>
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

    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) {
        cerr << "Ошибка создания сокета" << endl;
        WSACleanup();
        return -1;
    }

    sockaddr_in serv_addr;
    string username, password;

    cout << "Введите имя пользователя: ";
    cin >> username;
    cout << "Введите пароль: ";
    cin >> password;

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(8080);
    inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr);

    // Подключение к KDC
    if (connect(sock, (sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        cerr << "Ошибка подключения к KDC" << endl;
        closesocket(sock);
        WSACleanup();
        return -1;
    }

    // Отправка учетных данных в KDC
    string credentials = username + " " + password;
    send(sock, credentials.c_str(), credentials.size(), 0);

    // Получение ответа от KDC
    char buffer[1024] = { 0 };
    recv(sock, buffer, sizeof(buffer), 0);
    string response(buffer);

    // Разделение TGT и TGS
    stringstream ss(response);
    string tgt, tgs;
    getline(ss, tgt);
    getline(ss, tgs);

    cout << "Получен TGT: " << tgt << endl;
    cout << "Получен TGS: " << tgs << endl;

    // Подключение к серверу с TGS
    SOCKET server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock == INVALID_SOCKET) {
        cerr << "Ошибка создания сокета для сервера" << endl;
        closesocket(sock);
        WSACleanup();
        return -1;
    }

    serv_addr.sin_port = htons(8081);

    // Подключение к серверу
    if (connect(server_sock, (sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        cerr << "Ошибка подключения к серверу" << endl;
        closesocket(sock);
        closesocket(server_sock);
        WSACleanup();
        return -1;
    }

    // Отправка TGS на сервер
    send(server_sock, tgs.c_str(), tgs.size(), 0);

    // Закрытие сокетов
    closesocket(sock);
    closesocket(server_sock);
    WSACleanup();
    return 0;
}