#include "../../options.h"

typedef unsigned long DWORD;
typedef unsigned short WORD;

#ifndef FILTER_H
#define FILTER_H

class Filter
{
    public:
        virtual void filter32(DWORD *pointer,DWORD *source,int width,int height,int pitch) = 0;
        virtual void filter16(WORD *pointer,WORD *source,int width,int height,int pitch) = 0;
        virtual int getFilterDimension() = 0;
        static Filter* getFilter(videofiltertype type);
};

#endif