#include "Client.h"
#include <iostream>

Client::Client(const std::string& name, const std::string& password)
    : clientName(name), clientPassword(password) {
}

void Client::requestService(Service& service, KDC& kdc) {
    std::cout << "Клиент " << clientName << " запрашивает TGT." << std::endl;
    Ticket tgt = kdc.issueTGT(clientName, clientPassword);
    if (tgt.clientName.empty()) {
        std::cerr << "Аутентификация клиента не удалась." << std::endl;
        return;
    }
    std::cout << "Клиент " << clientName << " получил TGT." << std::endl;

    std::cout << "Клиент " << clientName << " запрашивает сервисный билет для сервиса "
        << service.getName() << std::endl;
    Ticket serviceTicket = kdc.issueServiceTicket(tgt, service.getName());

    std::cout << "Клиент " << clientName << " отправляет билет в сервис "
        << service.getName() << std::endl;
    if (service.validateTicket(serviceTicket)) {
        std::cout << "Клиент " << clientName
            << " успешно аутентифицирован в сервисе "
            << service.getName() << std::endl;
    }
    else {
        std::cerr << "Аутентификация в сервисе не удалась." << std::endl;
    }
}
