//
// Created by Grigory on 20/09/2022.
//

#include "Archive.h"

int main( int argc, char* argv[] ){
    if( argc > 2 ){

        if(argv[1] == "a") {

            FILE *tar = fopen(argv[2], "wb");
            if (!tar) {
                fprintf(stderr, "Failed to open %s for writing\n", argv[2]);
                return 1;
            }
            int i;
            for (i = 3; i < argc; ++i) {
                tar_add(tar, argv[i], argv[i]);
            }
            //Pad out the end of the tar file
            fexpand(tar, PD, 0);
            fclose(tar);
            return 0;
        }
        else{
            FILE *tar = fopen(argv[2], "rb");
            if (!tar) {
                fprintf(stderr, "Failed to read %s\n", argv[2]);
                return 1;
            }

//            extract_archive();

            return 0;
        }
    }
    fprintf( stderr, "Usage: Archiver <a(archive)|> <archive_name> <file1> [file2... fileN]\n"
                     "Archiver <e(extract)|> <archive_name>\n" );
    return 0;
}
