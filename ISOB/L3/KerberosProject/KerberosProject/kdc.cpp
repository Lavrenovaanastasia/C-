#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <string>
#include <ctime>
#include <winsock2.h>
#include <ws2tcpip.h>

using namespace std;

#pragma comment(lib, "Ws2_32.lib")

unordered_map<string, string> loadDatabase(const string& filename) {
    unordered_map<string, string> users;
    ifstream file(filename);
    string line;

    while (getline(file, line)) {
        stringstream ss(line);
        string username, password;
        ss >> username >> password;
        users[username] = password;
    }

    return users;
}

string generateTGT(const string& username) {
    return username + "_TGT_" + to_string(time(nullptr));
}

string generateTGS(const string& username) {
    return username + "_TGS_" + to_string(time(nullptr));
}

int main() {
    unordered_map<string, string> users = loadDatabase("database.txt");

    // Инициализация WinSock
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    // Создание сокета
    SOCKET server_socket = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(8080);

    bind(server_socket, (sockaddr*)&server_addr, sizeof(server_addr));
    listen(server_socket, SOMAXCONN);

    cout << "KDC запущен. Ожидание подключения клиента..." << endl;

    SOCKET client_socket = accept(server_socket, nullptr, nullptr);
    char buffer[1024] = { 0 };
    recv(client_socket, buffer, sizeof(buffer), 0);
    string credentials(buffer);

    // Разделение имени пользователя и пароля
    stringstream ss(credentials);
    string username, password;
    ss >> username >> password;

    // Проверка пользователя
    if (users.find(username) != users.end() && users[username] == password) {
        string tgt = generateTGT(username);
        cout << "Аутентификация успешна! TGT: " << tgt << endl;

        // Эмуляция запроса TGS
        string tgs = generateTGS(username);
        cout << "TGS: " << tgs << endl;

        // Отправка TGT и TGS клиенту
        string response = tgt + "\n" + tgs;
        send(client_socket, response.c_str(), response.size(), 0);
    }
    else {
        cout << "Ошибка аутентификации!" << endl;
        const char* error_msg = "Ошибка аутентификации!";
        send(client_socket, error_msg, strlen(error_msg), 0);
    }

    closesocket(client_socket);
    closesocket(server_socket);
    WSACleanup();
    return 0;
}