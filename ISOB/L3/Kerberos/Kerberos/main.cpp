#include <iostream>
#include "KDC.h"
#include "Service.h"
#include "Client.h"
#include <cstdlib>
#include <ctime>

int main() {
    setlocale(LC_ALL, "Russian");
    srand(static_cast<unsigned int>(time(nullptr)));

    std::string dbPath = "database.txt";
    KDC kdc(dbPath);
    if (!kdc.loadDatabase()) {
        return 1;
    }
    Service service("MyService");
    std::string username, password;
    std::cout << "¬ведите им€ пользовател€: ";
    std::cin >> username;
    std::cout << "¬ведите пароль: ";
    std::cin >> password;
    Client client(username, password);
    client.requestService(service, kdc);

    system("pause");
    return 0;
}
