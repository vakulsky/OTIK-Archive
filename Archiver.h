//
// Created by Grigory on 20/09/2022.
//

#ifndef OTIK_ARCHIVE_ARCHIVER_H
#define OTIK_ARCHIVE_ARCHIVER_H

#include <iostream>
#include <cstring>
#include <vector>

#include "functions.h"

#define VERSION 1

using namespace std;


struct file_header {
     filename;
    unsigned int filesize;
    unsigned int version = VERSION;

};

class Zipper {

private:
    vector<string> files;   // набор файлов (-files)
    vector<string> headers; //headers to be written before every file in archive
    string path;            // путь (-path)
    string real_bin_file;   // имя выходного файла-архива( используется при архивации )
public:
    Zipper(vector<string> &vec, string p) {
        if (vec.size() > 0) files.assign(vec.begin(), vec.end());
        path = p + "\\";
        real_bin_file = path + "binary.zipper";
    }


    void getInfo();   // Метод для получения информации о файлах на этапе архивации
    void InCompress();   // Архивация данных
    void OutCompress(string binary);   // Распаковка данных ( binary - путь до архива )

// Статический метод для выделения имени файла из полного пути.
// Используется для внутренних нужд.
    static string get_file_name(string fn) {
        return fn.substr(fn.find_last_of("\\") + 1, fn.size());
    }

};



#endif //OTIK_ARCHIVE_ARCHIVER_H
