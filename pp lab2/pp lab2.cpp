#include <iostream>
#include <locale.h>
#include <fstream>
#include <thread>
#include <vector>
#include <string>

using namespace std;

struct ErrorReturn {
    bool is_error = false;
    string message = ""; // текстовое описание ошибки
};


void task(int a, int b, string result_filename, ErrorReturn *er);
int search_nod(vector<int> dels1, vector<int> dels2);


int main()
{
    setlocale(LC_ALL, "Russian");
    
    // открытие файла
    ifstream input_file("input.txt");
    if (!input_file.is_open()) {
        cout << "Ошибка открытия файла с исходными данными." << endl;
        return 0;
    }

    // чтение данных
    vector<int> input_nums;
    int a, b;
    while (input_file >> a >> b) {
        input_nums.push_back(a);
        input_nums.push_back(b);
    }
    int n = input_nums.size() / 2; // кол-во пар
    input_file.close();

    if (input_nums.empty()) {
        cout << "Ошибка - во входном файле нет задач" << endl;
        return 0;
    }

    
    // имена выходных файлов потоков
    vector<string> output_filenames(n);
    for (int i = 0; i < n; i++) {
        string filename = "result_" + to_string(i) + ".txt";
        output_filenames[i] = filename;
    }


    vector<thread> threads(n);
    vector<ErrorReturn> err_ret(n);
    for (int i = 0; i < n; i++) {
        try {
            threads[i] = thread(task, input_nums[i * 2], input_nums[i * 2 + 1], output_filenames[i], &err_ret[i]);
        }
        catch (const system_error& e) {
            cout << "Ошибка создания потока " << i + 1 << endl;
            for (int j = 0; j < i; j++) {
                threads[j].join();
            }
            return 0;
        }
    }

    bool error_flag = false;
    for (int i = 0; i < n; i++) {
        threads[i].join();
        if (err_ret[i].is_error) {
            error_flag = true;
            cout << err_ret[i].message << endl;
        }
    }
    if (error_flag) { return 0; }


    // считывание данных
    for (int i = 0; i < n; i++) {
        int num1 = input_nums[i * 2];
        int num2 = input_nums[i * 2 + 1];
        ifstream res_file(output_filenames[i]);
        if (!res_file.is_open()) {
            cout << "Ошибка открытия основным потоком файла с результатом потока " << i << endl;
            continue;
        }
        int result;
        if (!(res_file >> result)) {
            cout << "Ошибка чтения основным потоком результата потока " << i << endl;
            res_file.close();
            continue;
        }

        cout << "НОД чисел " << num1 << " и " << num2 << ": " << result << endl;
        res_file.close();
    }

    return 0;
}


void task(int a, int b, string result_filename, ErrorReturn* er)
{
    if (a < 0) { a *= -1; }
    if (b < 0) { b *= -1; }

    vector<int> d1; // делители а
    vector<int> d2; // делители b

    for (int i = 1; i <= a / 2; i++) {
        if (a % i == 0) {
            d1.push_back(i);
        }
    }
    for (int i = 1; i <= b / 2; i++) {
        if (b % i == 0) {
            d2.push_back(i);
        }
    }

    d1.push_back(a);
    d2.push_back(b);

    ofstream res_file(result_filename);
    if (!res_file.is_open()) {
        er->is_error = true;
        er->message = "Ошибка открытия выходного файла потока";
        return;
    }

    int nod = search_nod(d1, d2);
    //особые случаи
    if (a == 0 && b == 0) { nod = 0; }
    else if (a == 0) { nod = b; }
    else if (b == 0) { nod = a; }

    res_file << nod;
    res_file.close();
}


int search_nod(vector<int> dels1, vector<int> dels2)
{
    for (int i = dels1.size() - 1; i >= 0; i--) {
        for (int j = dels2.size() - 1; j >= 0; j--) {
            if (dels1[i] == dels2[j]) { return dels1[i]; }
            else if (dels2[j] < dels1[i]) { break; }
        }
    }
    return 1;
}
