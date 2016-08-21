//
// Created by Alex on 21/08/2016.
//

#include "MbcUnlLiCheng.h"

void MbcUnlLiCheng::writeMemory(unsigned short address, register byte data) {
    mbc5Write(address,data,true,false);
}
