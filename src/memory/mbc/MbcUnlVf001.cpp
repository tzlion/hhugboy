/*
 * Additional mapper support for hhugboy emulator
 * by taizou 2022
 * This file released under Creative Commons CC0 https://creativecommons.org/publicdomain/zero/1.0/legalcode
 *
 * As part of the hhugboy project it is also licensed under the GNU General Public License v2
 * See "license.txt" in the project root
 */

#include <cstdio>
#include "MbcUnlVf001.h"
#include "../../debug.h"

bool runningmode = false;
byte runningvalue = 00;
byte cur6000 = 00;
byte cur7001 = 00;
byte cur7002 = 00;
byte cur7003 = 00;
byte cur7004 = 00;
byte cur7005 = 00;
byte cur7006 = 00;
byte cur7007 = 00;
byte cur7009 = 00;
byte cur700a = 00;

int changebank = 0;
int changeaddr = 0;
int changevals = 0;
byte changeval1 = 0;
byte changeval2 = 0;
byte changeval3 = 0;
byte changeval4 = 0;

int changesleft = 0;

bool shouldreplace = false;
int replaceaddr = 0;
int replacesourcebank = 0;


byte MbcUnlVf001::readMemory(unsigned short address) {
    // note im not 100% sure on the non-zero bank behaviour
    // as i observed it on a cart returning 3 bytes changes (instead of 4) immediately after the bank was switched
    // and not caring about the specified address
    // but it seems to work this way for now. needs more testing
    bool shouldtriggerchange = (changebank == 0 && address < 0x3fff) || (changebank == rom_bank && address >= 4000);
    if (shouldtriggerchange && address == changeaddr && changesleft == 0) {
        changesleft = changevals;
    }
    if (changesleft > 0 && address < 0x8000) {
        changesleft--;
        int curchange = changevals - changesleft;
        if (curchange == 1) return changeval1;
        if (curchange == 2) return changeval2;
        if (curchange == 3) return changeval3;
        if (curchange == 4) return changeval4;
    }
    /*if (address >= changeaddr && address < (changeaddr + changevals)) {
        if (address == changeaddr) return changeval1;
        if (address == changeaddr+1) return changeval2;
        if (address == changeaddr+2) return changeval3;
        if (address == changeaddr+3) return changeval4;
    }*/
    if (shouldreplace && address >= replaceaddr && address < 0x4000) {
        //int actualaddress = (replacesourcebank << 0xe) + address;
        byte* pointer = &(*this->gbCartRom)[replacesourcebank << 0xe];
        return pointer[address];
    }
    return MbcNin5_LogoSwitch::readMemory(address);
}

void MbcUnlVf001::writeMemory(unsigned short address, byte data) {
    MbcNin5::writeMemory(address, data);
    char buffer[420];
    /* byte firstbank = determineSelectedBankNo(0x0000);
     byte rombank = determineSelectedBankNo(0x4000);
     sprintf(buffer, "determined bank %02x / %02x", firstbank,rombank);
     LinkerLog::addMessage(buffer);*/
    if (address >= 0x6000 && address < 0x8000) {

        unsigned short funkyaddress = address & 0xf00f;

        sprintf(buffer, "Funky write: %04x %02x", address, data);
        debug_win(buffer);

        if (funkyaddress == 0x7000 && data == 0x96) {
            if (runningmode) {
                debug_print("Running mode enabled when running mode already on");
                debug_win(buffer);
            }
            runningmode = true;
            runningvalue = 00;
        } else if (funkyaddress == 0x700f && data == 0x96) {
            if (!runningmode) {
                debug_print("Running mode disabled when running mode already off");
                debug_win(buffer);
            }
            runningmode = false;
        } else if (!runningmode) {
            sprintf(buffer, "Funky address written when running mode off: %04x %02x", address, data);
            debug_print(buffer);
            debug_win(buffer);
        } else if (funkyaddress >= 0x700b) {
            sprintf(buffer, "Write to unknown funky address: %04x %02x", address, data);
            debug_print(buffer);
            debug_win(buffer);
        } else {
            runningvalue = ((runningvalue & 1) ? 0x80 : 0) + (runningvalue >> 1);
            runningvalue = runningvalue ^ data;
            sprintf(buffer, "Addr %04x Data %02x Running %02x", address, data, runningvalue);
            debug_win(buffer);
            switch(funkyaddress) {
                case 0x7001:
                    cur7001 = runningvalue;
                    break;
                case 0x7002:
                    if (runningvalue >= 0x40) {
                        sprintf(buffer, "Value outside bank 0 at 7002: %02x", runningvalue);
                        // debug_print(buffer);
                        debug_win(buffer);
                    }
                    cur7002 = runningvalue;
                    break;
                case 0x7003:
                    if (runningvalue != 0) {
                        sprintf(buffer, "Nonzero value at 7003: %02x", runningvalue);
                        // debug_print(buffer);
                        debug_win(buffer);
                    }
                    cur7003 = runningvalue;
                    break;
                case 0x7004:
                    cur7004 = runningvalue;
                    break;
                case 0x7005:
                    cur7005 = runningvalue;
                    break;
                case 0x7006:
                    cur7006 = runningvalue;
                    break;
                case 0x7007:
                    cur7007 = runningvalue;
                    break;
                case 0x7000:
                    changebank = cur7003;
                    changeaddr =(cur7002 << 8) + cur7001;
                    changeval1 = cur7004;
                    changeval2 = cur7005;
                    changeval3 = cur7006;
                    changeval4 = cur7007;
                    switch(runningvalue & 7) {
                        case 4:
                            changevals = 1;
                            break;
                        case 5:
                            changevals = 2;
                            break;
                        case 6:
                            changevals = 3;
                            break;
                        case 7:
                            changevals = 4;
                            break;
                        default:
                            sprintf(buffer, "Unknown command at 7008: %02x", runningvalue);
                            debug_print(buffer);
                            debug_win(buffer);
                            changevals = 0;
                    }
                    sprintf(buffer, "Now changing: bank %02x addr %04x vals %02x %02x %02x %02x count %01x", changebank, changeaddr, changeval1, changeval2, changeval3, changeval4, changevals);
                    debug_win(buffer);
                    break;

                case 0x7009:
                    cur7009 = runningvalue;
                    break;
                case 0x700a:
                    if (runningvalue >= 0x40) {
                        sprintf(buffer, "Value outside bank 0 at 700a: %02x", runningvalue);
                        debug_print(buffer);
                        debug_win(buffer);
                    }
                    cur700a = runningvalue;
                    break;
                case 0x6000:
                    cur6000 = runningvalue;
                    break;
                case 0x7008:
                    replaceaddr = (cur700a << 8) + cur7009;
                    replacesourcebank = cur6000;
                    if ((runningvalue & 0xf) == 0xf) { // F to pay respects
                        shouldreplace = true;
                    } else {
                        shouldreplace = false;
                        sprintf(buffer, "Unknown command at 7008: %02x", runningvalue);
                        debug_print(buffer);
                        debug_win(buffer);
                    }
                    sprintf(buffer, "Now replacing: addr %04x sourcebank %02x shouldreplace %01x", replaceaddr, replacesourcebank, shouldreplace);
                    debug_win(buffer);

                    break;
            }
        }

    }
}

void MbcUnlVf001::resetVars(bool preserveMulticartState) {
    MbcNin5_LogoSwitch::resetVars(preserveMulticartState);
}

void MbcUnlVf001::readMbcSpecificVarsFromStateFile(FILE *savefile) {
    MbcNin5_LogoSwitch::readMbcSpecificVarsFromStateFile(savefile);
}

void MbcUnlVf001::writeMbcSpecificVarsToStateFile(FILE *savefile) {
    MbcNin5_LogoSwitch::writeMbcSpecificVarsToStateFile(savefile);
}
