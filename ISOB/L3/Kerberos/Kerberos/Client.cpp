#include "Client.h"
#include <iostream>

Client::Client(const std::string& name, const std::string& password)
    : clientName(name), clientPassword(password) {
}

void Client::requestService(Service& service, KDC& kdc) {
    std::cout << "������ " << clientName << " ����������� TGT." << std::endl;
    Ticket tgt = kdc.issueTGT(clientName, clientPassword);
    if (tgt.clientName.empty()) {
        std::cerr << "�������������� ������� �� �������." << std::endl;
        return;
    }
    std::cout << "������ " << clientName << " ������� TGT." << std::endl;

    std::cout << "������ " << clientName << " ����������� ��������� ����� ��� ������� "
        << service.getName() << std::endl;
    Ticket serviceTicket = kdc.issueServiceTicket(tgt, service.getName());

    std::cout << "������ " << clientName << " ���������� ����� � ������ "
        << service.getName() << std::endl;
    if (service.validateTicket(serviceTicket)) {
        std::cout << "������ " << clientName
            << " ������� ���������������� � ������� "
            << service.getName() << std::endl;
    }
    else {
        std::cerr << "�������������� � ������� �� �������." << std::endl;
    }
}
