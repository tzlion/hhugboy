#ifndef HHUGBOY_DIRECTBOLLOCKSCONTAINER_H
#define HHUGBOY_DIRECTBOLLOCKSCONTAINER_H

#include <ddraw.h>

class DirectBollocksContainer {
    public:
        IDirectDrawClipper* ddClip;
        IDirectDraw7* dd;
        IDirectDrawSurface7* ddSurface;
        IDirectDrawSurface7* bSurface;
        IDirectDrawSurface7* borderSurface;
};


#endif //HHUGBOY_DIRECTBOLLOCKSCONTAINER_H
