#include <windows.h> 
#include <string> 
#include <chrono> 
#include <iostream> 

#define ll long long 
#define ld long double 

using namespace std; 
using namespace chrono; 

// Константный размер записи в байтах
const ll RECORD_SIZE = 256;

// Структура для хранения одной записи в базе данных
struct Record
{
    ll id; // Идентификатор записи
    char name[248]; // Имя записи
};

// Класс для работы с базой данных в файле
class Database
{
public:
    Database(const string& filename, ll recordCount) // Конструктор класса
        : filename_(filename), recordCount_(recordCount)
    {   // Открытие файла для чтения/записи
        hFile_ = CreateFileA(
            filename_.c_str(),
            GENERIC_READ | GENERIC_WRITE, // Флаги доступа (чтение и запись)
            0, // Нет совместного доступа
            NULL, // Нет атрибутов безопасности
            OPEN_ALWAYS, // Открывать файл всегда, создавая его, если он не существует
            FILE_ATTRIBUTE_NORMAL, // Обычный файл
            NULL); // Нет шаблона файла

        // Определение размера файла
        LARGE_INTEGER fileSize;
        fileSize.QuadPart = recordCount_ * RECORD_SIZE;
        SetFilePointerEx(hFile_, fileSize, NULL, FILE_BEGIN); // Установка указателя файла на конец
        SetEndOfFile(hFile_); // Установка конца файла в соответствии с указателем

        // Создание сопоставления файла в памяти
        hMap_ = CreateFileMappingA(
            hFile_,
            NULL, // Нет атрибутов безопасности
            PAGE_READWRITE, // Флаги защиты (чтение и запись)
            0,
            0,
            NULL); // Нет имени объекта

        // Создание представления сопоставленного файла
        data_ = static_cast<char*>(MapViewOfFile(
            hMap_,
            FILE_MAP_ALL_ACCESS, // Флаги доступа (полный доступ)
            0,
            0,
            0)); // Нет смещения и размера представления
    }

    // Деструктор класса
    ~Database()
    {
        if (data_)
            UnmapViewOfFile(data_); // Отмена представления сопоставленного файла
        if (hMap_)
            CloseHandle(hMap_); // Закрытие сопоставления файла
        if (hFile_ != INVALID_HANDLE_VALUE)
            CloseHandle(hFile_); // Закрытие файла, если он был успешно открыт
    }

    // Запись записи в базу данных
    void writeRecord(ll index, const Record& record)
    {
        char* ptr = data_ + index * RECORD_SIZE; // Указатель на начало записи
        memcpy(ptr, &record, sizeof(Record)); // Копирование данных записи в указанную область
    }

    // Чтение записи из базы данных
    Record readRecord(ll index) const
    {
        Record record; // Локальная переменная для хранения прочитанной записи
        const char* ptr = data_ + index * RECORD_SIZE; // Указатель на начало записи
        memcpy(&record, ptr, sizeof(Record)); // Копирование данных из указанной области в локальную переменную
        return record; // Возвращение прочитанной записи
    }

private:
    string filename_; // Имя файла базы данных
    ll recordCount_; // Количество записей в базе данных
    HANDLE hFile_ = INVALID_HANDLE_VALUE; // Дескриптор файла базы данных
    HANDLE hMap_ = NULL; // Дескриптор сопоставления файла
    char* data_ = NULL; // Указатель на начало представления сопоставленного файла
};


class SynchronousDatabase
{
public:
    SynchronousDatabase(const string& filename, ll recordCount) // Конструктор класса
        : filename_(filename), recordCount_(recordCount)
    {
        // Открытие файла для чтения/записи
        hFile_ = CreateFileA(
            filename_.c_str(),
            GENERIC_READ | GENERIC_WRITE, // Флаги доступа (чтение и запись)
            0, // Нет совместного доступа
            NULL, // Нет атрибутов безопасности
            OPEN_ALWAYS, // Открывать файл всегда, создавая его, если он не существует
            FILE_ATTRIBUTE_NORMAL, // Обычный файл
            NULL); // Нет шаблона файла

        // Определение размера файла
        LARGE_INTEGER fileSize;
        fileSize.QuadPart = recordCount_ * RECORD_SIZE;
        SetFilePointerEx(hFile_, fileSize, NULL, FILE_BEGIN); // Установка указателя файла на конец
        SetEndOfFile(hFile_); // Установка конца файла в соответствии с указателем
    }

    // Деструктор класса
    ~SynchronousDatabase()
    {
        if (hFile_ != INVALID_HANDLE_VALUE)
            CloseHandle(hFile_); // Закрытие файла, если он был успешно открыт
    }

    // Запись записи в базу данных
    void writeRecord(ll index, const Record& record)
    {
        // Установка указателя файла на начало записи
        LARGE_INTEGER offset;
        offset.QuadPart = index * RECORD_SIZE;
        SetFilePointerEx(hFile_, offset, NULL, FILE_BEGIN);

        // Запись данных записи в файл
        WriteFile(hFile_, &record, sizeof(Record), NULL, NULL);
    }

    // Чтение записи из базы данных
    Record readRecord(ll index) const
    {
        Record record; // Локальная переменная для хранения прочитанной записи

        // Установка указателя файла на начало записи
        LARGE_INTEGER offset;
        offset.QuadPart = index * RECORD_SIZE;
        SetFilePointerEx(hFile_, offset, NULL, FILE_BEGIN);

        // Чтение данных записи из файла
        ReadFile(hFile_, &record, sizeof(Record), NULL, NULL);

        return record; // Возвращение прочитанной записи
    }

private:
    string filename_; // Имя файла базы данных
    ll recordCount_; // Количество записей в базе данных
    HANDLE hFile_ = INVALID_HANDLE_VALUE; // Дескриптор файла базы данных
};

// Функция для создания новой записи
Record createRecord(ll id, const string& name)
{
    Record record; // Локальная переменная для хранения создаваемой записи

    // Заполнение полей записи
    record.id = id;
    memset(record.name, 0, sizeof(record.name)); // Очистка поля имени нулями
    strncpy_s(record.name, name.c_str(), sizeof(record.name) - 1); // Копирование имени в поле записи

    return record; // Возвращение созданной записи
}


int main()
{
    setlocale(LC_ALL, "Russian");
    system("cls");

   
    const string filename = "database.dat";
    // Количество записей в базе данных
    const ll recordCount = 100000000;
    // Создание объекта класса Database для работы с базой данных в памяти
    Database db(filename, recordCount);
    // Фиксация времени начала записи
    auto start = high_resolution_clock::now();
    // Цикл записи записей в базу данных
    for (ll i = 0; i < recordCount; ++i)
    {
        // Создание новой записи с заданным идентификатором и именем
        db.writeRecord(i, createRecord(static_cast<ll>(i), "Name_" + to_string(i)));
    }

    // Фиксация времени окончания записи
    auto end = high_resolution_clock::now();
    // Подсчет времени записи
    duration<double> duration = end - start;
    cout << "Время записи с использованием памяти : " << duration.count() << " секунд." << endl;
    // Фиксация времени начала чтения
    start = high_resolution_clock::now();
    // Цикл чтения записей из базы данных
    for (ll i = 0; i < recordCount; ++i)
    {
        // Чтение записи из базы данных
        Record r = db.readRecord(i);
    }

    // Фиксация времени окончания чтения
    end = high_resolution_clock::now();
    // Подсчет времени чтения
    duration = end - start;
    cout << "Время чтения с использованием памяти : " << duration.count() << " секунд." << endl;
    // Создание объекта класса SynchronousDatabase для синхронной работы с базой данных
    SynchronousDatabase dbsync(filename, recordCount);
    // Фиксация времени начала синхронной записи
    start = high_resolution_clock::now();
    // Цикл синхронной записи записей в базу данных
    for (ll i = 0; i < recordCount; ++i)
    {
        // Создание новой записи с заданным идентификатором и именем
        dbsync.writeRecord(i, createRecord(static_cast<ll>(i), "Name_" + to_string(i)));
    }

    // Фиксация времени окончания синхронной записи
    end = high_resolution_clock::now();
    // Подсчет времени синхронной записи
    duration = end - start;
    cout << "Время синхронной записи: " << duration.count() << " секунд." << endl;
    // Фиксация времени начала синхронного чтения
    start = high_resolution_clock::now();
    // Цикл синхронного чтения записей из базы данных
    for (ll i = 0; i < recordCount; ++i)
    {
        // Чтение записи из базы данных
        Record r = dbsync.readRecord(i);
    }

    // Фиксация времени окончания синхронного чтения
    end = high_resolution_clock::now();
    // Подсчет времени синхронного чтения
    duration = end - start;
    cout << "Время синхронного чтения: " << duration.count() << " секунд." << endl;
    return 0;
}
