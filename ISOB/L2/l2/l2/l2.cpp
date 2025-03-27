#include <iostream>
#include <string>
#include <clocale>
#include <windows.h>

using namespace std;

// Функция для шифрования одного символа
wchar_t encryptChar(wchar_t ch, int key) {
    if (ch >= L'A' && ch <= L'Z') { // Английский верхний регистр
        return (ch - L'A' + key + 26) % 26 + L'A';
    }
    else if (ch >= L'a' && ch <= L'z') { // Английский нижний регистр
        return (ch - L'a' + key + 26) % 26 + L'a';
    }
    else if (ch >= L'А' && ch <= L'Я') { // Русский верхний регистр
        return (ch - L'А' + key + 32) % 32 + L'А';
    }
    else if (ch >= L'а' && ch <= L'я') { // Русский нижний регистр
        return (ch - L'а' + key + 32) % 32 + L'а';
    }
    else if (ch == L'Ё') { // Специальная обработка буквы Ё
        return L'Ё';
    }
    else if (ch == L'ё') { // Специальная обработка ё
        return L'ё';
    }
    else if (ch >= L'0' && ch <= L'9') { // Шифрование цифр
        return (ch - L'0' + key + 10) % 10 + L'0';
    }
    return ch; // Остальные символы (пробелы, знаки препинания) не меняем
}

// Функция для шифрования текста
wstring caesarCipherEncrypt(const wstring& text, int key) {
    wstring encryptedText = L"";
    for (wchar_t ch : text) {
        encryptedText += encryptChar(ch, key);
    }
    return encryptedText;
}

// Функция для дешифрования текста
wstring caesarCipherDecrypt(const wstring& text, int key) {
    return caesarCipherEncrypt(text, -key);
}

int main() {
    setlocale(LC_ALL, "ru_RU.UTF-8"); // Устанавливаем локаль
    SetConsoleOutputCP(65001); // Устанавливаем кодировку UTF-8 в консоли Windows
    SetConsoleCP(65001); // Устанавливаем кодировку ввода

    wstring text;
    int key;

    wcout << L"Введите текст для шифрования: ";
    getline(wcin, text);

    wcout << L"Введите ключ (сдвиг): ";
    wcin >> key;

    wstring encryptedText = caesarCipherEncrypt(text, key);
    wcout << L"Зашифрованный текст: " << encryptedText << endl;

    int decryptKey;
    wcout << L"Введите ключ для дешифрования: ";
    wcin >> decryptKey;

    wstring decryptedText = caesarCipherDecrypt(encryptedText, decryptKey);
    wcout << L"Расшифрованный текст: " << decryptedText << endl;

    return 0;
}
