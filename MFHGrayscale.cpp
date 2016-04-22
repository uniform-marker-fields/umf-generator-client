#include "MFHGrayscale.h"
//#include <stdio.h>
//#include <math.h>
//#include <boost/algorithm/string.hpp>
using namespace cv;

/***********************************************************************
 * (DE)CONSTRUCTOR(S)
 **********************************************************************/
//MFHGrayscale::MFHGrayscale(unsigned int w, unsigned int h, unsigned int k, unsigned int k_t, unsigned int t, unsigned char sE, unsigned char sN, float cE, float cD, string d): MFHexagonal(w, h, k, k_t, t, sE, sN, cE, cD)
MFHGrayscale::MFHGrayscale(unsigned int w, unsigned int h, unsigned int k, unsigned int k_t, unsigned int t, unsigned char sE, const char *cn, float cs, string d): MFHexagonal(w, h, k, k_t, t, sE, cn, cs)
{
	initStart(d);
	
	initEnd();
}

/***********************************************************************
 * COST CALCULATIONS
 **********************************************************************/

void MFHGrayscale::checkNeighbors()
{
	for (unsigned int y = 0; y < height; y++)
	{
		unsigned long y_times_width = y * width;
		for (unsigned int x = 0; x < width; x++)
		{
			if (y < height-1)
			{
				incCostNeighborWrap(y_times_width + x, dataField[y_times_width + x] * 255 / (range-1), y_times_width+width + x, dataField[y_times_width+width + x] * 255 / (range-1));
				
				if ((y & 1) == MARKERFIELD_HEXA_LINE_START_LATER)
				{
					if (x < width-1)
					{
						incCostNeighborWrap(y_times_width + x, dataField[y_times_width + x] * 255 / (range-1), y_times_width+width + x+1, dataField[y_times_width+width + x+1] * 255 / (range-1));
					}
				}
				else
				{
					if (x > 0)
					{
						incCostNeighborWrap(y_times_width + x, dataField[y_times_width + x] * 255 / (range-1), y_times_width+width + x-1, dataField[y_times_width+width + x-1] * 255 / (range-1));
					}
				}
			}
			if (x < width-1)
			{
				incCostNeighborWrap(y_times_width + x, dataField[y_times_width + x] * 255 / (range-1), y_times_width + x+1, dataField[y_times_width + x+1] * 255 / (range-1));
			}
		}
	}
}

void MFHGrayscale::incCostNeighborWrap(unsigned long i1, unsigned char v1, unsigned long i2, unsigned char v2)
{
	bool conflictFlag = true;	
	incCostNeighbor(i1, v1, i2, v2, &conflictFlag);
	if (conflictFlag)
	{
		costField[i1] += MARKERFIELD_PEN_CONFLICTS * 800;
		costField[i2] += MARKERFIELD_PEN_CONFLICTS * 800;
		cost += MARKERFIELD_PEN_CONFLICTS * 1600;
		conflictCount++;
	}
}

void MFHGrayscale::recalculateCost()
{
	MFHexagonal::recalculateCost();
	checkNeighbors();
}

/***********************************************************************
 * IMPLEMENTED PURE VIRTUAL FUNCTIONS FROM MARKER FIELD
 **********************************************************************/
const char* MFHGrayscale::getClassName()
{
	return "Grayscale Hexagonal Marker Field";
}


/***********************************************************************
 * IMPLEMENTED PURE VIRTUAL FUNCTIONS FROM MARKER FIELD SQUARE
 **********************************************************************/

unsigned char MFHGrayscale::compareTwo(unsigned int first, unsigned int second) {
	if (first > second)
	{
		return MARKERFIELD_COMPARE_GREATER;
	}
	else if (first == second)
	{
		return MARKERFIELD_COMPARE_EQUAL;
	}
	else
	{
		return MARKERFIELD_COMPARE_SMALLER;
	}
}

Scalar MFHGrayscale::getRGBValue(unsigned int v)
{
	return Scalar(v * 255 / (range-1), v * 255 / (range-1), v * 255 / (range-1));
}
