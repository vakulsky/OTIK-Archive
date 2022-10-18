//
// Created by Grigory on 18/10/2022.
//

#include "Packer.h"

using namespace std;

int Packer::Pack(const string& file, const string& archiveName) {
    char byte[1];  // buffer for reading 1 byte

    file_header header;
    FILE *f;
    FILE *main = fopen((archiveName).c_str(), "wb");  // opening archive


    header = buildHeader(file);

    //open file
    f = fopen((file).c_str(), "rb");
    if (!f) {
        cout << file << " not found!" << endl;
        return 0;
    }

    //add new file to the end of archive file
    fseek(main, 0, SEEK_END);

    //write header
    fwrite( reinterpret_cast<const unsigned char *>(&header), 1, sizeof( struct file_header ), main );

    //write file data
    while (!feof(f)) {
        if (fread(byte, 1, 1, f) == 1) fwrite(byte, 1, 1, main);
    }
    cout << file << " added to archive '" << archiveName << "'." << endl;
    fclose(f);

    fclose(main);

    return atoi(header.size);
}


file_header Packer::buildHeader(const string file)
{
    file_header header;

    char byte[1];

    //openning file
    FILE *f = fopen(file.c_str(),"rb");
    if(!f)
        cout << "Can't get info of file " << file << endl;
    else {

        // get file size
        fseek(f, 0, SEEK_END);
        int size = ftell(f);

        string name = Packer::getFileName(file);  // get file name

        memset( &header, 0, sizeof( struct file_header ) );
        snprintf( header.signature, SIGNATURE_SZ, "%s", SIGN  );
        snprintf( header.name, NAME_SZ, "%s", name.c_str()  );
        snprintf( header.version, VERSION_SZ, "%s", VERSION );
        snprintf( header.size, SIZE_SZ, "%d", size );
    }

    return header;
}


void Packer::Unpack(FILE* archiveFile, file_header& header) {

    char byte[1];

    while (fread(byte, 1, 1, archiveFile) == 1) {

        fseek(archiveFile, PADDING_SZ, SEEK_CUR);

        //extracting file
        char full_path[255];
        strcat(full_path, header.name);
        int _sz = atoi(header.size);
        FILE *curr = fopen(header.name, "wb");

        for (int r = 1; r <= _sz; r++) {
            if (fread(byte, 1, 1, archiveFile) == 1) fwrite(byte, 1, 1, curr);
        }
        fclose(curr);
        cout << "--- " << header.name << " is extracted ---" << endl;

    }
}