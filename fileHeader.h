//
// Created by Grigory on 18/10/2022.
//

#ifndef OTIK_ARCHIVE_FILEHEADER_H
#define OTIK_ARCHIVE_FILEHEADER_H

#define VERSION "3"
#define SIGN "SVSA"
#define HEADER_SZ 128
#define SIGNATURE_SZ 8
#define NAME_SZ 50
#define VERSION_SZ 8
#define FILESIZE_SZ 12
#define ALGORITHM_SZ 5
#define ERRORCORR_SZ 5
#define HEADERPROTECT_SZ 5
#define DATASIZE_SZ 10
#define PADDING_SZ 25

struct file_header {
    char signature[SIGNATURE_SZ];
    char name[NAME_SZ];
    char version[VERSION_SZ];
    char file_size[FILESIZE_SZ];
    char algorithm[ALGORITHM_SZ];
    char errorcorr[ERRORCORR_SZ];
    char header_protection_flag[HEADERPROTECT_SZ];
    char data_size[DATASIZE_SZ];
    char padding[PADDING_SZ];

};

#endif //OTIK_ARCHIVE_FILEHEADER_H
