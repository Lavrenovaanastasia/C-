#ifndef KDC_H
#define KDC_H

#include <string>
#include <map>
#include <ctime>

// TGT 
struct Ticket {
    std::string clientName;   
    std::string sessionKey;  
    std::time_t timestamp;   
    std::string longTermKey; 
};

class KDC {
public:
    KDC(const std::string& dbPath);

    bool loadDatabase();

    // ������ TGT (���� ��� ������������ � ������ �����)
    Ticket issueTGT(const std::string& clientName, const std::string& password);

    // TGS
    Ticket issueServiceTicket(const Ticket& tgt, const std::string& serviceName);

private:
    std::string databasePath;
    std::map<std::string, std::string> credentials; // username -> password
    // ��������� ����������� ����� 
    std::string generateSessionKey();
};

#endif 