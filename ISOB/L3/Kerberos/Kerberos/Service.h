#ifndef SERVICE_H
#define SERVICE_H

#include <string>
#include "KDC.h"

class Service {
public:
    Service(const std::string& serviceName);

    // Метод проверки полученного сервисного билета
    bool validateTicket(const Ticket& serviceTicket);

    // Геттер для имени сервиса
    std::string getName() const { return name; }

private:
    std::string name;
};

#endif 
