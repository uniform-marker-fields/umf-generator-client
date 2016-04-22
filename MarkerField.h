#ifndef __MARKERFIELD_H_INCLUDED__
#define __MARKERFIELD_H_INCLUDED__

#include <string.h>
#include <iostream>
#include <stdlib.h>
#include <boost/algorithm/string.hpp>
//#include <map>
#include "opencv2/core/core.hpp"
using namespace cv;
using namespace std;

#define PRINT_MUTATION_OUTPUT 1

#define MARKERFIELD_DEF_WINSIZE 4
#define MARKERFIELD_DEF_SQUARE_PIXELS 10

#define MARKERFIELD_SHOW_WINDOW 1
#define MARKERFIELD_NOT_SHOW_WINDOW 0

#define MARKERFIELD_COMPARE_SMALLER 0
#define MARKERFIELD_COMPARE_EQUAL 1
#define MARKERFIELD_COMPARE_GREATER 2
#define MARKERFIELD_COMPARE_X 3

#define MARKERFIELD_PEN_NEIGHBORS_SAME 8.0
#define MARKERFIELD_PEN_NEIGHBORS_DIFF 5.0
#define MARKERFIELD_PEN_CONFLICTS 100.0
#define MARKERFIELD_PEN_SELF_CONFLICTS 6.0

#define MARKERFIELD_MUTATE_NUMBER 2
#define MARKERFIELD_MUTATE_MAX 10
#define MARKERFIELD_MUTATE_MAX_ATTEMPTS 50

#define MARKERFIELD_OTHER_METHOD 2
#define MARKERFIELD_CONFLICT_CHANGE 2

#define MARKERFIELD_FIELD_SEPARATOR ','

#define MF_COLOR_RGB_HEXA_LENGTH 7
#define MF_COLOR_RGB_HEXA_START '#'
#define MF_COLOR_RGB_SEPARATOR ';'

#define TYPE_TORUS_BIT 1
#define TYPE_COLOR_BIT 2
#define TYPE_RANGE_BIT_START 2
#define TYPE_RANGE_BITS 252
#define TYPE_RANGE_START 2

#define MODULE_TYPE_SQUARE 0
#define MODULE_TYPE_HEXA 1

#define furound(x) floor(((x)+0.5))

// 48 ASCII of 0, 57 ASCII of 9, 65 ASCII of A, 90 ASCII of Z
#define ASCII_0 48
#define ASCII_9 57
#define ASCII_a 97
#define ASCII_z 122
#define ASCII_a_VALUE 87
#define getNumericValue(C, R)	(((C) >= ASCII_0) && ((C) <= ASCII_9) ? \
			((C) > (R)+ASCII_0 ? 0 : (C) - ASCII_0) : \
			(((C) >= ASCII_a) && ((C) <= ASCII_z) ? \
				((C) > (R)+ASCII_a_VALUE ? 0 : (C) - ASCII_a_VALUE) : 0))
#define getTextValue(D) (D < 10 ? D + ASCII_0 : D + ASCII_a_VALUE)

/*struct coordinates
{
	unsigned int x;
	unsigned int y;
	
	//test(const unsigned int& valx = 0, const unsigned int& valy = 0) : x(valx), y(valy) {}

	bool operator<(const coordinates& rhs) const
	{
		return (y == rhs.y ? x < rhs.x : y < rhs.y);
	}

	bool operator==(const coordinates& rhs) const
	{
		return (x == rhs.x && y == rhs.y);
	}
};*/

struct tCostNeighbor
{
	unsigned char leftBound;
	unsigned char rightBound;
	float leftCost;
	float rightCost;
	
	float diffCost;
	unsigned char diffBound;
};

class MarkerFieldException: public exception
{
	const char* msg;
	
	virtual const char* what() const throw()
	{
		return msg;
	}
	
	public:
	MarkerFieldException(const char *msg): msg(msg)
	{
	}
};

template<class T> unsigned int levenshtein_distance(const T &s1, const T & s2)
{
	const size_t len1 = s1.size(), len2 = s2.size();
	vector<unsigned int> col(len2+1), prevCol(len2+1);

	for (unsigned int i = 0; i < prevCol.size(); i++)
		prevCol[i] = i;
	for (unsigned int i = 0; i < len1; i++)
	{
		col[0] = i+1;
		for (unsigned int j = 0; j < len2; j++)
			col[j+1] = min( min( 1 + col[j], 1 + prevCol[1 + j]), prevCol[j] + (s1[i]==s2[j] ? 0 : 1) );
		col.swap(prevCol);
	}
	return prevCol[len2];
}

class MarkerField
{
	protected:
	
		/***************************************************************
		 * STANDARD ATRIBUTES FOR A MARKER FIELD
		 **************************************************************/
		unsigned int width;
		unsigned int height;
		unsigned long size;
		unsigned int kernelSize;
		unsigned long kernelType;
		string colorsText;
		string imgAlgText;
		
		// derived from type:
		unsigned int range;
		unsigned int typeTorus;
		unsigned int typeColor;
		
		// fields
		unsigned int* dataField;
		float* costField;
		
		// cost settings
		unsigned char setEqual;
		string costNeighborsText;
		vector<tCostNeighbor> costNeighbors;
		
		float costSimilarity;
	
		// calculated
		unsigned int conflictCount;
		float cost;
		vector<unsigned long> conflicts; // WARNING! NOT SORTED
		
		// mutation info
		unsigned int attemptedMutations;
		unsigned int successfulMutations;
		unsigned int noMutations;
		unsigned int noConflictChanges;
		
		// kernel info
		unsigned int kernelModuleCount;
		
		// helpers in mutation
		vector<unsigned long> worstPoints;
		unsigned int mutateMax;
		
		// visualizing cost field
		float maxCostForNeighbors;
		unsigned int numberOfNeighbors;
		
		float rgbCostGoodThreshold;
		float rgbCostBadThreshold;
		
		/***************************************************************
		 * (DE)CONSTRUCTOR(S)
		 **************************************************************/
		//MarkerField(unsigned int w = 0, unsigned int h = 0, unsigned int k = 0, unsigned int kt = 0, unsigned int t = 0, const char* c = "", const char* ia = "", unsigned int kmc = 0, unsigned char sE = 0, unsigned char sN = 128, float cE = 8.0, float cD = 10.0);
		MarkerField(unsigned int w = 0, unsigned int h = 0, unsigned int k = 0, unsigned int kt = 0, unsigned int t = 0, unsigned int nON = 0, const char* c = "", const char* ia = "", unsigned int kmc = 0, unsigned char sE = 0, const char* cn = "", float cs = 1.0);//unsigned char sN = 128, float cE = 8.0, float cD = 10.0);
		
		/***************************************************************
		 * INIT FUNCTIONS
		 **************************************************************/
		
		void initCostNeighbors();
		
		virtual void initEnd();
		
		virtual void initKernelModules() = 0;
		
		virtual void initPassesTemplate() = 0;
		
		void initRGBCostThresholds();
		
		virtual void initStart(string d);
		
		virtual void readData(string d);
		
		/***************************************************************
		 * CHECK ATTRIBUTES
		 **************************************************************/
		
		virtual void checkAttributes();
		
		void checkKernelSize();
	
		virtual void checkKernelType();
		
		virtual void checkRange();
	
		void checkWidthHeight();
	
	public:
		
		~MarkerField();
	
		/***************************************************************
		 * GETTER METHODS FOR STANDARD ATTRIBUTES
		 **************************************************************/
		 
		string getColorsText();
		
		unsigned int getConflictCount();
		
		float getCost();
		
		unsigned int getHeight();
		
		string getImgAlgText();
		
		unsigned int getKernelSize();
		
		unsigned long getKernelType();
		
		unsigned int getType();
		
		unsigned int getWidth();
		
		/***************************************************************
		 * SOME STATIC METHODS DEALING WITH COMPLEX TYPE FORM
		 **************************************************************/
		 
		static unsigned int getRangeFromType(unsigned int t);
		
		static unsigned int getSynthesisedType(unsigned int t, unsigned int c, unsigned int r);
		
		static unsigned int getTypeColorFromType(unsigned int t);
		
		static unsigned int getTypeTorusFromType(unsigned int t);
		
		static int fillWithRandom(unsigned int **data, unsigned long size, unsigned int range);
		
		/***************************************************************
		 * COST AND CONFLICT CALCULATIONS
		 **************************************************************/
		 
		virtual Scalar getRGBCostValue(float c);
		
		virtual void incCostForConflict(unsigned long index, bool selfConflict = false);
		
		virtual void incCostNeighbor(unsigned long i1, unsigned char v1, unsigned long i2, unsigned char v2);
		
		virtual void incCostNeighbor(unsigned long i1, unsigned char v1, unsigned long i2, unsigned char v2, bool *conflictFlag);
		
		virtual void recalculateCost();
		
		virtual void checkForConflicts() = 0;
		
		/***************************************************************
		 * SELECTING POINTS FOR CHANGE
		 **************************************************************/

		virtual void setWorstPoints(unsigned int max);

		virtual void setConflictPoints(unsigned int max);
		
		/***************************************************************
		 * MAKING THE CHANGES
		 **************************************************************/
		
		/**
		 * Performs mutation in the marker field
		 */
		//virtual void mutate() = 0;
		virtual void mutate();
		
		/***************************************************************
		 * PURE VIRTUAL METHODS OF A MARKER FIELD
		 **************************************************************/
		 
		/**
		 * Returns class name
		 */
		virtual const char* getClassName() = 0;
		
		/**
		 * Returns kernel modules
		 */
		virtual vector<long> *getKernelModules(unsigned long i) = 0;
		
		/**
		 * Shows the results in a visual form (example an OpenCV window)
		 */
		virtual void show() = 0;
		
		/***************************************************************
		 * INFORMATIONS
		 **************************************************************/
		 
		/**
		 * Returns a readable data
		 */
		virtual string getReadableData();
		
		/**
		 * Prints out some information
		 */
		void printStatistics(bool brief = false);
};

#endif  // __MARKERFIELD_H_INCLUDED__
