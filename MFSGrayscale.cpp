#include "MFSGrayscale.h"
//#include <stdio.h>
//#include <math.h>
//#include <boost/algorithm/string.hpp>
using namespace cv;

/***********************************************************************
 * (DE)CONSTRUCTOR(S)
 **********************************************************************/
//MFSGrayscale::MFSGrayscale(unsigned int w, unsigned int h, unsigned int k, unsigned int k_t, unsigned int t, unsigned char sE, unsigned char sN, float cE, float cD, string d): MFSquare(w, h, k, k_t, t, sE, sN, cE, cD)
MFSGrayscale::MFSGrayscale(unsigned int w, unsigned int h, unsigned int k, unsigned int k_t, unsigned int t, unsigned char sE, const char *cn, float cs, string d): MFSquare(w, h, k, k_t, t, sE, cn, cs)
{
	initStart(d);
	
	initEnd();
}

/***********************************************************************
 * COST CALCULATIONS
 **********************************************************************/

void MFSGrayscale::checkNeighbors()
{
	for (unsigned int y = 0; y < height; y++)
	{
		unsigned long y_times_width = y * width;
		for (unsigned int x = 0; x < width; x++)
		{
			if (y < height-1)
			{
				incCostNeighbor(y_times_width + x, dataField[y_times_width + x] * 255 / (range-1), y_times_width+width + x, dataField[y_times_width+width + x] * 255 / (range-1));
			}
			if (x < width-1)
			{
				incCostNeighbor(y_times_width + x, dataField[y_times_width + x] * 255 / (range-1), y_times_width + x+1, dataField[y_times_width + x+1] * 255 / (range-1));
			}
		}
	}
}

void MFSGrayscale::recalculateCost()
{
	MFSquare::recalculateCost();
	checkNeighbors();
}

/***********************************************************************
 * IMPLEMENTED PURE VIRTUAL FUNCTIONS FROM MARKER FIELD
 **********************************************************************/
const char* MFSGrayscale::getClassName()
{
	return "Grayscale Square Marker Field";
}


/***********************************************************************
 * IMPLEMENTED PURE VIRTUAL FUNCTIONS FROM MARKER FIELD SQUARE
 **********************************************************************/

unsigned char MFSGrayscale::compareTwo(unsigned int first, unsigned int second) {
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

Scalar MFSGrayscale::getRGBValue(unsigned int v)
{
	return Scalar(v * 255 / (range-1), v * 255 / (range-1), v * 255 / (range-1));
}
