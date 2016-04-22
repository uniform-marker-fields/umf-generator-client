#include "MFSquare.h"
//#include "opencv2/core/core.hpp"

class MFSGrayscale: public MFSquare
{
	protected:
		
		/***************************************************************
		 * COST AND COFLICT CALCULATIONS
		 **************************************************************/
		
		void checkNeighbors();

		/***************************************************************
		 * IMPLEMENTED PURE VIRTUAL FUNCTIONS FROM MFSquare
		 **************************************************************/
		
		void recalculateCost();
		 
		unsigned char compareTwo(unsigned int first, unsigned int second);
		
		virtual Scalar getRGBValue(unsigned int v);
		
	public:
		/***************************************************************
		 * (DE)CONSTRUCTOR(S)
		 **************************************************************/
		//MFSGrayscale(unsigned int w, unsigned int h, unsigned int k, unsigned int k_t, unsigned int t, unsigned char sE, unsigned char sN, float cE, float cD, string d);
		MFSGrayscale(unsigned int w, unsigned int h, unsigned int k, unsigned int k_t, unsigned int t, unsigned char sE, const char *cn, float cs, string d);

		/***************************************************************
		 * IMPLEMENTED PURE VIRTUAL FUNCTIONS FROM MARKERFIELD
		 **************************************************************/
		const char* getClassName();
};
