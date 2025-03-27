#include "Service.h"
#include "Crypto.h"
#include <iostream>
#include <ctime>

Service::Service(const std::string& serviceName) : name(serviceName) {}

bool Service::validateTicket(const Ticket& serviceTicket) {
    // Дешифруем сессионный ключ, полученный в сервисном билете
    std::string decryptedKey = Crypto::simpleDecrypt(serviceTicket.sessionKey);

    std::string expectedSuffix = "_" + name;
    if (decryptedKey.size() >= expectedSuffix.size() &&
        decryptedKey.compare(decryptedKey.size() - expectedSuffix.size(),
            expectedSuffix.size(), expectedSuffix) == 0) {
        // проверка время действия билета 
        std::time_t now = std::time(nullptr);
        if (difftime(now, serviceTicket.timestamp) < 300) {
            std::cout << "Сервис " << name << ": Тикет для "
                << serviceTicket.clientName << " действителен." << std::endl;
            return true;
        }
        else {
            std::cerr << "Сервис " << name << ": Тикет истёк." << std::endl;
        }
    }
    else {
        std::cerr << "Сервис " << name << ": Тикет недействителен." << std::endl;
    }
    return false;
}
