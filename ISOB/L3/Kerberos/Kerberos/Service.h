#ifndef SERVICE_H
#define SERVICE_H

#include <string>
#include "KDC.h"

class Service {
public:
    Service(const std::string& serviceName);

    // ����� �������� ����������� ���������� ������
    bool validateTicket(const Ticket& serviceTicket);

    // ������ ��� ����� �������
    std::string getName() const { return name; }

private:
    std::string name;
};

#endif 
