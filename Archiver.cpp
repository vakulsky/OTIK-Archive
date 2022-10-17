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


    file_header Archiver::buildHeader(const string file, const int codeTableSize)
    {
        file_header header;

        char byte[1];

        FILE *f = fopen(file.c_str(),"rb");
        if(!f)
            cout << "Can't get info of file " << file << endl;
        else {

            // get file size
            fseek(f, 0, SEEK_END);
            int size = ftell(f);

            string name = Archiver::get_file_name(file);  // get file name

            memset(&header, 0, sizeof(struct file_header));
            snprintf(header.signature, SIGNATURE_SZ, "%s", SIGN);
            snprintf(header.name, NAME_SZ, "%s", name.c_str());
            snprintf(header.version, VERSION_SZ, "%s", VERSION);
            snprintf(header.size, SIZE_SZ, "%d", size);
            snprintf(header.algorithm, ALGORITHM_SZ, "%d", algorithmCode);
            snprintf(header.codeTableSize, TABLE_SZ, "%d", codeTableSize);
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

                //extracting file
                char full_path[255];
                strcat(full_path, header.name);
                int _sz = atoi(header.size);
                FILE *curr = fopen(header.name, "wb");

                for (int r = 1; r <= _sz; r++) {
                    if (fread(byte, 1, 1, arc) == 1) fwrite(byte, 1, 1, curr);
                }
                fclose(curr);
                cout << "--- " << header.name << " is extracted ---" << endl;

            }
            fclose(arc);
        }

    }

int Archiver::CompressShannon(){

    //vector of {symbol, {frequency, code}}
    vector<pair<string, pair<int, string>>> symbols;
    ifstream f;
    string str;
    int symbolsAmount=0;

    f.open(archive_file);
    if(!f)
        cout << "Can't read file " << archive_file << endl;
    else {
        while (getline(f, str)) {
            vector<string> syms = divideString(str);
            for(int i = 0; i < syms.size(); i++){
                analyzeUTF8(symbols, syms[i]);
                symbolsAmount += 1;
            }
        };
        f.close();

        shannonCodes(symbols, symbolsAmount);

    }

}


void CompressWrapper(){

}
void ExtractWrapper(){

}

void ExtractShannon(const string& archive_file){

//todo somehow read codes


}


void Archiver::analyzeUTF8(vector<pair<string, pair<int, string>>>& symbols, const string& currentSymbol){
    bool found = false;
    for(auto symbol : symbols){
        if(symbol.first == currentSymbol){
            //symbol is in the list, so increasing the amount
            symbol.second.first++;
            found = true;
        }
    }
    if(!found) {
        //symbol was not found in the list, adding it
        symbols.emplace_back(make_pair(currentSymbol, make_pair(1, "")));
    }
}

void Archiver::shannonCodes(vector<pair<std::string, pair<int, std::string>>> &symbols, int symbolsAmount) {
    for(int i = 0; i < symbols.size(); i++){
        int freqSum = 0;

        int codeLength = ceil(-log2(symbols[i].second.first));

        for(int j = 0; j < i; j++){
            freqSum += symbols[i].second.first;
        }
        double probabilitySum = (double)freqSum/symbolsAmount;

        //calculating code
        double fraction = probabilitySum - (int)(probabilitySum / symbolsAmount);
        for(int k = 0; k < codeLength; k++){
            symbols[i].second.second += to_string((int)(fraction*2));
            if(fraction*2 > 1){
                fraction = fraction - 1;
            }
        }
    }
}

void Archiver::writeShannon(const string& file, vector<pair<std::string, pair<int, std::string>>> &codes){
    char byte[1];  // buffer for reading 1 byte

    file_header header;
    ifstream f;
    ofstream main;
    string str;
    main.open(archive_file, ios_base::binary);  // opening archive

    string codeTable = buildShannonTable(codes);


    //writing file and header to archive
    header = buildHeader(file, sizeof codeTable);

    //todo write table
    //todo write codes


    f.open(file, ios_base::in);
    if(!f)
        cout << "Can't read file " << file << endl;
    else {
        while (getline(f, str)) {
            vector<string> syms = divideString(str);
            for (int i = 0; i < syms.size(); i++) {
                analyzeUTF8(symbols, syms[i]);
                symbolsAmount += 1;
            }
        };
        f.close();
    }

    //write header
    fwrite( reinterpret_cast<const unsigned char *>(&header), 1, sizeof( struct file_header ), main );

    //write file data
    while (!feof(f)) {
        if (fread(byte, 1, 1, f) == 1) fwrite(byte, 1, 1, main);
    }
    cout << *itr << " added to archive '" << this->archive_file << "'." << endl;
    fclose(f);

    fclose(main);
}

string Archiver::buildShannonTable(const vector<pair<string, pair<int, string>>>& codes){
    string table;

    for(auto code : codes){
        table += code.first + " " + code.second.second + "\n";
    }
}


