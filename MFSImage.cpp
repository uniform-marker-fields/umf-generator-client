#include "MFSImage.h"
#include <boost/algorithm/string.hpp>
//#include <stdio.h>
//#include <math.h>
using namespace cv;
using namespace boost;

/***********************************************************************
 * (DE)CONSTRUCTOR(S)
 **********************************************************************/
//MFSImage::MFSImage(unsigned int w, unsigned int h, unsigned int k, unsigned int k_t, unsigned int t, unsigned char sE, unsigned char sN, float cE, float cD, const char* c, const char* ia, string d, string img, const char* sCheck): MFSquare(w, h, k, k_t, t, sE, sN, cE, cD, c, ia)
MFSImage::MFSImage(unsigned int w, unsigned int h, unsigned int k, unsigned int k_t, unsigned int t, unsigned char sE, const char *cn, float cs, const char* c, const char* ia, string d, string img, const char* sCheck): MFSquare(w, h, k, k_t, t, sE, cn, cs, c, ia)
{
	initStart(d);
	
	init(img, sCheck);
	
	initEnd();
}

/***********************************************************************
 * INITALIZATION METHODS
 **********************************************************************/

void MFSImage::init(string img, const char* sCheck)
{
	initColors();
	initImage(img);
	initSimilarityCheck(sCheck);
}

void MFSImage::initColors()
{
	colors = vector<Scalar>();
	unsigned char r, g, b;
	
	// colors added...
	if (typeColor == 1)
	{
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
	else
	{
		for (unsigned int i = 0; i < range; i++)
		{
			colors.push_back(Scalar (i * 255 / (range-1), i * 255 / (range-1), i * 255 / (range-1)));
		}
	}
}

void MFSImage::initImage(string img)
{	
	goalImage = vector<Scalar>(width * height, Scalar(0,0,0));
	goalMask = vector<bool>(width * height, false);
	
	vector<char> imageData(img.begin(), img.end());
	Mat original = imdecode(Mat(imageData), CV_LOAD_IMAGE_COLOR);
	//imshow("xx", original);
	//waitKey();
	//cout << colors.size() << endl;
	//cout << original.cols << " " << original.rows << endl;
	double ratio = 1.0 * width / height;
	double ratio_o = 1.0 * original.cols / original.rows;
	//cout << ratio << " " << ratio_o << endl;
	bool use_width = ratio_o > ratio;
	bool use_original = (use_width ? (unsigned) original.cols <= width : (unsigned) original.rows <= height);
	unsigned new_width = width, new_height = height;
	if (use_width)
	{
		new_height = furound(width / ratio_o);
	}
	else
	{
		new_width = furound(height * ratio_o);
	}
	//cout << new_width << " " << new_height << endl;
	if (use_original)
	{
		unsigned x_s = (use_width ? 0 : (width-new_width)/2);
		unsigned y_s = (use_width ? (height-new_height)/2 : 0);
		for (unsigned int x = 0; x < new_width; x++)
		{
			for (unsigned int y = 0; y < new_height; y++)
			{
				goalImage[x_s+x + (y_s+y)*width] = Scalar(original.at<Vec3b>(y * original.rows / new_height, x * original.cols / new_width));
				goalMask[x_s+x + (y_s+y)*width] = true;
			}
		}
	}
	else
	{
		unsigned x_s = (use_width ? 0 : (width-new_width)/2);
		unsigned y_s = (use_width ? (height-new_height)/2 : 0);
		for (unsigned int x = 0; x < new_width; x++)
		{
			for (unsigned int y = 0; y < new_height; y++)
			{
				unsigned int x2_from = x * (unsigned) original.cols / new_width;
				unsigned int x2_to = (x+1) * (unsigned) original.cols / new_width;
				unsigned int y2_from = y * (unsigned) original.rows / new_height;
				unsigned int y2_to = (y+1) * (unsigned) original.rows / new_height;
				unsigned long r = 0, g = 0, b = 0;
				unsigned int div = (x2_to - x2_from) * (y2_to - y2_from);
				for (unsigned int y2 = y2_from; y2 < y2_to; y2++)
				{
					for (unsigned int x2 = x2_from; x2 < x2_to; x2++)
					{
						Scalar s_tmp = Scalar(original.at<Vec3b>(y2, x2));
						b += s_tmp[0];
						g += s_tmp[1];
						r += s_tmp[2];
					}
				}
				
				goalImage[x_s+x + (y_s+y)*width] = Scalar(b / div, g / div, r /div);
				goalMask[x_s+x + (y_s+y)*width] = true;
			}
		}
	}
}

void MFSImage::initSimilarityCheck(const char* s)
{
	if (strlen(s) == 0)
	{
		similarityCheck.push_back(1);
	}
	else
	{
		vector<string> strs;
		split(strs,s,is_any_of(";"));
		
		sCheckSize = sqrtl(strs.size());
		double sum = 0.0;
		for (vector<string>::iterator it = strs.begin(); it != strs.end(); ++it)
		{
			similarityCheck.push_back(atof((*it).c_str()));
			sum += atof((*it).c_str());
		//	cout << (*it) << endl;
		}
		//cout << endl;
		
		if (sCheckSize % 2 == 0 || sCheckSize*sCheckSize != strs.size())
		{
			if (sCheckSize % 2 == 0)
			{
				sCheckSize++;
			}
			else
			{
				sCheckSize++;
				sCheckSize++;
			}
			//cout << sCheckSize*sCheckSize << endl << endl;
			similarityCheck.insert(similarityCheck.end(),sCheckSize*sCheckSize-strs.size(), 0.0);
		}
		
		for (vector<double>::iterator it = similarityCheck.begin(); it != similarityCheck.end(); ++it)
		{
		//	cout << (*it) << endl;
			(*it) /= sum;
		}
		//cout << endl;
		
		reverse(similarityCheck.begin(), similarityCheck.end());
		
		
		//for (vector<double>::iterator it = similarityCheck.begin(); it != similarityCheck.end(); ++it)
		//{
		//	cout << (*it) << endl;
		//}
		//cout << endl;
	}
}

/***************************************************************
 * CHECK ATTRIBUTES
 **************************************************************/

void MFSImage::checkAttributes()
{
	MFSquare::checkAttributes();
	checkColors();
}

void MFSImage::checkColors()
{
	string s = colorsText;
	if (typeColor == 1 && s.length() != range * MF_COLOR_RGB_HEXA_LENGTH + range - 1)
	{
		throw MarkerFieldException("Colors' text does not fit with range!");
	}
}

/***********************************************************************
 * COST CALCULATIONS
 **********************************************************************/

void MFSImage::checkDifference()
{
	
	bluredImage.clear();
	
	for (unsigned long i = 0; i < width * height; i++)
	{
		if (goalMask[i])
		{
			incCostDiffEuclidean(i);
		}
	}
}

void MFSImage::checkNeighbors()
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

void MFSImage::incCostDiffEuclidean(unsigned long i)
{
	Scalar actual = Scalar(0.0, 0.0, 0.0);
	unsigned long halfSize = sCheckSize/2;
	//unsigned long tmpI = i;
	Scalar tmpS;
	
	unsigned long Ix = i % width;
	unsigned long Iy = i / width;
	unsigned long Iy_width = Iy * width;
	unsigned long tmpIx = Ix;
	unsigned long tmpIy = Iy_width;
	
	for (unsigned y = 0; y < sCheckSize; y++)
	{
		tmpIy = Iy_width;
		if (Iy+y >= halfSize)
		{
			if (Iy+y+1 <= halfSize+height)
			{
				tmpIy = tmpIy + width * (y - halfSize);
			}
			/*else
			{
				tmpIy = (height-1) * width;
			}*/
		}
		unsigned long sCSize_y = sCheckSize*y;
		for (unsigned x = 0; x < sCheckSize; x++)
		{
			tmpIx = Ix;
			if (Ix+x >= halfSize)
			{
				if (Ix+x+1 <= halfSize+width)
				{
					tmpIx = tmpIx + x - halfSize;
				}
				/*else
				{
					tmpIx = width - 1;
				}*/
			}
			tmpS = getRGBValue(dataField[tmpIy + tmpIx]);
			actual[0] += tmpS[0] * similarityCheck[x+sCSize_y];
			actual[1] += tmpS[1] * similarityCheck[x+sCSize_y];
			actual[2] += tmpS[2] * similarityCheck[x+sCSize_y];
		}
	}
	bluredImage.push_back(actual);
	
	/*if (i / width <= sCheckSize/2-1 || i / width >= height-sCheckSize/2 || i % width <= sCheckSize/2-1 || i % width >= width-sCheckSize/2)
	{
		actual = getRGBValue(dataField[i]);
	}
	else
	{
		unsigned long tmpI = i - (sCheckSize/2) - (sCheckSize/2)*width;
		Scalar tmpS;
		for (unsigned y = 0; y < sCheckSize; y++)
		{
			unsigned long width_y = width*y;
			unsigned sCSize_y = sCheckSize*y;
			for (unsigned x = 0; x < sCheckSize; x++)
			{
				tmpS = getRGBValue(dataField[tmpI+x+width_y]);
				actual[0] += tmpS[0] * similarityCheck[x+sCSize_y];
				actual[1] += tmpS[1] * similarityCheck[x+sCSize_y];
				actual[2] += tmpS[2] * similarityCheck[x+sCSize_y];
			}
		}
		//cout << actual[0] << " " << actual[1] << " " << actual[2] << " " << endl;
	}*/
	Scalar goal = goalImage[i];
	
	float diffCost = costSimilarity * sqrt((actual[0] - goal[0])*(actual[0] - goal[0]) + (actual[1] - goal[1])*(actual[1] - goal[1]) + (actual[2] - goal[2])*(actual[2] - goal[2]) );
	
	cost += diffCost;
	costField[i] += diffCost;
}

void MFSImage::recalculateCost()
{
	MFSquare::recalculateCost();
	checkDifference();
	checkNeighbors();
}

/***********************************************************************
 * IMPLEMENTED VIRTUAL FUNCTIONS FROM MARKER FIELD
 **********************************************************************/
const char* MFSImage::getClassName()
{
	return "Image Square Marker Field";
}

void MFSImage::show()
{
	
	Mat image = Mat::zeros(height * MARKERFIELD_DEF_SQUARE_PIXELS, width * MARKERFIELD_DEF_SQUARE_PIXELS, CV_8UC3);
	
	for (unsigned long i = 0; i < goalImage.size(); i++)
	{
		rectangle(image, Rect((i % width) * MARKERFIELD_DEF_SQUARE_PIXELS, (i / width) * MARKERFIELD_DEF_SQUARE_PIXELS, MARKERFIELD_DEF_SQUARE_PIXELS, MARKERFIELD_DEF_SQUARE_PIXELS), bluredImage[i], CV_FILLED);
	}
	
	imshow("Goal image", image);
	
	MFSquare::show();
}

/***********************************************************************
 * IMPLEMENTED PURE VIRTUAL FUNCTIONS FROM MARKER FIELD SQUARE
 **********************************************************************/

unsigned char MFSImage::compareTwo(unsigned int first, unsigned int second) {
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

Scalar MFSImage::getRGBValue(unsigned int v)
{
	return colors[v];
}
