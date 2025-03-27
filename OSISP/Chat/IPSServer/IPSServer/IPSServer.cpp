#include <windows.h>
#include <string>
#include <chrono>
#include <iostream>
#include <mutex>
#include <vector>
#include <fstream>
#include <sstream>
#include <atomic>

using namespace std;

#define PIPE_NAME R"(\\.\pipe\Mypipe)"

bool running = true;
mutex log_mutex;
mutex messages_mutex;
ofstream logfile;

struct Message {
    unsigned int id;  
    unsigned int clientID;
    string text;
};

vector<Message> messages;
atomic<unsigned int> messageCounter(1);  

string CurrentTimestamp() {
    time_t now = time(nullptr);
    tm localTime;
    localtime_s(&localTime, &now);
    char buffer[64];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &localTime);
    return buffer;
}

unsigned int GetClientNumber(HANDLE hPipe) {
    DWORD dwClientNumber = 0;
    if (!GetNamedPipeClientProcessId(hPipe, &dwClientNumber)) {
        cerr << "Ошибка при получении номера клиента: " << GetLastError() << endl;
    }
    return dwClientNumber;
}

void HandleClient(HANDLE hPipe) {
    char buffer[1024];
    DWORD bytesRead;

    unsigned int clientID = GetClientNumber(hPipe);
    cout << "Подключен клиент №" << clientID << endl;

    while (true) {
        BOOL success = ReadFile(hPipe, buffer, sizeof(buffer) - 1, &bytesRead, NULL);
        if (!success || bytesRead == 0) {
            break; 
        }
        buffer[bytesRead] = '\0';
        string message = buffer;

        if (message == "GET_OTHER_MESSAGES") {
            lock_guard<mutex> lock(messages_mutex);
            stringstream response;
            for (const auto& msg : messages) {
                if (msg.clientID != clientID) {
                    response << "[" << msg.id << "] [" << CurrentTimestamp() << "] Клиент №" << msg.clientID << ": " << msg.text << "\n";
                }
            }
            string responseStr = response.str();
            DWORD bytesWritten;
            WriteFile(hPipe, responseStr.c_str(), static_cast<DWORD>(responseStr.size()), &bytesWritten, NULL);
            continue;
        }

        {
            lock_guard<mutex> lock(messages_mutex);
            unsigned int id = messageCounter++;
            messages.push_back({ id, clientID, message });
        }

        string timestamp = CurrentTimestamp();
        {
            lock_guard<mutex> lock(log_mutex);
            logfile << "[" << timestamp << "] Client №" << clientID << ": " << message << endl;
            logfile.flush();
        }

        cout << "Получено сообщение от клиента №" << clientID << ": " << message << endl;
    }

    CloseHandle(hPipe);
}

int main() {
    setlocale(LC_ALL, "Russian");
    logfile.open("IPSServer.log", ios::out);
    if (!logfile.is_open()) {
        cerr << "Ошибка при открытии файла журнала." << endl;
        return 1;
    }

    HANDLE hPipe;
    vector<thread> client_threads;

    cout << "Ожидание " << endl;

    while (running) {
        hPipe = CreateNamedPipeA(
            PIPE_NAME,
            PIPE_ACCESS_DUPLEX,
            PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
            PIPE_UNLIMITED_INSTANCES,
            1024, 
            1024, 
            0, 
            NULL);

        if (hPipe == INVALID_HANDLE_VALUE) {
            cerr << "Ошибка при создании канала: " << GetLastError() << endl;
            return 1;
        }

        BOOL connected = ConnectNamedPipe(hPipe, NULL) ? TRUE : (GetLastError() == ERROR_PIPE_CONNECTED);
        if (connected) {
            client_threads.emplace_back(HandleClient, hPipe);
        }
        else {
            CloseHandle(hPipe);
        }
    }

    for (auto& t : client_threads) {
        if (t.joinable()) t.join();
    }

    logfile.close();
    return 0;
}
