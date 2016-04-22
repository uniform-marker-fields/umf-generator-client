#include "MFHColor.h"
#include <boost/algorithm/string.hpp>
//#include <stdio.h>
//#include <math.h>
using namespace cv;

/***********************************************************************
 * (DE)CONSTRUCTOR(S)
 **********************************************************************/
//MFHColor::MFHColor(unsigned int w, unsigned int h, unsigned int k, unsigned int k_t, unsigned int t, unsigned char sE, unsigned char sN, float cE, float cD, const char* c, string d): MFHexagonal(w, h, k, k_t, t, sE, sN, cE, cD, c)
MFHColor::MFHColor(unsigned int w, unsigned int h, unsigned int k, unsigned int k_t, unsigned int t, unsigned char sE, const char *cn, float cs, const char* c, string d): MFHexagonal(w, h, k, k_t, t, sE, cn, cs, c)
{
	initStart(d);
	
	init();
	
	initEnd();
}

/***********************************************************************
 * INITALIZATION METHODS
 **********************************************************************/

void MFHColor::init()
{
	initColors();
}

void MFHColor::initColors()
{
	colors = vector<Scalar>();
	unsigned char r, g, b;
	string s = boost::to_lower_copy(colorsText);
	
	unsigned int i = 0;
	unsigned int rangeHere = 0;
	while (i < s.length() || rangeHere < range)
	{
		if (i+MF_COLOR_RGB_HEXA_LENGTH <= s.length())
		{
			if (s[i] != MF_COLOR_RGB_HEXA_START)
			{
				cerr << "Warning: not the proper hexa format is used!" << endl;
			}
			r = (unsigned char) getNumericValue(s[i+1],16)*16 + getNumericValue(s[i+2],16);
			g = (unsigned char) getNumericValue(s[i+3],16)*16 + getNumericValue(s[i+4],16);
			b = (unsigned char) getNumericValue(s[i+5],16)*16 + getNumericValue(s[i+6],16);
			colors.push_back(Scalar( b, g, r));
			rangeHere++;
		}
		else
		{
			cerr << "Warning: zero values added!" << endl;
			colors.push_back(Scalar( 0, 0, 0));
			rangeHere++;
		}
		i += MF_COLOR_RGB_HEXA_LENGTH;
		if (i < s.length() && s[i] != MF_COLOR_RGB_SEPARATOR)
		{
			cerr << "Warning: not the proper separator used!" << endl;
		}
		i++;
	}
}

/***************************************************************
 * CHECK ATTRIBUTES
 **************************************************************/

void MFHColor::checkAttributes()
{
	MFHexagonal::checkAttributes();
	checkColors();
}

void MFHColor::checkColors()
{
	string s = colorsText;
	if (s.length() != range * MF_COLOR_RGB_HEXA_LENGTH + range - 1)
	{
		throw MarkerFieldException("Colors' text does not fit with range!");
	}
}

/***********************************************************************
 * COST CALCULATIONS
 **********************************************************************/

void MFHColor::checkNeighbors()
{
	Scalar v1;
	for (unsigned int y = 0; y < height; y++)
	{
		unsigned long y_times_width = y * width;
		for (unsigned int x = 0; x < width; x++)
		{
			v1 = getRGBValue(dataField[y_times_width + x]);
			if (y < height-1)
			{
				incCostNeighborWrap(y_times_width + x, v1, y_times_width+width + x, getRGBValue(dataField[y_times_width+width + x]));
				
				if ((y & 1) == MARKERFIELD_HEXA_LINE_START_LATER)
				{
					if (x < width-1)
					{
						incCostNeighborWrap(y_times_width + x, v1, y_times_width+width + x+1, getRGBValue(dataField[y_times_width+width + x+1]));
					}
				}
				else
				{
					if (x > 0)
					{
						incCostNeighborWrap(y_times_width + x, v1, y_times_width+width + x-1, getRGBValue(dataField[y_times_width+width + x-1]));
					}
				}
			}
			if (x < width-1)
			{
				incCostNeighborWrap(y_times_width + x, v1, y_times_width + x+1, getRGBValue(dataField[y_times_width + x+1]));
			}
		}
	}
}

void MFHColor::incCostNeighborWrap(unsigned long i1, Scalar v1, unsigned long i2, Scalar v2)
{
	bool conflictFlag = true;	
	incCostNeighbor(i1, v1[0], i2, v2[0], &conflictFlag);
	incCostNeighbor(i1, v1[1], i2, v2[1], &conflictFlag);
	incCostNeighbor(i1, v1[2], i2, v2[2], &conflictFlag);
	if (conflictFlag)
	{
		costField[i1] += MARKERFIELD_PEN_CONFLICTS * 800;
		costField[i2] += MARKERFIELD_PEN_CONFLICTS * 800;
		cost += MARKERFIELD_PEN_CONFLICTS * 1600;
		conflictCount++;
	}
}

void MFHColor::recalculateCost()
{
	MFHexagonal::recalculateCost();
	checkNeighbors();
}

/***********************************************************************
 * IMPLEMENTED PURE VIRTUAL FUNCTIONS FROM MARKER FIELD
 **********************************************************************/
const char* MFHColor::getClassName()
{
	return "Color Hexagonal Marker Field";
}


/***********************************************************************
 * IMPLEMENTED PURE VIRTUAL FUNCTIONS FROM MARKER FIELD SQUARE
 **********************************************************************/

unsigned char MFHColor::compareTwo(unsigned int first, unsigned int second) {
	Scalar c1 = getRGBValue(first);
	Scalar c2 = getRGBValue(second);
	
	unsigned char ret = 0;

	// BLUE
	if (c1[0] > c2[0])
	{
		ret += MARKERFIELD_COMPARE_GREATER;
	}
	else if (c1[0] == c2[0])
	{
		ret += MARKERFIELD_COMPARE_EQUAL;
	}
	else
	{
		ret += MARKERFIELD_COMPARE_SMALLER;
	}
	ret = ret << 2;

	// GREEN
	if (c1[1] > c2[1])
	{
		ret += MARKERFIELD_COMPARE_GREATER;
	}
	else if (c1[1] == c2[1])
	{
		ret += MARKERFIELD_COMPARE_EQUAL;
	}
	else
	{
		ret += MARKERFIELD_COMPARE_SMALLER;
	}
	ret = ret << 2;

	// RED
	if (c1[2] > c2[2])
	{
		ret += MARKERFIELD_COMPARE_GREATER;
	}
	else if (c1[2] == c2[2])
	{
		ret += MARKERFIELD_COMPARE_EQUAL;
	}
	else
	{
		ret += MARKERFIELD_COMPARE_SMALLER;
	}
	ret = ret << 2;
	
	return ret;
}

Scalar MFHColor::getRGBValue(unsigned int v)
{
	return colors[v];
}
