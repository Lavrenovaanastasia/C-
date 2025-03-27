#ifndef CLIENT_H
#define CLIENT_H

#include <string>
#include "KDC.h"
#include "Service.h"

class Client {
public:
    Client(const std::string& name, const std::string& password);

    // �����, � ������� ������ ����������� � KDC ����� � ���������� � �������
    void requestService(Service& service, KDC& kdc);

private:
    std::string clientName;
    std::string clientPassword;
};

#endif 
