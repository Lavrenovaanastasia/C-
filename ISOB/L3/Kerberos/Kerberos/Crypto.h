#ifndef CRYPTO_H
#define CRYPTO_H

#include <string>

class Crypto {
public:
    
    static std::string simpleEncrypt(const std::string& data, int shift = 3) {
        std::string result;
        for (char c : data) {
            result += static_cast<char>(c + shift);
        }
        return result;
    }

    static std::string simpleDecrypt(const std::string& data, int shift = 3) {
        std::string result;
        for (char c : data) {
            result += static_cast<char>(c - shift);
        }
        return result;
    }
};

#endif 
