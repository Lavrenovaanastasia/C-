#include <windows.h>
#include <string>
#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <unordered_set>

using namespace std;

#define PIPE_NAME R"(\\.\pipe\Mypipe)"

void ClientThread(int source_id) {
    HANDLE hPipe;
    DWORD bytesWritten, bytesRead;
    char buffer[4096];
    unsigned int lastMessageNumber = 0; // Счетчик выведенных сообщений

    while (true) {
        hPipe = CreateFileA(PIPE_NAME, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);

        if (hPipe != INVALID_HANDLE_VALUE) break;

        if (GetLastError() != ERROR_PIPE_BUSY) {
            cerr << "Ошибка подключения клиента №" << source_id << endl;
            return;
        }

        if (!WaitNamedPipeA(PIPE_NAME, 1000)) {
            cerr << "Сервер не отвечает для клиента №" << source_id << endl;
            return;
        }
    }

    cout << "Клиент №" << source_id << " подключен. Введите сообщения." << endl;

    thread timer_thread([&]() {
        while (true) {
            this_thread::sleep_for(chrono::seconds(1));
            string command = "GET_OTHER_MESSAGES";
            WriteFile(hPipe, command.c_str(), static_cast<DWORD>(command.size()), &bytesWritten, NULL);

            BOOL result = ReadFile(hPipe, buffer, sizeof(buffer) - 1, &bytesRead, NULL);
            if (result && bytesRead > 0) {
                buffer[bytesRead] = '\0';
                string messages(buffer);

                // Разбор сообщений по строкам
                size_t start = 0, end;
                while ((end = messages.find('\n', start)) != string::npos) {
                    string message = messages.substr(start, end - start);
                    start = end + 1;

                    // Извлечение порядкового номера сообщения
                    size_t idStart = message.find('[') + 1;
                    size_t idEnd = message.find(']');
                    if (idStart != string::npos && idEnd != string::npos && idEnd > idStart) {
                        unsigned int messageID = stoi(message.substr(idStart, idEnd - idStart));

                        if (messageID > lastMessageNumber) {
                            lastMessageNumber = messageID;
                            cout << "Новое сообщение:\n" << message << endl;
                        }
                    }
                }
            }
        }
        });

    while (true) {
        string message;
        getline(cin, message);

        if (message.empty()) continue;

        WriteFile(hPipe, message.c_str(), static_cast<DWORD>(message.size()), &bytesWritten, NULL);
        cout << "Сообщение отправлено: " << message << endl;
    }

    timer_thread.detach();
    CloseHandle(hPipe);
}

int main() {
    setlocale(LC_ALL, "Russian");

    int client_count = 1;

    vector<thread> clients;
    for (int i = 0; i < client_count; ++i) {
        clients.emplace_back(ClientThread, i + 1);
    }

    for (auto& client : clients) {
        client.join();
    }

    return 0;
}
