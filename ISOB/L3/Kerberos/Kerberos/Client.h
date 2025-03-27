#ifndef CLIENT_H
#define CLIENT_H

#include <string>
#include "KDC.h"
#include "Service.h"

class Client {
public:
    Client(const std::string& name, const std::string& password);

    // Метод, в котором клиент запрашивает у KDC билет и обращается к сервису
    void requestService(Service& service, KDC& kdc);

private:
    std::string clientName;
    std::string clientPassword;
};

#endif 
