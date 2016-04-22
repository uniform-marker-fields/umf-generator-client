#include "MFSColor.h"
#include <boost/algorithm/string.hpp>
//#include <stdio.h>
//#include <math.h>
using namespace cv;

/***********************************************************************
 * (DE)CONSTRUCTOR(S)
 **********************************************************************/
//MFSColor::MFSColor(unsigned int w, unsigned int h, unsigned int k, unsigned int k_t, unsigned int t, unsigned char sE, unsigned char sN, float cE, float cD, const char* c, string d): MFSquare(w, h, k, k_t, t, sE, sN, cE, cD, c)
MFSColor::MFSColor(unsigned int w, unsigned int h, unsigned int k, unsigned int k_t, unsigned int t, unsigned char sE, const char *cn, float cs, const char* c, string d): MFSquare(w, h, k, k_t, t, sE, cn, cs, c)
{
	initStart(d);
	
	init();
	
	initEnd();
}

/***********************************************************************
 * INITALIZATION METHODS
 **********************************************************************/

void MFSColor::init()
{
	initColors();
}

void MFSColor::initColors()
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

void MFSColor::checkAttributes()
{
	MFSquare::checkAttributes();
	checkColors();
}

void MFSColor::checkColors()
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

void MFSColor::checkNeighbors()
{
	Scalar v1, v2;
	for (unsigned int y = 0; y < height; y++)
	{
		unsigned long y_times_width = y * width;
		for (unsigned int x = 0; x < width; x++)
		{
			v1 = getRGBValue(dataField[y_times_width + x]);
			if (y < height-1)
			{
				v2 = getRGBValue(dataField[y_times_width+width + x]);
				incCostNeighbor(y_times_width + x, v1[0], y_times_width+width + x, v2[0]);
				incCostNeighbor(y_times_width + x, v1[1], y_times_width+width + x, v2[1]);
				incCostNeighbor(y_times_width + x, v1[2], y_times_width+width + x, v2[2]);
			}
			if (x < width-1)
			{
				v2 = getRGBValue(dataField[y_times_width + x+1]);
				incCostNeighbor(y_times_width + x, v1[0], y_times_width + x+1, v2[0]);
				incCostNeighbor(y_times_width + x, v1[1], y_times_width + x+1, v2[1]);
				incCostNeighbor(y_times_width + x, v1[2], y_times_width + x+1, v2[2]);
			}
		}
	}
}

void MFSColor::recalculateCost()
{
	MFSquare::recalculateCost();
	checkNeighbors();
}

/***********************************************************************
 * IMPLEMENTED PURE VIRTUAL FUNCTIONS FROM MARKER FIELD
 **********************************************************************/
const char* MFSColor::getClassName()
{
	return "Color Square Marker Field";
}


/***********************************************************************
 * IMPLEMENTED PURE VIRTUAL FUNCTIONS FROM MARKER FIELD SQUARE
 **********************************************************************/

unsigned char MFSColor::compareTwo(unsigned int first, unsigned int second) {
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

Scalar MFSColor::getRGBValue(unsigned int v)
{
	return colors[v];
}
