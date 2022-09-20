//
// Created by Grigory on 20/09/2022.
//

#ifndef OTIK_ARCHIVE_ARCHIVE_H
#define OTIK_ARCHIVE_ARCHIVE_H

#include<cstdio>
#include<cstring>
#include <cstdlib>

const int PD = 128;
const char* SIGN = "ZGV";
const char* VERSION = "1";

struct tar_header{
    char signature[8];
    char name[50];
    char version[8];
    char size[12];
    char padding[22];
};

void fexpand(FILE* f, size_t amount, int value){
    while( amount-- ){
        fputc( value, f );
    }
}

void tar_add(FILE* tar_file, const char* file, const char* internal_name){
    //Get current position; round to a multiple of 512 if we aren't there already
    size_t index = ftell( tar_file );
    size_t offset = index % PD;
    if( offset != 0 ){
        fexpand( tar_file, PD - offset, 0);
    }
    //Store the index for the header to return to later
    index = ftell( tar_file );
    //Write some space for our header
    fexpand( tar_file, sizeof(struct tar_header), 0 );
    //Write the input file to the tar file
    FILE* input = fopen( file, "rb" );
    if( input == NULL ){
        fprintf( stderr, "Failed to open %s for reading\n", file);
        return;
    }
    //Copy the file content to the tar file
    while( !feof(input) ){
        char buffer[2000];
        size_t read = fread( buffer, 1, 2000, input );
        fwrite( buffer, 1, read, tar_file);
    }
    //Get the end to calculate the size of the file
    size_t end = ftell( tar_file );
    //Round the file size to a multiple of 512 bytes
    offset = end % PD;
    if( end != 0 ){
        fexpand( tar_file, PD - offset, 0);
    }
    //Fill out a new tar header
    struct tar_header header{};
    memset( &header, 0, sizeof( struct tar_header ) );
    snprintf( header.signature, 8, "%s", SIGN  );
    snprintf( header.name, 100, "%s", internal_name  );
    snprintf( header.version, 8, "%s", VERSION );
    snprintf( header.size, 12, "%011llu", end - PD - index );


    //Calculate the checksum
    size_t checksum = 0;
    int i;
    const auto* bytes = reinterpret_cast<const unsigned char *>(&header);

    //Save the new end to return to after writing the header
    end = ftell(tar_file);

    //Write the header
    fseek( tar_file, index, SEEK_SET );
    fwrite( bytes, 1, sizeof( struct tar_header ), tar_file );

    //Return to the end
    fseek( tar_file, end, SEEK_SET );
    fclose( input );
}

void extract_archive(FILE* tar_file){
    struct tar_header header{};

    //get to the beginning of archive file
    fseek( tar_file,0, SEEK_SET );

    while( !feof(tar_file) ){
        //read header from file
        fread( header.signature, 1, sizeof (header.signature), tar_file );
        fread( header.name, 1, sizeof (header.name), tar_file );
        fread( header.version, 1, sizeof (header.version), tar_file );
        fread( header.size, 1, sizeof (header.size), tar_file );

        if(
            header.signature == SIGN &&
            header.version == VERSION
                ){

            FILE *out = fopen(header.name, "wb");
            if (!out) {
                fprintf(stderr, "Failed to open %s for writing\n", header.name);
                break;
            }

            fseek( out,0, SEEK_SET );
            int data_size=0;
            try{
                data_size = atoi(header.size);
            }
            catch(...){
                fprintf(stderr, "Error accessing file size inside of archive\n");
            }

            size_t begin_data = ftell( tar_file ),
                   current = begin_data;

            while( current-begin_data <= data_size ){
                char buffer[2000];
                //todo read data to out file
                size_t read = fread( buffer, 1, 2000, tar_file );
                fwrite( buffer, 1, read, tar_file);
            }

            fclose(out);
        }

        }
    }




}

#endif //OTIK_ARCHIVE_ARCHIVE_H
