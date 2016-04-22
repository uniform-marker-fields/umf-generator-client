#ifndef __MARKERFIELD_SQUARE_H_INCLUDED__
#define __MARKERFIELD_SQUARE_H_INCLUDED__

#include "MarkerField.h"
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"

#define MARKERFIELD_SQUARE_SYMMETRY_DIRECTIONS 4

class MFSquare: public MarkerField
{
	protected:
		
		/***************************************************************
		 * ATTRIBUTES
		 **************************************************************/
		
		vector<vector<pair<unsigned long, unsigned long> > > passesTemplate;
		
		vector<long> kernelModules;
		
		/***************************************************************
		 * (DE)CONSTRUCTOR(S)
		 **************************************************************/
		
		MFSquare(unsigned int w = 0, unsigned int h = 0, unsigned int k = 0, unsigned int kt = 0, unsigned int t = 0, unsigned char sE = 0, const char* cn = "8.0,8.0:0;10.0,0.0:128", float cs = 1.0, const char* c = "", const char* ia = "");
		
		~MFSquare();
		
		/***************************************************************
		 * INITALIZATION METHODS
		 **************************************************************/
		
		void initKernelModules();
		
		void initPassesTemplate();
		
		/***************************************************************
		 * COST AND COFLICT CALCULATIONS
		 **************************************************************/
		
		void checkForConflicts();
		
		virtual void recalculateCost();
		
		/***************************************************************
		 * PURE VIRTUAL METHODS OF A MARKER FIELD SQUARE
		 **************************************************************/
		
		virtual unsigned char compareTwo(unsigned int first, unsigned int second) = 0;
		
		virtual Scalar getRGBValue(unsigned int v) = 0;
		
	public:

		/***************************************************************
		 * IMPLEMENTED PURE VIRTUAL FUNCTIONS FROM MARKERFIELD
		 **************************************************************/

		vector<long> *getKernelModules(unsigned long);
		
		virtual void show();

		// @TODO review mutate and show
		//void mutate();
};

#endif  //__MARKERFIELD_SQUARE_H_INCLUDED__
