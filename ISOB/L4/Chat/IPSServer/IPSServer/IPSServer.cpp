#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <unordered_map>
#include <fstream>
#include <atomic>
#include <string>
#include <algorithm>
#include <sstream>
#include <chrono>

#pragma comment(lib, "ws2_32.lib")

using namespace std;

#define SERVER_PORT 5000
#define PASSWORD "2"
#define MAX_CONNECTIONS_PER_IP 3  
#define CONNECTION_LIMIT_TIME_SEC 10  

struct ClientInfo {
    SOCKET socket;
    int id;
    string ip;
    bool authenticated;
};

atomic<int> clientCounter(1);
vector<ClientInfo> clients;
mutex clients_mutex, log_mutex, messages_mutex;

// Структура для защиты от атак
unordered_map<string, vector<chrono::steady_clock::time_point>> connectionAttempts;

void LogMessage(const string& msg) {
    lock_guard<mutex> lock(log_mutex);
    cout << msg << endl;
}

void WriteToDatabase(size_t hashValue, const string& originalMessage) {
    lock_guard<mutex> lock(messages_mutex);
    ofstream dbFile("hashes.txt", ios::app);
    if (dbFile.is_open()) {
        dbFile << hashValue << ":" << originalMessage << "\n";
    }
    else {
        LogMessage("Ошибка при открытии файла базы данных.");
    }
}

string ReadFromDatabase(size_t hashValue) {
    lock_guard<mutex> lock(messages_mutex);
    ifstream dbFile("hashes.txt");
    string line;
    while (getline(dbFile, line)) {
        size_t delimiterPos = line.find(':');
        if (delimiterPos != string::npos) {
            size_t storedHash = stoull(line.substr(0, delimiterPos));
            if (storedHash == hashValue) {
                return line.substr(delimiterPos + 1);
            }
        }
    }
    return "";
}

void BroadcastMessage(const string& originalMsg, size_t hashValue, int senderId) {
    lock_guard<mutex> lock(clients_mutex);
    for (const auto& client : clients) {
        // Если клиент не тот, кто отправил сообщение
        if (client.id != senderId) {
            string msg;
            if (client.authenticated) {
                msg = "Клиент #" + to_string(senderId) + ": " + originalMsg + "\n";
            }
            else {
                msg = "Клиент #" + to_string(senderId) + " [Хеш]: " + to_string(hashValue) + "\n";
            }
            send(client.socket, msg.c_str(), msg.size(), 0);
        }
    }
}

bool IsConnectionRateLimited(const string& clientIP) {
    auto now = chrono::steady_clock::now();
    auto& attempts = connectionAttempts[clientIP];

  
    attempts.erase(remove_if(attempts.begin(), attempts.end(),
        [now](const chrono::steady_clock::time_point& tp) {
            return chrono::duration_cast<chrono::seconds>(now - tp).count() > CONNECTION_LIMIT_TIME_SEC;
        }),
        attempts.end());

    if (attempts.size() >= MAX_CONNECTIONS_PER_IP) {
        return true; // Превышен лимит подключений
    }

    
    attempts.push_back(now);
    return false;
}

void HandleClient(SOCKET clientSocket, string clientIP, int clientId) {
    char buffer[1024];
    int bytesReceived;

    if (IsConnectionRateLimited(clientIP)) {
        string warningMessage = "Слишком много подключений с вашего IP. Пожалуйста, подождите.\n";
        send(clientSocket, warningMessage.c_str(), warningMessage.size(), 0);
        closesocket(clientSocket);
        return;
    }

    {
        lock_guard<mutex> lock(clients_mutex);
        clients.push_back({ clientSocket, clientId, clientIP, false });
    }
    LogMessage("Клиент #" + to_string(clientId) + " (" + clientIP + ") подключился.");

    send(clientSocket, "Введите пароль: ", 16, 0);
    bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
    if (bytesReceived <= 0) {
        closesocket(clientSocket);
        return;
    }
    buffer[bytesReceived] = '\0';
    string receivedPassword(buffer);
    receivedPassword.erase(remove(receivedPassword.begin(), receivedPassword.end(), '\r'), receivedPassword.end());
    receivedPassword.erase(remove(receivedPassword.begin(), receivedPassword.end(), '\n'), receivedPassword.end());

    if (receivedPassword != PASSWORD) {
        send(clientSocket, "Неверный пароль! Отключение.\n", 30, 0);
        closesocket(clientSocket);
        return;
    }

    {
        lock_guard<mutex> lock(clients_mutex);
        for (auto& client : clients) {
            if (client.id == clientId) {
                client.authenticated = true;
                break;
            }
        }
    }
    send(clientSocket, "Доступ разрешен!\n", 18, 0);
    LogMessage("Клиент #" + to_string(clientId) + " прошел аутентификацию.");

    while ((bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0)) > 0) {
        buffer[bytesReceived] = '\0';
        string message(buffer);
        message.erase(remove(message.begin(), message.end(), '\r'), message.end());
        message.erase(remove(message.begin(), message.end(), '\n'), message.end());
        if (message.empty()) continue;

        size_t hashValue;
        bool isNumeric = false;
        try {
            hashValue = stoull(message);
            isNumeric = true;
        }
        catch (...) {
            isNumeric = false;
        }

        if (isNumeric) {
            bool isAuthenticated = false;
            {
                lock_guard<mutex> lock(clients_mutex);
                for (const auto& client : clients) {
                    if (client.id == clientId) {
                        isAuthenticated = client.authenticated;
                        break;
                    }
                }
            }
            if (!isAuthenticated) {
                string response = "Ошибка: требуется аутентификация.\n";
                send(clientSocket, response.c_str(), response.size(), 0);
                continue;
            }

            string originalMessage = ReadFromDatabase(hashValue);
            string response;
            if (!originalMessage.empty()) {
                response = "[Расшифровано] " + originalMessage + "\n";
            }
            else {
                response = "Неизвестный хеш.\n";
            }
            send(clientSocket, response.c_str(), response.size(), 0);
        }
        else {
            hash<string> hasher;
            size_t computedHash = hasher(message);
            WriteToDatabase(computedHash, message);
            string confirmation = "Сообщение принято. Хеш: " + to_string(computedHash) + "\n";
            send(clientSocket, confirmation.c_str(), confirmation.size(), 0);
            BroadcastMessage(message, computedHash, clientId);
        }
    }

    {
        lock_guard<mutex> lock(clients_mutex);
        auto it = remove_if(clients.begin(), clients.end(), [clientId](const ClientInfo& info) {
            return info.id == clientId;
            });
        clients.erase(it, clients.end());
    }
    LogMessage("Клиент #" + to_string(clientId) + " отключился.");
    closesocket(clientSocket);
}

int main() {
    setlocale(LC_ALL, "Russian");

    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        cerr << "Ошибка инициализации Winsock." << endl;
        return 1;
    }

    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        cerr << "Ошибка создания сокета." << endl;
        WSACleanup();
        return 1;
    }

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(SERVER_PORT);

    if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        cerr << "Ошибка привязки сокета." << endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) {
        cerr << "Ошибка прослушивания." << endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    LogMessage("Сервер запущен на порту " + to_string(SERVER_PORT) + "...");
    vector<thread> clientThreads;
    while (true) {
        sockaddr_in clientAddr;
        int clientAddrSize = sizeof(clientAddr);
        SOCKET clientSocket = accept(serverSocket, (sockaddr*)&clientAddr, &clientAddrSize);
        if (clientSocket == INVALID_SOCKET) {
            cerr << "Ошибка принятия подключения." << endl;
            continue;
        }
        char clientIP[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &clientAddr.sin_addr, clientIP, INET_ADDRSTRLEN);
        int clientId = clientCounter.fetch_add(1);
        clientThreads.emplace_back(HandleClient, clientSocket, string(clientIP), clientId);
    }

    for (auto& t : clientThreads) {
        if (t.joinable())
            t.join();
    }

    closesocket(serverSocket);
    WSACleanup();
    return 0;
}
