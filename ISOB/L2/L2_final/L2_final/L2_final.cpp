#include <iostream>
#include <string>
#include <clocale>
#include <windows.h>

using namespace std;

wchar_t caesarEncryptChar(wchar_t ch, int key) {
    if (ch >= L'A' && ch <= L'Z') {
        return (ch - L'A' + key + 26) % 26 + L'A';
    }
    else if (ch >= L'a' && ch <= L'z') { 
        return (ch - L'a' + key + 26) % 26 + L'a';
    }
    else if (ch >= L'А' && ch <= L'Я') { 
        return (ch - L'А' + key + 32) % 32 + L'А';
    }
    else if (ch >= L'а' && ch <= L'я') { 
        return (ch - L'а' + key + 32) % 32 + L'а';
    }
    else if (ch == L'Ё') { 
        return L'Ё';
    }
    else if (ch == L'ё') { 
        return L'ё';
    }
    else if (ch >= L'0' && ch <= L'9') { 
        return (ch - L'0' + key + 10) % 10 + L'0';
    }
    return ch; 
}
// Функция для шифрования текста шифром Цезаря
wstring caesarCipherEncrypt(const wstring& text, int key) {
    wstring encryptedText;
    for (wchar_t ch : text) {
        encryptedText += caesarEncryptChar(ch, key);
    }
    return encryptedText;
}
// Функция для дешифрования текста шифром Цезаря
wstring caesarCipherDecrypt(const wstring& text, int key) {
    return caesarCipherEncrypt(text, -key);
}

wchar_t vigenereEncryptChar(wchar_t ch, wchar_t keyChar, bool decrypt = false) {
    int shift = decrypt ? -1 : 1; // Если дешифруем, сдвиг в обратную сторону

    if (ch >= L'A' && ch <= L'Z') { 
        return (ch - L'A' + shift * (keyChar - L'A') + 26) % 26 + L'A';
    }
    else if (ch >= L'a' && ch <= L'z') { 
        return (ch - L'a' + shift * (keyChar - L'a') + 26) % 26 + L'a';
    }
    else if (ch >= L'А' && ch <= L'Я') { 
        return (ch - L'А' + shift * (keyChar - L'А') + 32) % 32 + L'А';
    }
    else if (ch >= L'а' && ch <= L'я') { 
        return (ch - L'а' + shift * (keyChar - L'а') + 32) % 32 + L'а';
    }
    else if (ch == L'Ё') { 
        return ch;
    }
    else if (ch == L'ё') { 
        return ch;
    }
    else if (ch >= L'0' && ch <= L'9' && decrypt == false) { // при шифровании
        return (ch - L'0' + shift * (keyChar - L'0') + 10) % 10 + L'0' - 1;
    }
    else if (ch >= L'0' && ch <= L'9' && decrypt == true) { // при дешифровании
        return (ch - L'0' + shift * (keyChar - L'0') + 10) % 10 + L'0' + 1;
    }
    return ch; 
}

// Функция для шифрования текста шифром Виженера
wstring vigenereEncrypt(const wstring& text, const wstring& key) {
    wstring encryptedText;
    int keyIndex = 0;
    for (wchar_t ch : text) {
        encryptedText += vigenereEncryptChar(ch, key[keyIndex], false) + 1;
        keyIndex = (keyIndex + 1) % key.length();
    }
    return encryptedText;
}

// Функция для дешифрования текста шифром Виженера
wstring vigenereDecrypt(const wstring& text, const wstring& key) {
    wstring decryptedText;
    int keyIndex = 0;
    for (wchar_t ch : text) {
        decryptedText += vigenereEncryptChar(ch, key[keyIndex], true) - 1;
        keyIndex = (keyIndex + 1) % key.length();
    }
    return decryptedText;
}

int main() {
    setlocale(LC_ALL, "ru_RU.UTF-8");
    SetConsoleOutputCP(65001);
    SetConsoleCP(65001);

    bool repeat = true;
    while (repeat) {
        int choice;
        wcout << L"\nВыберите вариант шифрования:\n";
        wcout << L"1 - Шифр Цезаря\n";
        wcout << L"2 - Шифр Виженера\n";
        wcout << L"Ваш выбор: ";
        wcin >> choice;
        wcin.ignore(); // удаляем символ новой строки из буфера

        if (choice == 1) {
            // Режим шифрования Цезаря
            wstring text;
            int key;

            wcout << L"Введите текст для шифрования: ";
            getline(wcin, text);

            wcout << L"Введите ключ (сдвиг): ";
            wcin >> key;
            wcin.ignore();

            wstring encryptedText = caesarCipherEncrypt(text, key);
            wcout << L"Зашифрованный текст: " << encryptedText << endl;

            int decryptKey;
            wcout << L"Введите ключ для дешифрования: ";
            wcin >> decryptKey;
            wcin.ignore();
            wstring decryptedText = caesarCipherDecrypt(encryptedText, decryptKey);
            wcout << L"Расшифрованный текст: " << decryptedText << endl;
        }
        else if (choice == 2) {
            // Режим шифрования Виженера
            wstring text, key;

            wcout << L"Введите текст для шифрования: ";
            getline(wcin, text);

            wcout << L"Введите ключ: ";
            getline(wcin, key);

            wstring encryptedText = vigenereEncrypt(text, key);
            wcout << L"Зашифрованный текст: " << encryptedText << endl;

            wstring decryptedText = vigenereDecrypt(encryptedText, key);
            wcout << L"Расшифрованный текст: " << decryptedText << endl;
        }
        else {
            wcout << L"Неверный выбор!" << endl;
        }
        wcout << L"\nХотите выполнить операцию ещё раз? (Y/N): ";
        wchar_t answer;
        wcin >> answer;
        wcin.ignore();

        if (answer == L'N' || answer == L'n') {
            repeat = false;
        }
    }
    return 0;
}
