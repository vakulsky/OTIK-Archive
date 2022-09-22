//
// Created by Grigory on 20/09/2022.
//

#ifndef OTIK_ARCHIVE_ARCHIVER_H
#define OTIK_ARCHIVE_ARCHIVER_H

#include <iostream>
#include <cstring>
#include <vector>

#include "functions.h"

#define VERSION "1"
#define SIGN "SIGN"
#define HEADER_SZ 128
#define SIGNATURE_SZ 8
#define NAME_SZ 50
#define VERSION_SZ 8
#define SIZE_SZ 12
#define PADDING_SZ 50


using namespace std;


struct file_header {
    char signature[SIGNATURE_SZ];
    char name[NAME_SZ];
    char version[VERSION_SZ];
    char size[SIZE_SZ];
    char padding[PADDING_SZ];

};

class Zipper {

private:
    vector<string> files; //headers to be written before every file in archive
    string path;            // путь (-path)
    string real_bin_file;   // имя выходного файла-архива( используется при архивации )
public:
    Zipper(vector<string> &vec, string p) {
        if (vec.size() > 0) files.assign(vec.begin(), vec.end());
        real_bin_file = p;
    }


    file_header getInfo(const string file);   // Метод для получения информации о файлах на этапе архивации
    void InCompress();   // Архивация данных
    void OutCompress(const string& binary);   // Распаковка данных ( binary - путь до архива )

// Статический метод для выделения имени файла из полного пути.
// Используется для внутренних нужд.
    static string get_file_name(string fn) {
        return fn.substr(fn.find_last_of("\\") + 1, fn.size());
    }

};



#endif //OTIK_ARCHIVE_ARCHIVER_H
