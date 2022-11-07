//
// Created by Grigory on 20/09/2022.
//

#include "Archiver.h"
#include "tools.h"



int main(int argv, char* argc[])
{

    cout<<endl<<"------------------------ ARCHIVER ------------------------"<<endl<<endl;
    if(argv>1)
    {
        vector<string> files;  //files from console
        vector<CompressType> algorithms;  //files from console
        string path = "";  // path
        bool flag_fs = false, flag_path = false, pack_flag = false;  // flags
        char type[15];              // flag container
        memset(type,0,7);

        for(int i=1;i<argv;i++)
        {
            if(strcmp(argc[i],"-pack")==0) {  flag_fs=flag_path=false; pack_flag=true;}
            //todo choose between normal pack, Shannon and intelligent
            if(strcmp(argc[i],"-unpack")==0) { strcpy(type,"unpack"); flag_fs=flag_path=pack_flag=false;}
            if(strcmp(argc[i],"-path")==0) {flag_path=true; flag_fs=pack_flag=false; continue; }
            if(strcmp(argc[i],"-files")==0) {flag_fs=true; flag_path=pack_flag=false; continue; }

            if(flag_path) {path.assign(argc[i]); }
            if(flag_fs) files.emplace_back(argc[i]);
            if(pack_flag && algorithms.size() <= ALGORITHM_SZ) addAlgorithm(algorithms, argc[i]);
        }

        Archiver zip(files, path);
        if(!algorithms.empty())
            zip.Compress(algorithms);

        if(strcmp(type,"unpack")==0) zip.Extract(files[0]);
    }
    else {
        cout << "USAGE:" << endl;
        cout << "-pack <normal | shannon | rle | lz77 | intelligent > -files <files to compress> "
                "-path <folder_to_save_archive_file>" << endl;
        cout << "MAY USE UP TO " << ALGORITHM_SZ << " ALGORITHMS AT ONCE" << endl;
        cout << "OR" << endl;
        cout << "-unpack -files <archive_file> -path <path for extracted files>" << endl;
    }
    cout<<endl<<"----------------------------------------------------------"<<endl<<endl;

}
