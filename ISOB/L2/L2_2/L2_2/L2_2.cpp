#include <iostream>
#include <string>
#include <clocale>
#include <windows.h>

using namespace std;

// Функция для шифрования символа с учетом русского и английского алфавитов
wchar_t vigenereEncryptChar(wchar_t ch, wchar_t keyChar, bool decrypt = false) {
    int shift = decrypt ? -1 : 1; // Если дешифруем, сдвиг идет в обратную сторону

    if (ch >= L'A' && ch <= L'Z') { // Английский верхний регистр
        return (ch - L'A' + shift * (keyChar - L'A') + 26) % 26 + L'A';
    }
    else if (ch >= L'a' && ch <= L'z') { // Английский нижний регистр
        return (ch - L'a' + shift * (keyChar - L'a') + 26) % 26 + L'a';
    }
    else if (ch >= L'А' && ch <= L'Я') { // Русский верхний регистр
        return (ch - L'А' + shift * (keyChar - L'А') + 32) % 32 + L'А';
    }
    else if (ch >= L'а' && ch <= L'я') { // Русский нижний регистр
        return (ch - L'а' + shift * (keyChar - L'а') + 32) % 32 + L'а';
    }
    else if (ch == L'Ё') { // Обработка буквы Ё
        return ch;  // Не изменяем символ Ё
    }
    else if (ch == L'ё') { // Обработка буквы ё
        return ch;  // Не изменяем символ ё
    }
    else if (ch >= L'0' && ch <= L'9'&& decrypt == false) { // Шифрование цифр
        return (ch - L'0' + shift * (keyChar - L'0') + 10) % 10 + L'0' -1;
    }
    else if (ch >= L'0' && ch <= L'9' && decrypt == true) { // Шифрование цифр
        return (ch - L'0' + shift * (keyChar - L'0') + 10) % 10 + L'0' + 1;
    }
    return ch; // Остальные символы (пробелы, знаки препинания) не меняем
}

// Функция для шифрования текста с использованием шифра Виженера
wstring vigenereEncrypt(const wstring& text, const wstring& key) {
    wstring encryptedText = L"";
    int keyIndex = 0;
    for (wchar_t ch : text) {
        // Для каждого символа текста применяем соответствующий символ из ключа
        encryptedText += vigenereEncryptChar(ch, key[keyIndex], false)+1;
        keyIndex = (keyIndex + 1) % key.length(); // Повторяем ключ, если текст длиннее ключа
    }
    return encryptedText;
}

// Функция для дешифрования текста с использованием шифра Виженера
wstring vigenereDecrypt(const wstring& text, const wstring& key) {
    wstring decryptedText = L"";
    int keyIndex = 0;
    for (wchar_t ch : text) {
        // Для каждого символа текста применяем соответствующий символ из ключа для дешифрования
        decryptedText += vigenereEncryptChar(ch, key[keyIndex], true)-1; // Для дешифрования передаем параметр true
        keyIndex = (keyIndex + 1) % key.length(); // Повторяем ключ
    }
    return decryptedText;
}

int main() {
    setlocale(LC_ALL, "ru_RU.UTF-8");
    SetConsoleOutputCP(65001);
    SetConsoleCP(65001);

    wstring text, key;

    wcout << L"Введите текст для шифрования: ";
    getline(wcin, text);

    wcout << L"Введите ключ: ";
    getline(wcin, key);

    wstring encryptedText = vigenereEncrypt(text, key);
    wcout << L"Зашифрованный текст: " << encryptedText << endl;

    wstring decryptedText = vigenereDecrypt(encryptedText, key);
    wcout << L"Расшифрованный текст: " << decryptedText << endl;

    return 0;
}
