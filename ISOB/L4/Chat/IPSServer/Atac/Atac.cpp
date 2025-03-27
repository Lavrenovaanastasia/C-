#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <thread>

#pragma comment(lib, "ws2_32.lib")

using namespace std;

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 5000

void Attack(const char* serverIP, int serverPort) {
    for (int i = 0; i < 10; ++i) {  // 10 попыток подключений
        SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, 0);
        if (clientSocket == INVALID_SOCKET) {
            cerr << "Ошибка создания сокета." << endl;
            continue;
        }

        sockaddr_in serverAddr{};
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons(serverPort);
        inet_pton(AF_INET, serverIP, &serverAddr.sin_addr);

        if (connect(clientSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
            cerr << "Не удалось подключиться к серверу!" << endl;
        }
        else {
            cout << "Успешное подключение!" << endl;
        }

        closesocket(clientSocket);
    }
}

int main() {
    setlocale(LC_ALL, "Russian");
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        cerr << "Ошибка инициализации Winsock." << endl;
        return 1;
    }

    Attack(SERVER_IP, SERVER_PORT);

    WSACleanup();
    return 0;
}
