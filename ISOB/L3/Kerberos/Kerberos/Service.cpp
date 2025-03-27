#include "Service.h"
#include "Crypto.h"
#include <iostream>
#include <ctime>

Service::Service(const std::string& serviceName) : name(serviceName) {}

bool Service::validateTicket(const Ticket& serviceTicket) {
    // ��������� ���������� ����, ���������� � ��������� ������
    std::string decryptedKey = Crypto::simpleDecrypt(serviceTicket.sessionKey);

    std::string expectedSuffix = "_" + name;
    if (decryptedKey.size() >= expectedSuffix.size() &&
        decryptedKey.compare(decryptedKey.size() - expectedSuffix.size(),
            expectedSuffix.size(), expectedSuffix) == 0) {
        // �������� ����� �������� ������ 
        std::time_t now = std::time(nullptr);
        if (difftime(now, serviceTicket.timestamp) < 300) {
            std::cout << "������ " << name << ": ����� ��� "
                << serviceTicket.clientName << " ������������." << std::endl;
            return true;
        }
        else {
            std::cerr << "������ " << name << ": ����� ����." << std::endl;
        }
    }
    else {
        std::cerr << "������ " << name << ": ����� ��������������." << std::endl;
    }
    return false;
}
