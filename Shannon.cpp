//
// Created by Grigory on 18/10/2022.
//

#include "Shannon.h"

int Shannon::Compress(const string& file, const string& archiveName){

    ifstream f;
    string str;

    codes.clear();
    symbolsAmount = 0;

    f.open(file);
    if(!f) {
        cout << "Can't read file " << file << endl;
        return 0;
    }
    else {
        while (getline(f, str)) {
            vector<string> syms = divideString(str);
            for(auto & sym : syms){
                analyzeUTF8(sym);
                symbolsAmount++;
            }
        };
        f.close();

        shannonCodes();

        /////
        for(auto & code : codes){
            cout << "DEBUG| (codes): " << code.first << " | " << code.second.first << " | " << code.second.second << endl;
        }
        /////

        file_header header = buildHeader(file);
        writeToFile(file, archiveName, header);

        return atoi(header.size);

    }

}


void Shannon::analyzeUTF8(const string& currentSymbol){
    bool found = false;
    for(auto & symbol : codes){
        if(symbol.first == currentSymbol){
            //symbol is in the list, so increasing the amount
            symbol.second.first++;
            found = true;
        }
    }
    if(!found) {
    //symbol was not found in the list, adding it
    codes.emplace_back(make_pair(currentSymbol, make_pair(1, "")));
    }
}

void Shannon::shannonCodes() {

    //sort
    sort(codes.begin(), codes.end(), frequencyDescending());

    for (int i = 0; i < codes.size(); i++) {
        int freqSum = 0;

        //probability = frequency/symbolsAmount
        int codeLength = ceil(-log2((double)(codes[i].second.first)/symbolsAmount));

        for (int j = 0; j < i; j++)
            freqSum += codes[i].second.first;

        double probabilitySum = (double) freqSum / symbolsAmount;

        //calculating code
        double fraction = probabilitySum - (int) (probabilitySum);

        /////
        cout << "DEBUG| (shannonCodes fun (sym)): " << codes[i].first << endl;
        cout << "DEBUG| (freq sum): " << freqSum << endl;
        cout << "DEBUG| (code length): " << codeLength << endl;
        cout << "DEBUG| (fraction): " << fraction << endl;
        cout << "DEBUG| (prob sum): " << probabilitySum << endl;
        /////

        for (int k = 0; k < codeLength; k++) {
            fraction = fraction * 2;
            codes[i].second.second += to_string((int) (fraction));
            if ( fraction > 1 ) {
                fraction = fraction - 1;
            }
        }
    }

    /////
    cout << "DEBUG| (syms amount): " << symbolsAmount << endl;
    /////
}

file_header Shannon::buildHeader(const string& file)
{
    file_header header;
    int size = 0;

    for(const auto& sym : codes){
        size += sym.second.first * sym.second.second.size();
    }


    memset( &header, 0, sizeof( struct file_header ) );
    snprintf( header.signature, SIGNATURE_SZ, "%s", SIGN  );
    snprintf( header.name, NAME_SZ, "%s", file.c_str()  );
    snprintf( header.version, VERSION_SZ, "%s", VERSION );
    snprintf( header.size, SIZE_SZ, "%d", size );
    snprintf( header.algorithm, ALGORITHM_SZ, "%d", 1 );


    return header;
}

void Shannon::writeToFile(const string& file, const string& archiveName, file_header& header) {

    ofstream archive;
    ifstream fileStream;
    string str;

    archive.open(archiveName);
    if (!archive) {
        cout << "Can't open file " << archiveName << endl;
    } else {

        archive.seekp(ios_base::end);

        //writing header
        archive << header.signature
                << header.name
                << header.version
                << header.size
                << header.algorithm
                << header.padding;

        //writing table size (symbols in table)
        archive << codes.size() << endl;

        //writing table
        for (auto code: codes) {
            archive << code.first << " " << code.second.second << endl;
        }

        fileStream.open(file);
        if (!fileStream) {
            cout << "Can't read file " << file << endl;
        } else {
            while (getline(fileStream, str)) {
                vector<string> syms = divideString(str);
                for (auto &sym: syms) {
                    for (auto code: codes) {
                        if (sym == code.first) {
                            archive << code.second.second;
                        }
                    }
                }


            };
            fileStream.close();


        };
        archive.close();
    }
}

void Shannon::Extract(FILE *archiveFile, file_header &header) {

//todo somehow read codes

}
