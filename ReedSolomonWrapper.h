//
// Created by grigory on 16/11/22.
//

#ifndef OTIK_ARCHIVE_REEDSOLOMONWRAPPER_H
#define OTIK_ARCHIVE_REEDSOLOMONWRAPPER_H

#include <cstddef>

#include "schifra/schifra_galois_field.hpp"
#include "schifra/schifra_sequential_root_generator_polynomial_creator.hpp"
#include "schifra/schifra_reed_solomon_encoder.hpp"
#include "schifra/schifra_reed_solomon_file_encoder.hpp"
#include "schifra/schifra_reed_solomon_decoder.hpp"
#include "schifra/schifra_reed_solomon_file_decoder.hpp"

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
class ReedSolomonWrapper {

public:
    void AddProtection(const string& inFileName, const string& outFileName);
    void RemoveProtection(const string& inFileName, const string& outFileName);

};


#endif //OTIK_ARCHIVE_REEDSOLOMONWRAPPER_H
