//
// Created by Grigory on 18/10/2022.
//

#include "Shannon.h"

int Shannon::Compress(const string& file, const string& archiveName){

    ifstream f;
    string str;
    int compressedSize = 0;

    codes.clear();
    symbolsAmount = 0;

    f.open(file);
    if(!f) {
        cout << "Can't read file " << file << endl;
        return compressedSize;
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
        compressedSize = writeToFile(file, archiveName, header);

        return compressedSize;

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
        //counting coded text
        size += sym.second.first * sym.second.second.size();
    }


    memset( &header, 0, sizeof( struct file_header ) );
    snprintf( header.signature, SIGNATURE_SZ, "%s", SIGN  );
    snprintf( header.name, NAME_SZ, "%s", file.c_str()  );
    snprintf( header.version, VERSION_SZ, "%s", VERSION );
    snprintf( header.size, SIZE_SZ, "%d", size );
    snprintf( header.algorithm, ALGORITHM_SZ, "%s", "1" );


    return header;
}

int Shannon::writeToFile(const string& file, const string& archiveName, file_header& header) {

    ofstream archive;
    ifstream fileStream;
    string str;

    archive.open(archiveName);
    if (!archive) {
        cout << "Can't open file " << archiveName << endl;
        return 0;
    } else {

//       archive.seekp(ios_base::end);

        //writing header
        archive.write(header.signature, sizeof(header.signature));
        archive.write(header.name, sizeof(header.name));
        archive.write(header.version, sizeof(header.version));
        archive.write(header.size, sizeof(header.size));
        archive.write(header.algorithm, sizeof(header.algorithm));
        archive.write(header.padding, sizeof(header.padding));

        auto size = archive.tellp();

        //writing table size (symbols in table)
        archive << codes.size() << endl;

        //writing table
        for (auto code: codes) {
            archive << code.first  << code.second.second << endl;
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
        size = archive.tellp() - size;
        archive.close();

        return size;
    }
}

void Shannon::Extract(FILE *archiveFile, file_header &header) {

    char symbolsInTable[10];
    char byte[1];
    string accum;
    ofstream extractedFile;
    extractedFile.open(header.name);

    fgets(symbolsInTable, sizeof symbolsInTable, archiveFile);

    /////
    cout << "DEBUG | (syms in table): " << atoi(symbolsInTable) << endl;
    /////

    codes.clear();
    for(int i = 0; i < atoi(symbolsInTable); i++){
        char str[20];
        fgets(str, sizeof str, archiveFile);
        parseCode(string(str));
    }

    /////
    cout << "DEBUG | CODES FROM ARCHIVE" << endl;
    for(auto code : codes){
        cout << code.first << " | " << code.second.second << endl;
    }
    /////

    while (fread(byte, 1, 1, archiveFile) == 1) {
        accum += byte[0];

        /////
        cout << "DEBUG | (accum): " << accum << endl;
        /////

        for(const auto& sym : codes){
            if(accum == sym.second.second){  //todo not working

                /////
                cout << "DEBUG | (matched): " << accum << " --> " << sym.first << endl;
                /////

                extractedFile << sym.first;
                accum.clear();
            }
        }

    }
    extractedFile.close();

}
