//
// Created by Grigory on 18/10/2022.
//

#ifndef OTIK_ARCHIVE_FILEHEADER_H
#define OTIK_ARCHIVE_FILEHEADER_H

#define VERSION "2"
#define SIGN "SVSA"
#define HEADER_SZ 128
#define SIGNATURE_SZ 8
#define NAME_SZ 50
#define VERSION_SZ 8
#define SIZE_SZ 12
#define ALGORITHM_SZ 1
#define PADDING_SZ 41

struct file_header {
    char signature[SIGNATURE_SZ];
    char name[NAME_SZ];
    char version[VERSION_SZ];
    char size[SIZE_SZ];
    char algorithm[ALGORITHM_SZ];
    char padding[PADDING_SZ];

};

#endif //OTIK_ARCHIVE_FILEHEADER_H
