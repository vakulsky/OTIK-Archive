//
// Created by Grigory on 20/09/2022.
//

#include<cstdio>
#include<cstring>
#include <ctime>

const int PD = 128;
const char* signature = "ZGV";

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
    snprintf( header.signature, 8, "%s", signature  );
    snprintf( header.name, 100, "%s", internal_name  );
    snprintf( header.version, 8, "%07o", 1 );
    snprintf( header.size, 12, "%011llo", end - PD - index );


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

int main( int argc, char* argv[] ){
    if( argc > 1 ){
        FILE* tar = fopen( argv[1], "wb" );
        if( !tar ){
            fprintf( stderr, "Failed to open %s for writing\n", argv[1] );
            return 1;
        }
        int i;
        for( i = 2; i < argc; ++i ){
            tar_add( tar, argv[i], argv[i] );
        }
        //Pad out the end of the tar file
        fexpand( tar, 1024, 0);
        fclose( tar );
        return 0;
    }
    fprintf( stderr, "Please specify some file names!\n" );
    return 0;
}
