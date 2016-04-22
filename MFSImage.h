#include "MFSquare.h"
//#include "opencv2/core/core.hpp"

class MFSImage: public MFSquare
{
	protected:
		/***************************************************************
		 * ATTRIBUTES
		 **************************************************************/
		
		vector<Scalar> colors;
		
		vector<Scalar> goalImage;
		
		vector<bool> goalMask;
		
		unsigned sCheckSize;
		vector<double> similarityCheck;
		
		vector<Scalar> bluredImage;
		
		/***************************************************************
		 * INITALIZATION METHODS
		 **************************************************************/
		
		void init(string img, const char* sCheck);
		
		void initColors();
		
		void initImage(string img);
		
		void initSimilarityCheck(const char* sCheck);
		
		/***************************************************************
		 * CHECK ATTRIBUTES
		 **************************************************************/
		
		virtual void checkAttributes();
		
		void checkColors();
		
		/***************************************************************
		 * COST AND COFLICT CALCULATIONS
		 **************************************************************/
		
		void checkNeighbors();
		
		void checkDifference();
		
		void incCostDiffEuclidean(unsigned long i);
		
		void recalculateCost();

		/***************************************************************
		 * IMPLEMENTED PURE VIRTUAL FUNCTIONS FROM MFSquare
		 **************************************************************/
		 
		unsigned char compareTwo(unsigned int first, unsigned int second);
		
		virtual Scalar getRGBValue(unsigned int v);
		
	public:
		/***************************************************************
		 * (DE)CONSTRUCTOR(S)
		 **************************************************************/
		//MFSImage(unsigned int w, unsigned int h, unsigned int k, unsigned int k_t, unsigned int t, unsigned char sE, unsigned char sN, float cE, float cD, const char* c, const char* ia, string d, string img, const char* sCheck = "");
		MFSImage(unsigned int w, unsigned int h, unsigned int k, unsigned int k_t, unsigned int t, unsigned char sE, const char *cn, float cs, const char* c, const char* ia, string d, string img, const char* sCheck = "");

		/***************************************************************
		 * IMPLEMENTED VIRTUAL FUNCTIONS FROM MARKERFIELD
		 **************************************************************/
		const char* getClassName();
		
		virtual void show();
};
