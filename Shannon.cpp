//
// Created by Grigory on 18/10/2022.
//

#include "Shannon.h"

void Shannon::Compress(const string& inFileName, const string& outFileName){

    ifstream f;
    string str;
    int compressedSize = 0;

    codes.clear();
    symbolsAmount = 0;

    f.open(inFileName);
    if(!f) {
        cout << "Can't read inFileName " << inFileName << endl;

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

        writeToFile(inFileName, outFileName);
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
            freqSum += codes[j].second.first;

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


void Shannon::writeToFile(const string& inFileName, const string& outFileName) {

    ofstream archive;
    ifstream fileStream;
    string str;

    archive.open(outFileName, ios::app);
    if (!archive) {
        cout << "Can't open inFileName " << outFileName << endl;

    } else {


        //writing table size (symbols in table)
        archive << codes.size() << endl;

        //writing table
        for (auto code: codes) {
            archive << code.first  << code.second.second << endl;
        }

        fileStream.open(inFileName);
        if (!fileStream) {
            cout << "Can't read inFileName " << inFileName << endl;
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
            archive << "\n";
            fileStream.close();


        };
        archive.close();
    }
}

void Shannon::Extract(ifstream & archiveFile, file_header &header) {

    string symbolsInTable,
                    accum,
                    buff;
    char byte[1];

    ofstream extractedFile;
    extractedFile.open(header.name);

    getline(archiveFile, symbolsInTable);

    codes.clear();
    for(int i = 0; i < stoi(symbolsInTable); i++){
        buff.clear();
       getline(archiveFile, buff);
        parseCode(buff);
    }

    /////
    cout << "DEBUG | CODES FROM ARCHIVE" << endl;
    for(auto code : codes){
        cout << code.first << " | " << code.second.second << endl;
    }
    /////


    archiveFile.read(byte, 1);
    while (byte[0] != '\n') {
        accum += byte[0];

        for(const auto& sym : codes){
            if(accum == sym.second.second){

                /////
                cout << "DEBUG | (matched): " << accum << " --> " << sym.first << endl;
                /////

                extractedFile << sym.first;
                accum.clear();
            }
        }

        archiveFile.read(byte, 1);

    }
    extractedFile.close();

}
