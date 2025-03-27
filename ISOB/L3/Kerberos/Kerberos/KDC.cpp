#include "KDC.h"
#include "Crypto.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <cstdlib>
#include <iomanip>  
KDC::KDC(const std::string& dbPath) : databasePath(dbPath) {}

bool KDC::loadDatabase() {
    std::ifstream file(databasePath);
    if (!file) {
        std::cerr << "Не удалось открыть базу данных: " << databasePath << std::endl;
        return false;
    }
    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string username, password;
        if (iss >> username >> password) {
            credentials[username] = password;
        }
    }
    file.close();
    return true;
}

std::string KDC::generateSessionKey() {
    // генерируем случайный сессионный ключ в виде строки
    int num = rand() % 1000000;
    return "SK" + std::to_string(num);
}

Ticket KDC::issueTGT(const std::string& clientName, const std::string& password) {
    Ticket ticket;
    //существует ли пользователь в базе
    if (credentials.find(clientName) == credentials.end()) {
        std::cerr << "Пользователь " << clientName << " не найден." << std::endl;
        return ticket;
    }
    // корректность пароля
    if (credentials[clientName] != password) {
        std::cerr << "Неверный пароль для " << clientName << std::endl;
        return ticket;
    }
    // Выдаем TGT
    ticket.clientName = clientName;
    ticket.longTermKey = password;  // Долговременный ключ(пароль)
    std::string plainKey = generateSessionKey();

    // Шифруем сессионный ключ перед отправкой
    ticket.sessionKey = Crypto::simpleEncrypt(plainKey);
    ticket.timestamp = std::time(nullptr);  

    std::cout << "KDC: Выдан TGT для " << clientName << std::endl;
    std::cout << "Долговременный ключ (пароль): " << ticket.longTermKey << std::endl;
    std::cout << "Зашифрованный сессионный ключ (TGT): " << ticket.sessionKey << std::endl;

    return ticket;
}

Ticket KDC::issueServiceTicket(const Ticket& tgt, const std::string& serviceName) {
    Ticket serviceTicket;
    serviceTicket.clientName = tgt.clientName;
    // Дешифруем TGT, чтобы получить исходный сессионный ключ
    std::string decryptedKey = Crypto::simpleDecrypt(tgt.sessionKey);

    // Модифицируем сессионный ключ для конкретного сервиса
    std::string serviceKey = decryptedKey + "_" + serviceName;

    // Шифруем полученный ключ для передачи клиенту
    serviceTicket.sessionKey = Crypto::simpleEncrypt(serviceKey);
    serviceTicket.timestamp = std::time(nullptr);  
    std::cout << "KDC: Выдан сервисный билет для " << tgt.clientName << " для сервиса " << serviceName << std::endl;
    std::cout << "Зашифрованный сессионный ключ (TGS): " << serviceTicket.sessionKey << std::endl;
   

    return serviceTicket;
}
