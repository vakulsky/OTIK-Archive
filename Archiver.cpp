//
// Created by Grigory on 20/09/2022.
//

#include "Archiver.h"

using namespace std;

void Archiver::Compress() {
    char byte[1];  // buffer for reading 1 byte

    file_header header;
    FILE *f;
    FILE *main = fopen((this->archive_file).c_str(), "wb");  // opening archive

    /////
    if(!main){
        cout << "DEBUG | cannot open archive file for writing: " << archive_file << endl;
    }
    /////


    // writing each file and header to archive
    for (vector<string>::iterator itr = this->files.begin(); itr != this->files.end(); ++itr) {

        header = buildHeader(*itr);

        //open file
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
        cout << *itr << " added to archive '" << this->archive_file << "'." << endl;
        fclose(f);
    }
    fclose(main);
}


    file_header Archiver::buildHeader(const string file)
    {

        /////
        cout << "DEBUG | (info) inside of Archiver::buildHeader" <<  endl;
        /////

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

            /////
            cout << "DEBUG | (info) filesize: " << size <<  endl;
            /////

            string name = Archiver::get_file_name(file);  // get file name

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


    void Archiver::Extract(const string& archive_file)
    {
        FILE *arc = fopen(archive_file.c_str(), "rb");   // open archive file
        if(!arc) {
            cout << "Can't open archive file " << archive_file << endl;
        }
        else {

            char byte[1];
            file_header header{};

            fseek(arc, 0, SEEK_SET);

            while (fread(byte, 1, 1, arc) == 1) {

                //going 1 byte back after checking in "while"
                fseek(arc, -1, SEEK_CUR);

                //read header from archive
                memset(&header, 0, sizeof(struct file_header));
                fread(header.signature, SIGNATURE_SZ, 1, arc);
                fread(header.name, NAME_SZ, 1, arc);
                fread(header.version, VERSION_SZ, 1, arc);
                fread(header.size, SIZE_SZ, 1, arc);

                if (strcmp(header.signature, SIGN) != 0) {
                    cout << "Error: Signature mismatch!" << endl;
                    break;
                }

                if (strcmp(header.version, VERSION) != 0) {
                    cout << "Error: Incompatible version!" << endl;
                    break;
                }

                fseek(arc, PADDING_SZ, SEEK_CUR);   //going through padding without reading

                /////
                cout << "DEBUG | header.name : " << header.name << endl;
                cout << "DEBUG | header.signature : " << header.signature << endl;
                cout << "DEBUG | header.version : " << header.version << endl;
                cout << "DEBUG | header.size : " << header.size << endl;
                /////

                //extracting file
                char full_path[255];
                strcat(full_path, header.name);
                int _sz = atoi(header.size);
                FILE *curr = fopen(header.name, "wb");

                /////
                if (!curr)
                    cout << "Can't get info of file " << curr << endl;
                /////

                for (int r = 1; r <= _sz; r++) {
                    if (fread(byte, 1, 1, arc) == 1) fwrite(byte, 1, 1, curr);
                }
                fclose(curr);
                cout << "--- " << header.name << " is extracted ---" << endl;

            }
            fclose(arc);
        }

    }
