#include "Filter.h"
#include "filters.h"
#include "scale2x.h"
#include "scale3x.h"

Filter* Filter::getFilter(videofiltertype type)
{
    switch(type){
		case VIDEO_FILTER_SOFT2X:
		    return new NearestNeighbour(2); 
			break;
		case VIDEO_FILTER_SOFTXX:
		    return new NearestNeighbour(8); 
			break;
		case VIDEO_FILTER_SCALE2X:
			return new Scale2x();
			break;   
		case VIDEO_FILTER_SCALE3X:
			return new Scale3x();   
			break;           
		case VIDEO_FILTER_NONE:
		default:
			return new NoFilter();
			break;
	}   
}