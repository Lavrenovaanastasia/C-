#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <thread>
#include <string>
#include <algorithm>

#pragma comment(lib, "ws2_32.lib")

using namespace std;

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 5000

void ReceiveMessages(SOCKET clientSocket) {
    char buffer[1024];
    int bytesRead;
    while ((bytesRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0)) != SOCKET_ERROR) {
        if (bytesRead == 0) break;  
        buffer[bytesRead] = '\0';
        string receivedMessage(buffer);
        receivedMessage.erase(remove(receivedMessage.begin(), receivedMessage.end(), '\r'), receivedMessage.end());
        cout << receivedMessage << endl;
    }
}

void SendMessages(SOCKET clientSocket) {
    string message;
    while (true) {
        getline(cin, message);
        if (message.empty()) continue;
        send(clientSocket, message.c_str(), message.size(), 0);
    }
}

int main() {
    setlocale(LC_ALL, "Russian");

    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        cerr << "Ошибка инициализации Winsock." << endl;
        return 1;
    }

    SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == INVALID_SOCKET) {
        cerr << "Ошибка создания сокета." << endl;
        WSACleanup();
        return 1;
    }

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(SERVER_PORT);
    inet_pton(AF_INET, SERVER_IP, &serverAddr.sin_addr);

    if (connect(clientSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        cerr << "Не удалось подключиться к серверу!" << endl;
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }

    cout << "Подключено к серверу." << endl;

    thread receiveThread(ReceiveMessages, clientSocket);
    receiveThread.detach();  // Поток для получения сообщений

    thread sendThread(SendMessages, clientSocket);  
    sendThread.detach();

   
    while (true) {
       
    }

    closesocket(clientSocket);
    WSACleanup();
    return 0;
}
