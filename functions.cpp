//
// Created by Grigory on 20/09/2022.
//

#include "functions.h"

int digs(double w)
{
    int yield = 0;
    while(w>10) {yield++;w/=10;}
    return yield+1;
}