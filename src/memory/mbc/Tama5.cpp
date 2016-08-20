//
// Created by Alex on 20/08/2016.
//

#include "Tama5.h"

byte Tama5::readMemory(register unsigned short address) {

    if(address >= 0xA000 && address < 0xC000)
    {
        if(address == 0xA000)
        {
            switch(tama_flag)
            {
                case 0x0A: // test register ?
                    return 1;
                case 0x0D: // RTC registers
                {
                    byte read = 0;
                    if(tama_val4 == 1)
                    {
                        if(tama_val6 == 0x04)
                        {
                            if(tama_val7 == 0x06) // get minutes higher nibble
                            {
                                read = rtc.m;
                                if(read > 0x0f)
                                    read += 6;
                                if(read > 0x1f)
                                    read += 6;
                                if(read > 0x2f)
                                    read += 6;
                                if(read > 0x3f)
                                    read += 6;
                                if(read > 0x4f)
                                    read += 6;
                                if(read == 0x5A)
                                    read = 0;
                                read = (read&0xf0)>>4;
                            }
                            else
                            if(tama_val7 == 0x07) // get hours higher nibble
                            {
                                read = rtc.h;
                                if(read > 0x0f)
                                    read += 6;
                                if(read > 0x1f)
                                    read += 6;
                                if(read > 0x2f)
                                    read += 6;
                                if(read > 0x3f)
                                    read += 6;

                                read = (read&0xf0)>>4;
                            }
                        }
                    } else
                    { // read memory ?
                        read = (*gbMemory)[0xA000|(tama_val6<<4)|tama_val7];
                    }
                    return read;
                }
                case 0x0C: // RTC registers
                {
                    updateTamaRtc();
                    byte read = 0;
                    switch(tama_val4)
                    {
                        case 0:
                            if(rtc.s == 0)
                                read = 0;
                            else
                                read = rtc.s - (tama_time+1); // return 0 if second has passed
                            tama_time = rtc.s;
                            break;
                        case 1:
                            if(tama_val6 == 0x04)
                            {
                                if(tama_val7 == 0x06) // get minutes lower nibble
                                {
                                    read = rtc.m;

                                    //change into correct format
                                    if(read > 0x0f)
                                        read += 6;
                                    if(read > 0x1f)
                                        read += 6;
                                    if(read > 0x2f)
                                        read += 6;
                                    if(read > 0x3f)
                                        read += 6;
                                    if(read > 0x4f)
                                        read += 6;
                                    if(read == 0x5A)
                                        read = 0;

                                    read &= 0x0f;
                                }
                                else
                                if(tama_val7 == 0x07) // get hours lower nibble
                                {
                                    read = rtc.h;

                                    //change into correct format
                                    if(read > 0x0f)
                                        read += 6;
                                    if(read > 0x1f)
                                        read += 6;
                                    if(read > 0x2f)
                                        read += 6;
                                    if(read > 0x3f)
                                        read += 6;

                                    read &= 0x0f;
                                }
                            }
                            break;
                        case 7: // day low
                            read = rtc.d&0xF;
                            break;
                        case 8: // day high
                            read = ((rtc.d&0xf0)>>4);
                            break;
                        case 9: // month
                            read = tama_month;
                            break;
                        case 0xA: // ? (year maybe?)
                            read = 0;
                            break;
                        case 0xF: // ? (year maybe?)
                            read = 0;
                            break;
                    }
                    return read;
                }
            }
        }
    }

    return Default::readMemory(address);
}

void Tama5::updateTamaRtc() {
    time_t now = time(0);
    time_t diff = now-rtc.last_time;
    if(diff > 0)
    {
        rtc.s += diff % 60;
        if(rtc.s > 59)
        {
            rtc.s -= 60;
            rtc.m++;
        }

        diff /= 60;

        rtc.m += diff % 60;
        if(rtc.m > 59)
        {
            rtc.m -= 60;
            rtc.h++;
        }

        diff /= 60;

        rtc.h += diff % 24;
        if(rtc.h > 24)
        {
            rtc.h -= 24;
            rtc.d++;
        }
        diff /= 24;

        rtc.d += diff;
        if(rtc.d > 31)
        {
            rtc.d -= 31;
            tama_month++;
            if(tama_month > 12)
                tama_month -= 12;
        }
    }
    rtc.last_time = now;
}
