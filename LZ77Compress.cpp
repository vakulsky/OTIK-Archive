//
// Created by grigory on 03/11/22.
//

#include "LZ77Compress.h"


file_header LZ77Compress::buildHeader(const string& fileName){
    file_header header{};
    ifstream file;

    file.open(fileName, ios_base::in);
    if(!file) {
        cout << "Can't open file " << fileName << endl;
    }
    else {

        file.seekg( 0, std::ios::end );
        int fileSize = (int)(file.tellg());

        memset(&header, 0, sizeof(struct file_header));
        snprintf(header.signature, SIGNATURE_SZ, "%s", SIGN);
        snprintf(header.name, NAME_SZ, "%s", fileName.c_str());
        snprintf(header.version, VERSION_SZ, "%s", VERSION);
        snprintf(header.size, SIZE_SZ, "%d", fileSize);
        snprintf(header.algorithm, ALGORITHM_SZ, "%s", "3");
    }
    return header;

}


int LZ77Compress::Compress(const string& fileName, const string& archiveName, bool writeHeader) {
    ifstream file;
    ofstream archive;
    char buff[1];
    char previous[1];
    char repeatedSym[1];
    string diffAccum;
    int count = 0, minRep = 3;
    bool firstIteration = true;


    file.open(fileName, ios::in);
    file.seekg(0, ios::beg);
    archive.open(archiveName, ios::app);
    if (!file) {
        cout << "Can't read file " << fileName << endl;
        return 0;
    } else if (!archive) {
        cout << "Can't open archive file " << fileName << endl;
        return 0;
    } else {

        if (writeHeader) {
            file_header header = buildHeader(fileName);

            //writing header
            archive.write(header.signature, SIGNATURE_SZ);
            archive.write(header.name, NAME_SZ);
            archive.write(header.version, VERSION_SZ);
            archive.write(header.size, SIZE_SZ);
            archive.write(header.algorithm, ALGORITHM_SZ);
            archive.write(header.padding, PADDING_SZ);
        }

        auto size = archive.tellp();

        std::string buffer;
        TEXT_IN = "";

        while( !file.eof() )
        {
            std::getline( file, buffer );
            TEXT_IN += buffer;
            buffer.clear();
        }

        TEXT_OUT = "";
        strToAnalyze = "";
        window = "";
        SL.first = 0;
        SL.second = 0;
        position = 0;
        encodeLZ77();

        //finally write '\n'
        archive << "\n";

        size = archive.tellp() - size;

        archive << TEXT_OUT << "\n";

        file.close();
        archive.close();
        return size;


    }
}



void LZ77Compress::Extract(ifstream& archiveFile, file_header& header){

    ofstream file;
    int shift, size;
    string str;
    string fileName = header.name;


    file.open(fileName, ios::out);
    if (!file) {
        cout << "Can't read file " << fileName << endl;

    } else if (!archiveFile) {
        cout << "Can't open archive file" << endl;

    } else {

        TEXT_IN = "";
        TEXT_OUT = "";

        std::getline( archiveFile, TEXT_IN);
        std::getline( archiveFile, TEXT_IN );



        position = 0;
        while(position < TEXT_IN.size()){
            shift = atoi(TEXT_IN.substr(position, 1).c_str());
            position++;
            size = atoi(TEXT_IN.substr(position, 1).c_str());
            position++;

            if(shift == 0){

                /////
                cout << "DEBUG: shift = 0 "  <<  endl;
                /////


                if(position < TEXT_IN.size())
                    str = TEXT_IN.substr(position, 1);
                position++;

                /////
                cout << "DEBUG: written: " << str << endl <<  endl;
                /////


                TEXT_OUT.append(str);
            }
            else{

                /////
                cout << "DEBUG: shift > 0 "  <<  endl;
                /////



                for(int i = 0; i < size; i++){
                    str = TEXT_OUT[TEXT_OUT.size()-shift];
                    TEXT_OUT.append(str);
                }
                /////
                cout << "DEBUG: writing " << str << " " <<  size << " times" <<  endl;
                /////

                if(position < TEXT_IN.size()) {
                    str = TEXT_IN.substr(position, 1);
                    TEXT_OUT.append(str);

                    /////
                    cout << "DEBUG: then writing " << str  << endl <<  endl;
                    /////

                }
                position++;

            }
        }

        file << TEXT_OUT << endl;

    }


    file.close();
}


void LZ77Compress::encodeLZ77() {

    stringstream builder;

    while(position < TEXT_IN.size()){

        strToAnalyze = TEXT_IN.substr(position, 1);
        updateWindow();

        /////
        cout << "DEBUG: analyze: " << strToAnalyze << endl;
        cout << "DEBUG: window: " << window << endl;
        /////
        
        if(window.find(strToAnalyze ) == std::string::npos )
        {

            /////
            cout << "DEBUG: cant find " << strToAnalyze << " in " << window <<  endl;
            /////

            // Cant find any byte from buffer
            builder << "00" << strToAnalyze;

            /////
            cout << "DEBUG: written:  " << "00" << strToAnalyze <<  endl << endl;
            /////

            TEXT_OUT.append(builder.str());
            builder.str("");
            strToAnalyze.clear();
            position++;
            updateWindow();
        }
        else
        {
            // We founded any byte from buffer in inspection
            SL.first = position - windowStartPos - window.find(strToAnalyze );
            while(window.find(strToAnalyze ) != std::string::npos && SL.second < maxChunkLength && position < TEXT_IN.size())
            {

                SL.second++;
                position++;

                /////
                cout << "DEBUG: found " << strToAnalyze << " in " << window <<  endl;
                /////

                /////
                cout << "DEBUG: pos:  " << position << " size: " << TEXT_IN.size() << endl << endl;
                /////

                updateWindow();
                if(position < TEXT_IN.size()) {
                    if (strToAnalyze.find(TEXT_IN.substr(position, 1)) != string::npos) {
                        strToAnalyze.append(TEXT_IN.substr(position, 1));
                    }
                    else
                        break;
                }

            }

            if(position < TEXT_IN.size()){

                builder << SL.first << SL.second << TEXT_IN.substr(position, 1);

                /////
                cout << "DEBUG: written:  " << SL.first << SL.second << TEXT_IN.substr(position, 1) << endl << endl;
                /////

                position++;

            }
            else{
                builder << SL.first << SL.second << "";

                /////
                cout << "DEBUG: written:  " << SL.first << SL.second <<  endl << endl;
                /////

                position++;
            }


            TEXT_OUT.append(builder.str());
            builder.str("");

            strToAnalyze.clear();
            SL.first = 0;
            SL.second = 0;
        }

        /////
        cout << "DEBUG: pos:  " << position << " size: " << TEXT_IN.size() << endl << endl;
        /////
    }

}

void LZ77Compress::updateWindow() {
    int pos = 0,
        n = position;

    if(position < 1){
        window = "";
    }
    else
    {
        if(position > windowSize) {
            pos = position - windowSize;
            n = windowSize;
        }

        window = TEXT_IN.substr(pos, n);
    }

    windowStartPos = pos;



}



