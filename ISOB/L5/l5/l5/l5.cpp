#include <iostream>
#include <cstring>
#include <cstdlib>

void check_buffer_overflow(char* input) {
    const size_t buffer_size = 50;
    char buffer[buffer_size];

    std::cout << "Размер буфера: " << buffer_size << " байт." << std::endl;

    //  переполнения буфера
    size_t input_length = strlen(input);
    if (input_length >= buffer_size) {
        std::cout << "Входные данные слишком длинные для буфера. Будет выполнено обрезание строки." << std::endl;

        strncpy_s(buffer, buffer_size, input, buffer_size - 1);
        buffer[buffer_size - 1] = '\0'; //  завершение строки

        size_t truncated_length = input_length - (buffer_size - 1);
        std::cout << "Строка была обрезана на " << truncated_length << " символов." << std::endl;
    }
    else {
        std::cout << "Строка подходит по размеру, копируем в буфер..." << std::endl;
        strcpy_s(buffer, buffer_size, input);
    }

    std::cout << "Результат после обработки: " << buffer << std::endl;
}

void demonstrate_buffer_overflow(const char* input) {
    std::cout << "Демонстрация защиты от переполнения буфера" << std::endl;

    std::cout << "Ввод: " << input << std::endl;

    check_buffer_overflow(const_cast<char*>(input));
       
    std::cout << "Демонстрация завершена." << std::endl;
}

int main() {
    setlocale(LC_ALL, "Russian");

    const char* safe_input = "Это безопасный ввод";
    std::cout << "-------------Тестирование с безопасным вводом--------------------------" << std::endl;
    demonstrate_buffer_overflow(safe_input);

    const char* unsafe_input = "Это строка слишком длинная для нашего буфера и вызовет переполнение!";
    std::cout << "\n-------------Тестирование с опасным вводом----------------------------" << std::endl;
    demonstrate_buffer_overflow(unsafe_input);

    return 0;
}
