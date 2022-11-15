//
// Created by grigory on 15/11/22.
//

#ifndef OTIK_ARCHIVE_HAMMINGCODE_H
#define OTIK_ARCHIVE_HAMMINGCODE_H

#include <iostream>
#include <string>
#include <cstring>
#include <vector>
#include <fstream>
#include <cmath>
#include "fileHeader.h"
#include <algorithm>
#include <filesystem>
#include <sstream>

using namespace std;
class HammingCode {


public:
    void AddProtection(const string& inFileName, const string& outFileName);
    void RemoveProtection(const string& inFileName, const string& outFileName);

};


#endif //OTIK_ARCHIVE_HAMMINGCODE_H
