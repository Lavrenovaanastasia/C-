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
        std::cerr << "�� ������� ������� ���� ������: " << databasePath << std::endl;
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
    // ���������� ��������� ���������� ���� � ���� ������
    int num = rand() % 1000000;
    return "SK" + std::to_string(num);
}

Ticket KDC::issueTGT(const std::string& clientName, const std::string& password) {
    Ticket ticket;
    //���������� �� ������������ � ����
    if (credentials.find(clientName) == credentials.end()) {
        std::cerr << "������������ " << clientName << " �� ������." << std::endl;
        return ticket;
    }
    // ������������ ������
    if (credentials[clientName] != password) {
        std::cerr << "�������� ������ ��� " << clientName << std::endl;
        return ticket;
    }
    // ������ TGT
    ticket.clientName = clientName;
    ticket.longTermKey = password;  // �������������� ����(������)
    std::string plainKey = generateSessionKey();

    // ������� ���������� ���� ����� ���������
    ticket.sessionKey = Crypto::simpleEncrypt(plainKey);
    ticket.timestamp = std::time(nullptr);  

    std::cout << "KDC: ����� TGT ��� " << clientName << std::endl;
    std::cout << "�������������� ���� (������): " << ticket.longTermKey << std::endl;
    std::cout << "������������� ���������� ���� (TGT): " << ticket.sessionKey << std::endl;

    return ticket;
}

Ticket KDC::issueServiceTicket(const Ticket& tgt, const std::string& serviceName) {
    Ticket serviceTicket;
    serviceTicket.clientName = tgt.clientName;
    // ��������� TGT, ����� �������� �������� ���������� ����
    std::string decryptedKey = Crypto::simpleDecrypt(tgt.sessionKey);

    // ������������ ���������� ���� ��� ����������� �������
    std::string serviceKey = decryptedKey + "_" + serviceName;

    // ������� ���������� ���� ��� �������� �������
    serviceTicket.sessionKey = Crypto::simpleEncrypt(serviceKey);
    serviceTicket.timestamp = std::time(nullptr);  
    std::cout << "KDC: ����� ��������� ����� ��� " << tgt.clientName << " ��� ������� " << serviceName << std::endl;
    std::cout << "������������� ���������� ���� (TGS): " << serviceTicket.sessionKey << std::endl;
   

    return serviceTicket;
}
