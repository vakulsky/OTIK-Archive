//
// Created by Grigory on 20/09/2022.
//

#include "Archiver.h"

using namespace std;

void Zipper::InCompress() {
    char byte[1];  // единичный буфер для считывания одного байта

    file_header header;
    FILE *f;
    FILE *main = fopen((this->real_bin_file).c_str(), "wb");  // файл - архив

    /////
    if(!main){
        cout << "DEBUG | cannot open archive file for writing: " << real_bin_file <<  endl;
    }
    /////

    //todo write header for each file in a loop


    // последовательная запись в архив архивируемых файлов побайтно :
    for (vector<string>::iterator itr = this->files.begin(); itr != this->files.end(); ++itr) {

        header = getInfo(*itr);

        f = fopen((*itr).c_str(), "rb");
        if (!f) {
            cout << *itr << " not found!" << endl;
            break;
        }

        //write header
        fwrite( reinterpret_cast<const unsigned char *>(&header), 1, sizeof( struct file_header ), main );

        //write file data
        while (!feof(f)) {
            if (fread(byte, 1, 1, f) == 1) fwrite(byte, 1, 1, main);
        }
        cout << *itr << " added to archive '" << this->real_bin_file << "'." << endl;
        fclose(f);
    }
    fclose(main);
}


    file_header Zipper::getInfo(const string file)
    {

        /////
        cout << "DEBUG | (info) inside of Zipper::getInfo" <<  endl;
        /////

        file_header header;

        char byte[1];  // единичный буфер для считывания одного байта

        //openning file
        FILE *f = fopen(file.c_str(),"rb");
        if(!f)
            cout << "Can't get info of file " << file << endl;
        else {

            // получаем размер архивируемого файла
            fseek(f, 0, SEEK_END);
            int size = ftell(f);

            /////
            cout << "DEBUG | (info) filesize: " << size <<  endl;
            /////

            string name = Zipper::get_file_name(file);  // получаем имя архивируемого файла

            memset( &header, 0, sizeof( struct file_header ) );
            snprintf( header.signature, SIGNATURE_SZ, "%s", SIGN  );
            snprintf( header.name, NAME_SZ, "%s", name.c_str()  );
            snprintf( header.version, VERSION_SZ, "%s", VERSION );
            snprintf( header.size, SIZE_SZ, "%d", size );

            /////
            cout << "DEBUG | header.name : " << header.name << endl;
            cout << "DEBUG | header.signature : " << header.signature << endl;
            cout << "DEBUG | header.version : " << header.version << endl;
            cout << "DEBUG | header.size : " << header.size << endl;
            /////

        }

        return header;
    }


    void Zipper::OutCompress(const string& binary)
    {
        FILE *bin = fopen(binary.c_str(),"rb");   // открываем архив в режиме чтения

        char byte[1];
        file_header header{};

        fseek(bin, 0, SEEK_SET);

        while (fread(byte,1,1,bin)==1) {

            //going 1 byte back after checking in "while"
            fseek(bin, -1, SEEK_CUR);

            //read header from archive
            memset( &header, 0, sizeof( struct file_header ) );
            fread(header.signature,SIGNATURE_SZ, 1,bin);
            fread(header.name,NAME_SZ,1,bin);
            fread(header.version,VERSION_SZ,1,bin);
            fread(header.size,SIZE_SZ,1,bin);

            fseek(bin, PADDING_SZ, SEEK_CUR);

            /////
            cout << "DEBUG | header.name : " << header.name << endl;
            cout << "DEBUG | header.signature : " << header.signature << endl;
            cout << "DEBUG | header.version : " << header.version << endl;
            cout << "DEBUG | header.size : " << header.size << endl;
            /////

            //read file
            char full_path[255];
            strcpy(full_path,this->path.c_str());
            strcat(full_path,header.name);
            int _sz = atoi(header.size);
            cout<<"--  '"<<header.name<<"' extracted to '"<<this->path<<"' ."<<endl;
            FILE *curr = fopen(full_path,"wb");
            for(int r=1;r<=_sz;r++)
            {
                if(fread(byte,1,1,bin)==1) fwrite(byte,1,1,curr);
            }
            fclose(curr);

        }
        fclose(bin);

    }
