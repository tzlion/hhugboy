#ifndef HHUGBOY_DIRECTDRAWSTUFF_H
#define HHUGBOY_DIRECTDRAWSTUFF_H

#include <ddraw.h>

class DirectDrawStuff {
    public:
        IDirectDrawClipper* ddClip;
        IDirectDraw7* dd;
        IDirectDrawSurface7* ddSurface;
        IDirectDrawSurface7* bSurface;
        IDirectDrawSurface7* borderSurface;
};


#endif //HHUGBOY_DIRECTDRAWSTUFF_H
