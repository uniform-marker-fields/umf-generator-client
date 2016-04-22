#ifndef __MARKERFIELD_HEXA_H_INCLUDED__
#define __MARKERFIELD_HEXA_H_INCLUDED__

#include "MarkerField.h"
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"

#define MARKERFIELD_HEXA_SYMMETRY_DIRECTIONS 6

#define MARKERFIELD_HEXA_LINE_START_LATER 0
#define MARKERFIELD_HEXA_LINE_START_SOONER 1

class MFHexagonal: public MarkerField
{
	protected:
		/***************************************************************
		 * ATTRIBUTES
		 **************************************************************/
		
		vector<vector<pair<long, long> > > passesTemplateLater;
		vector<vector<pair<long, long> > > passesTemplateSooner;
		
		vector<long> kernelModulesLater;
		vector<long> kernelModulesSooner;
		
		/***************************************************************
		 * (DE)CONSTRUCTOR(S)
		 **************************************************************/
		
		//MFHexagonal(unsigned int w = 0, unsigned int h = 0, unsigned int k = 0, unsigned int kt = 0, unsigned int t = 0, unsigned char sE = 0, unsigned char sN = 64, float cE = 8.0, float cD = 10.0, const char* c = "", const char* ia = "");
		MFHexagonal(unsigned int w = 0, unsigned int h = 0, unsigned int k = 0, unsigned int kt = 0, unsigned int t = 0, unsigned char sE = 0, const char* cn = "8.0,8.0:0;10.0,0.0:64", float cs = 1.0, const char* c = "", const char* ia = "");
		
		~MFHexagonal(); // @TODO check if this works..
		
		/***************************************************************
		 * INITALIZATION METHODS
		 **************************************************************/
		
		void initKernelModules();
		
		void initPassesTemplate();
		
		/***************************************************************
		 * CHECK ATTRIBUTES
		 **************************************************************/
	
		void checkKernelType();
		
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

		vector<long> *getKernelModules(unsigned long i);
		
		virtual void show();

		// @TODO review mutate and show
		//void mutate();
		
		/***************************************************************
		 * ORIENTATION IN THE HEXAGONAL MARKERFIELD
		 **************************************************************/
		
		enum HEXA_ORIENTATION {
			NORTHWEST,
			NORTHEAST,
			EAST,
			SOUTHEAST,
			SOUTHWEST,
			WEST
		};
		
		pair<long, bool> getAbsolutePosition(pair<long, bool> index, HEXA_ORIENTATION orientation, unsigned distance = 1);
		
		pair<long, bool> getRelativePosition(bool lineStartSooner, HEXA_ORIENTATION orientation, unsigned distance = 1);
};

#endif  //__MARKERFIELD_HEXA_H_INCLUDED__
