//
// Created by grigory on 03/11/22.
//

#include "LZ77Compress.h"


void LZ77Compress::Compress(const string& inFileName, const string& outFileName) {
    ifstream file;
    ofstream archive;
    char buff[1];
    char previous[1];
    char repeatedSym[1];
    string diffAccum;
    int count = 0, minRep = 3;
    bool firstIteration = true;


    file.open(inFileName, ios::in);
    file.seekg(0, ios::beg);
    archive.open(outFileName, ios::app);
    if (!file) {
        cout << "Can't read file " << inFileName << endl;

    } else if (!archive) {
        cout << "Can't open archive file " << inFileName << endl;

    } else {

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

        archive << TEXT_OUT << "\n";

        file.close();
        archive.close();
    }
}



void LZ77Compress::Extract(ifstream& archiveFile, file_header& header){

    ofstream file;
    unsigned int shift, size;
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
        while(position < TEXT_IN.size()){   //todo buts -> int
            auto shiftSize = toInt(TEXT_IN[position]);
            position++;
            shift = shiftSize[0];
            size = shiftSize[1];

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
                cout << "DEBUG: shift = "  << shift  <<  endl;
                /////


                for(int i = 0; i < size; i++){
                    str = TEXT_OUT[TEXT_OUT.size()-shift];
                    /////
                    cout << "DEBUG: writing " << str <<  endl;
                    /////
                    TEXT_OUT.append(str);
                }


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
            builder << toByte(0, 0) << strToAnalyze;

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
                cout << "DEBUG: shift: " << SL.first <<  endl;
                /////

                //updateWindow();
                if(position < TEXT_IN.size()) {
                    if (strToAnalyze.find(TEXT_IN.substr(position, 1)) != string::npos) {
                        strToAnalyze.append(TEXT_IN.substr(position, 1));
                    }
                    else
                        break;
                }

            }

            if(position < TEXT_IN.size()){

                builder << toByte(SL.first, SL.second) << TEXT_IN.substr(position, 1);

                /////
                cout << "DEBUG: written:  " << toByte(SL.first, SL.second) << TEXT_IN.substr(position, 1) << endl << endl;
                /////

                position++;

            }
            else{
                builder << toByte(SL.first, SL.second) << "";

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
        cout << "DEBUG: pos:  " << position << " file_size: " << TEXT_IN.size() << endl << endl;
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


char LZ77Compress::toByte(unsigned int s, unsigned int l) {
    char c;

    c = (char)((s << 4) | (l & 15));

    return c;

}
vector<unsigned int> LZ77Compress::toInt(const char c){
    vector<unsigned int> sl;

    sl.emplace_back((unsigned int)((c >> 4) & 15));
    sl.emplace_back((unsigned int)(c & 15));

    return sl;

}




