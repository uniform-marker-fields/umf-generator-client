#include "MFHexagonal.h"

/***************************************************************
 * (DE)CONSTRUCTOR(S)
 **************************************************************/

MFHexagonal::MFHexagonal(unsigned int w, unsigned int h, unsigned int k, unsigned int kt, unsigned int t, unsigned char sE, const char* cn, float cs, const char* c, const char* ia): MarkerField(w, h, k, kt, t, MARKERFIELD_HEXA_SYMMETRY_DIRECTIONS, c, ia, 1 + 3 * ((k+1) / 2) * ((k+1) / 2 + 1), sE, cn, cs)
{
}

MFHexagonal::~MFHexagonal()
{
}

/***************************************************************
 * INITALIZATION METHODS
 **************************************************************/

void MFHexagonal::initKernelModules()
{	
	kernelModulesLater.push_back(0);
	kernelModulesSooner.push_back(0);
	
	pair<long, bool> lInd1, lInd2, lInd3, lInd4, lInd5, lInd6;
	pair<long, bool> sInd1, sInd2, sInd3, sInd4, sInd5, sInd6;
	for (unsigned n = 1; n < (kernelSize+1)/2+1; n++)
	{
		lInd1 = getRelativePosition(false, NORTHWEST, n);
		lInd2 = getRelativePosition(false, NORTHEAST, n);
		lInd3 = getRelativePosition(false, EAST, n);
		lInd4 = getRelativePosition(false, SOUTHEAST, n);
		lInd5 = getRelativePosition(false, SOUTHWEST, n);
		lInd6 = getRelativePosition(false, WEST, n);
		
		sInd1 = getRelativePosition(true, NORTHWEST, n);
		sInd2 = getRelativePosition(true, NORTHEAST, n);
		sInd3 = getRelativePosition(true, EAST, n);
		sInd4 = getRelativePosition(true, SOUTHEAST, n);
		sInd5 = getRelativePosition(true, SOUTHWEST, n);
		sInd6 = getRelativePosition(true, WEST, n);
		
		for (unsigned i = 0; i < n; i++)
		{
			kernelModulesLater.push_back(lInd1.first);
			kernelModulesLater.push_back(lInd2.first);
			kernelModulesLater.push_back(lInd3.first);
			kernelModulesLater.push_back(lInd4.first);
			kernelModulesLater.push_back(lInd5.first);
			kernelModulesLater.push_back(lInd6.first);
			
			kernelModulesSooner.push_back(sInd1.first);
			kernelModulesSooner.push_back(sInd2.first);
			kernelModulesSooner.push_back(sInd3.first);
			kernelModulesSooner.push_back(sInd4.first);
			kernelModulesSooner.push_back(sInd5.first);
			kernelModulesSooner.push_back(sInd6.first);
			
			lInd1 = getAbsolutePosition(lInd1, EAST);
			lInd2 = getAbsolutePosition(lInd2, SOUTHEAST);
			lInd3 = getAbsolutePosition(lInd3, SOUTHWEST);
			lInd4 = getAbsolutePosition(lInd4, WEST);
			lInd5 = getAbsolutePosition(lInd5, NORTHWEST);
			lInd6 = getAbsolutePosition(lInd6, NORTHEAST);
			
			sInd1 = getAbsolutePosition(sInd1, EAST);
			sInd2 = getAbsolutePosition(sInd2, SOUTHEAST);
			sInd3 = getAbsolutePosition(sInd3, SOUTHWEST);
			sInd4 = getAbsolutePosition(sInd4, WEST);
			sInd5 = getAbsolutePosition(sInd5, NORTHWEST);
			sInd6 = getAbsolutePosition(sInd6, NORTHEAST);
		}
	}
}

void MFHexagonal::initPassesTemplate()
{
	passesTemplateLater = vector<vector<pair<long, long> > >(MARKERFIELD_HEXA_SYMMETRY_DIRECTIONS, vector<pair<long, long> >());
	passesTemplateSooner = vector<vector<pair<long, long> > >(MARKERFIELD_HEXA_SYMMETRY_DIRECTIONS, vector<pair<long, long> >());
	
	pair<long, bool> lInd1, lInd2, lInd3, lInd4, lInd5, lInd6;
	pair<long, bool> sInd1, sInd2, sInd3, sInd4, sInd5, sInd6;
	
	pair<long, bool> lInd1n, lInd2n, lInd3n, lInd4n, lInd5n, lInd6n;
	pair<long, bool> sInd1n, sInd2n, sInd3n, sInd4n, sInd5n, sInd6n;
	
	// check all levels
	for (unsigned n = kernelSize; n > 0; n--)
	{
		if (n % 2 == 1)
		{
			lInd1 = getRelativePosition(false, NORTHWEST, (n+1)/2);
			lInd2 = getRelativePosition(false, NORTHEAST, (n+1)/2);
			lInd3 = getRelativePosition(false, EAST, (n+1)/2);
			lInd4 = getRelativePosition(false, SOUTHEAST, (n+1)/2);
			lInd5 = getRelativePosition(false, SOUTHWEST, (n+1)/2);
			lInd6 = getRelativePosition(false, WEST, (n+1)/2);
			
			sInd1 = getRelativePosition(true, NORTHWEST, (n+1)/2);
			sInd2 = getRelativePosition(true, NORTHEAST, (n+1)/2);
			sInd3 = getRelativePosition(true, EAST, (n+1)/2);
			sInd4 = getRelativePosition(true, SOUTHEAST, (n+1)/2);
			sInd5 = getRelativePosition(true, SOUTHWEST, (n+1)/2);
			sInd6 = getRelativePosition(true, WEST, (n+1)/2);
			
			for (unsigned i = 0; i < n; i++)
			{
				if (i % 2 == 0)
				{
					lInd1n = getAbsolutePosition(lInd1, SOUTHEAST, 1);
					lInd2n = getAbsolutePosition(lInd2, SOUTHWEST, 1);
					lInd3n = getAbsolutePosition(lInd3, WEST, 1);
					lInd4n = getAbsolutePosition(lInd4, NORTHWEST, 1);
					lInd5n = getAbsolutePosition(lInd5, NORTHEAST, 1);
					lInd6n = getAbsolutePosition(lInd6, EAST, 1);
					
					sInd1n = getAbsolutePosition(sInd1, SOUTHEAST, 1);
					sInd2n = getAbsolutePosition(sInd2, SOUTHWEST, 1);
					sInd3n = getAbsolutePosition(sInd3, WEST, 1);
					sInd4n = getAbsolutePosition(sInd4, NORTHWEST, 1);
					sInd5n = getAbsolutePosition(sInd5, NORTHEAST, 1);
					sInd6n = getAbsolutePosition(sInd6, EAST, 1);
				}
				else
				{
					lInd1n = getAbsolutePosition(lInd1, NORTHEAST, 1);
					lInd2n = getAbsolutePosition(lInd2, EAST, 1);
					lInd3n = getAbsolutePosition(lInd3, SOUTHEAST, 1);
					lInd4n = getAbsolutePosition(lInd4, SOUTHWEST, 1);
					lInd5n = getAbsolutePosition(lInd5, WEST, 1);
					lInd6n = getAbsolutePosition(lInd6, NORTHWEST, 1);
					
					sInd1n = getAbsolutePosition(sInd1, NORTHEAST, 1);
					sInd2n = getAbsolutePosition(sInd2, EAST, 1);
					sInd3n = getAbsolutePosition(sInd3, SOUTHEAST, 1);
					sInd4n = getAbsolutePosition(sInd4, SOUTHWEST, 1);
					sInd5n = getAbsolutePosition(sInd5, WEST, 1);
					sInd6n = getAbsolutePosition(sInd6, NORTHWEST, 1);
				}
				
				passesTemplateLater[0].push_back(make_pair(lInd1.first, lInd1n.first));
				passesTemplateLater[1].push_back(make_pair(lInd2.first, lInd2n.first));
				passesTemplateLater[2].push_back(make_pair(lInd3.first, lInd3n.first));
				passesTemplateLater[3].push_back(make_pair(lInd4.first, lInd4n.first));
				passesTemplateLater[4].push_back(make_pair(lInd5.first, lInd5n.first));
				passesTemplateLater[5].push_back(make_pair(lInd6.first, lInd6n.first));
				
				passesTemplateSooner[0].push_back(make_pair(sInd1.first, sInd1n.first));
				passesTemplateSooner[1].push_back(make_pair(sInd2.first, sInd2n.first));
				passesTemplateSooner[2].push_back(make_pair(sInd3.first, sInd3n.first));
				passesTemplateSooner[3].push_back(make_pair(sInd4.first, sInd4n.first));
				passesTemplateSooner[4].push_back(make_pair(sInd5.first, sInd5n.first));
				passesTemplateSooner[5].push_back(make_pair(sInd6.first, sInd6n.first));
				
				lInd1 = lInd1n;
				lInd2 = lInd2n;
				lInd3 = lInd3n;
				lInd4 = lInd4n;
				lInd5 = lInd5n;
				lInd6 = lInd6n;
				
				sInd1 = sInd1n;
				sInd2 = sInd2n;
				sInd3 = sInd3n;
				sInd4 = sInd4n;
				sInd5 = sInd5n;
				sInd6 = sInd6n;
			}
		}
		else
		{
			lInd1 = getRelativePosition(false, NORTHWEST, n/2);
			lInd2 = getRelativePosition(false, NORTHEAST, n/2);
			lInd3 = getRelativePosition(false, EAST, n/2);
			lInd4 = getRelativePosition(false, SOUTHEAST, n/2);
			lInd5 = getRelativePosition(false, SOUTHWEST, n/2);
			lInd6 = getRelativePosition(false, WEST, n/2);
			
			sInd1 = getRelativePosition(true, NORTHWEST, n/2);
			sInd2 = getRelativePosition(true, NORTHEAST, n/2);
			sInd3 = getRelativePosition(true, EAST, n/2);
			sInd4 = getRelativePosition(true, SOUTHEAST, n/2);
			sInd5 = getRelativePosition(true, SOUTHWEST, n/2);
			sInd6 = getRelativePosition(true, WEST, n/2);
			
			for (unsigned i = 0; i < n/2; i++)
			{
				lInd1n = getAbsolutePosition(lInd1, EAST, 1);
				lInd2n = getAbsolutePosition(lInd2, SOUTHEAST, 1);
				lInd3n = getAbsolutePosition(lInd3, SOUTHWEST, 1);
				lInd4n = getAbsolutePosition(lInd4, WEST, 1);
				lInd5n = getAbsolutePosition(lInd5, NORTHWEST, 1);
				lInd6n = getAbsolutePosition(lInd6, NORTHEAST, 1);
				
				sInd1n = getAbsolutePosition(sInd1, EAST, 1);
				sInd2n = getAbsolutePosition(sInd2, SOUTHEAST, 1);
				sInd3n = getAbsolutePosition(sInd3, SOUTHWEST, 1);
				sInd4n = getAbsolutePosition(sInd4, WEST, 1);
				sInd5n = getAbsolutePosition(sInd5, NORTHWEST, 1);
				sInd6n = getAbsolutePosition(sInd6, NORTHEAST, 1);
				
				passesTemplateLater[0].push_back(make_pair(lInd1.first, lInd1n.first));
				passesTemplateLater[1].push_back(make_pair(lInd2.first, lInd2n.first));
				passesTemplateLater[2].push_back(make_pair(lInd3.first, lInd3n.first));
				passesTemplateLater[3].push_back(make_pair(lInd4.first, lInd4n.first));
				passesTemplateLater[4].push_back(make_pair(lInd5.first, lInd5n.first));
				passesTemplateLater[5].push_back(make_pair(lInd6.first, lInd6n.first));
				
				passesTemplateSooner[0].push_back(make_pair(sInd1.first, sInd1n.first));
				passesTemplateSooner[1].push_back(make_pair(sInd2.first, sInd2n.first));
				passesTemplateSooner[2].push_back(make_pair(sInd3.first, sInd3n.first));
				passesTemplateSooner[3].push_back(make_pair(sInd4.first, sInd4n.first));
				passesTemplateSooner[4].push_back(make_pair(sInd5.first, sInd5n.first));
				passesTemplateSooner[5].push_back(make_pair(sInd6.first, sInd6n.first));
				
				lInd1 = lInd1n;
				lInd2 = lInd2n;
				lInd3 = lInd3n;
				lInd4 = lInd4n;
				lInd5 = lInd5n;
				lInd6 = lInd6n;
				
				sInd1 = sInd1n;
				sInd2 = sInd2n;
				sInd3 = sInd3n;
				sInd4 = sInd4n;
				sInd5 = sInd5n;
				sInd6 = sInd6n;
			}
		}
	}
}

/***************************************************************
 * CHECK ATTRIBUTES
 **************************************************************/

void MFHexagonal::checkKernelType()
{
}

/***********************************************************************
 * COST CALCULATIONS
 **********************************************************************/

void MFHexagonal::checkForConflicts()
{
	map<string, unsigned long> uniqueWindows;
	conflicts.clear();
	
	unsigned long tmpIndex;
	string tmpEdgeVal = "";
	
	for (unsigned y = 1; y < height-1; y++)
	{
		unsigned long y_times_width = y * width;
		for (unsigned x = 1; x < width-1; x++)
		{
			tmpIndex = x + y_times_width;
			
			vector<string> tmpPassVal;
			if ((y & 1) == MARKERFIELD_HEXA_LINE_START_LATER)
			{
				for (vector<vector<pair<long, long> > >::iterator pass = passesTemplateLater.begin(); pass != passesTemplateLater.end(); ++pass)
				{
					tmpEdgeVal = "";
					for (vector<pair<long, long> >::iterator p = pass->begin(); p != pass->end(); ++p)
					{
						tmpEdgeVal += compareTwo(dataField[tmpIndex + p->first], dataField[tmpIndex + p->second]);
					}
					
					tmpPassVal.push_back(tmpEdgeVal);
				}
			}
			else
			{
				for (vector<vector<pair<long, long> > >::iterator pass = passesTemplateSooner.begin(); pass != passesTemplateSooner.end(); ++pass)
				{
					tmpEdgeVal = "";
					for (vector<pair<long, long> >::iterator p = pass->begin(); p != pass->end(); ++p)
					{
						tmpEdgeVal += compareTwo(dataField[tmpIndex + p->first], dataField[tmpIndex + p->second]);
					}
					
					tmpPassVal.push_back(tmpEdgeVal);
				}
			}
			
			vector<string> tmpPassValForUse;
			tmpPassValForUse.push_back(tmpPassVal[0] + tmpPassVal[1] + tmpPassVal[2] + tmpPassVal[3] + tmpPassVal[4] + tmpPassVal[5]);
			tmpPassValForUse.push_back(tmpPassVal[1] + tmpPassVal[2] + tmpPassVal[3] + tmpPassVal[4] + tmpPassVal[5] + tmpPassVal[0]);
			tmpPassValForUse.push_back(tmpPassVal[2] + tmpPassVal[3] + tmpPassVal[4] + tmpPassVal[5] + tmpPassVal[0] + tmpPassVal[1]);
			tmpPassValForUse.push_back(tmpPassVal[3] + tmpPassVal[4] + tmpPassVal[5] + tmpPassVal[0] + tmpPassVal[1] + tmpPassVal[2]);
			tmpPassValForUse.push_back(tmpPassVal[4] + tmpPassVal[5] + tmpPassVal[0] + tmpPassVal[1] + tmpPassVal[2] + tmpPassVal[3]);
			tmpPassValForUse.push_back(tmpPassVal[5] + tmpPassVal[0] + tmpPassVal[1] + tmpPassVal[2] + tmpPassVal[3] + tmpPassVal[4]);
			
			//cout << tmpIndex / width << ", " << tmpIndex % width << ": " << endl;
			//cout << ".." << readable(tmpPassVal[0] + tmpPassVal[1] + tmpPassVal[2] + tmpPassVal[3] + tmpPassVal[4] + tmpPassVal[5]) << endl;
			//cout << ".." << readable(tmpPassVal[1] + tmpPassVal[2] + tmpPassVal[3] + tmpPassVal[4] + tmpPassVal[5] + tmpPassVal[0]) << endl;
			//cout << ".." << readable(tmpPassVal[2] + tmpPassVal[3] + tmpPassVal[4] + tmpPassVal[5] + tmpPassVal[0] + tmpPassVal[1]) << endl;
			//cout << ".." << readable(tmpPassVal[3] + tmpPassVal[4] + tmpPassVal[5] + tmpPassVal[0] + tmpPassVal[1] + tmpPassVal[2]) << endl;
			//cout << ".." << readable(tmpPassVal[4] + tmpPassVal[5] + tmpPassVal[0] + tmpPassVal[1] + tmpPassVal[2] + tmpPassVal[3]) << endl;
			//cout << ".." << readable(tmpPassVal[5] + tmpPassVal[0] + tmpPassVal[1] + tmpPassVal[2] + tmpPassVal[3] + tmpPassVal[4]) << endl;
			
			
			for (vector<string>::iterator c = tmpPassValForUse.begin(); c != tmpPassValForUse.end(); ++c)
			{
				if (uniqueWindows.find(*c) == uniqueWindows.end())
				{
					uniqueWindows[(*c)] = tmpIndex;
				}
				else if (find(conflicts.begin(), conflicts.end(), tmpIndex) == conflicts.end())
				{
					conflicts.push_back(tmpIndex);
				
					if (tmpIndex != uniqueWindows[(*c)])
					{
						incCostForConflict(tmpIndex);
						
						if (find(conflicts.begin(), conflicts.end(), uniqueWindows[(*c)]) == conflicts.end())
						{
							conflicts.push_back(uniqueWindows[(*c)]);
							incCostForConflict(uniqueWindows[(*c)]);
						}
						
						break;
					}
					else
					{
						incCostForConflict(tmpIndex, true);
					}
				}
			}
		}
	}
	
	conflictCount = conflicts.size();
}

void MFHexagonal::recalculateCost()
{
	MarkerField::recalculateCost();
}

/***************************************************************
 * IMPLEMENTED PURE VIRTUAL FUNCTIONS FROM MARKERFIELD
 **************************************************************/

vector<long> *MFHexagonal::getKernelModules(unsigned long i)
{
	if (((i / width) & 1) == MARKERFIELD_HEXA_LINE_START_LATER)
	{
		return &kernelModulesLater;
	}
	else
	{
		return &kernelModulesSooner;
	}
}

void MFHexagonal::show()
{
	Mat image = Mat::zeros(height * MARKERFIELD_DEF_SQUARE_PIXELS, width * MARKERFIELD_DEF_SQUARE_PIXELS + MARKERFIELD_DEF_SQUARE_PIXELS/2, CV_8UC3);
    
	for (unsigned int y = 0; y < height; y++)
	{
		for (unsigned int x = 0; x < width; x++)
		{
			rectangle(image, Rect(x * MARKERFIELD_DEF_SQUARE_PIXELS + ((y & 1) == MARKERFIELD_HEXA_LINE_START_LATER ? MARKERFIELD_DEF_SQUARE_PIXELS/2 : 0), y * MARKERFIELD_DEF_SQUARE_PIXELS, MARKERFIELD_DEF_SQUARE_PIXELS, MARKERFIELD_DEF_SQUARE_PIXELS), getRGBValue(dataField[y*width + x]), CV_FILLED);
		}
	}
	
	for (vector<unsigned long>::iterator it=conflicts.begin(); it != conflicts.end(); ++it)
	{
		rectangle(image, Rect(((*it) % width) * MARKERFIELD_DEF_SQUARE_PIXELS+ ((((*it) / width) & 1) == MARKERFIELD_HEXA_LINE_START_LATER ? MARKERFIELD_DEF_SQUARE_PIXELS/2 : 0), ((*it) / width) * MARKERFIELD_DEF_SQUARE_PIXELS, MARKERFIELD_DEF_SQUARE_PIXELS, MARKERFIELD_DEF_SQUARE_PIXELS), Scalar(0,0,255), 2);
	}
	
	imshow("Marker field", image);
	
	Mat image2 = Mat::zeros(height * MARKERFIELD_DEF_SQUARE_PIXELS, width * MARKERFIELD_DEF_SQUARE_PIXELS + MARKERFIELD_DEF_SQUARE_PIXELS/2, CV_8UC3);
	
	for (unsigned int y = 0; y < height; y++)
	{
		for (unsigned int x = 0; x < width; x++)
		{
			rectangle(image2, Rect(x * MARKERFIELD_DEF_SQUARE_PIXELS + ((y & 1) == MARKERFIELD_HEXA_LINE_START_LATER ? MARKERFIELD_DEF_SQUARE_PIXELS/2 : 0), y * MARKERFIELD_DEF_SQUARE_PIXELS, MARKERFIELD_DEF_SQUARE_PIXELS, MARKERFIELD_DEF_SQUARE_PIXELS), Scalar(0, 0, costField[y * width + x] > 255.0 ? 255 : ceil(costField[y * width + x])), CV_FILLED);
		}
	}
	imshow("Marker field cost", image2);
	
	int ret = waitKey(30);
	if ((ret & 255) == 27)
	{
		ret = waitKey();
	}
}

// @TODO review mutate
/*void MFHexagonal::mutate()
{
	// init variables
	attemptedMutations++;
	unsigned int *whichOneToChoose;
	unsigned int *newValues;
	
	unsigned long size = width * height;
	unsigned long conflictedSize = kernelSize * kernelSize * conflicts.size();
	if (conflictedSize == 0)
	{
		conflictedSize = size;
	}
	
	unsigned int mutateHere;
	unsigned int mutateMaxHere;
	//noConflictChange = 0;
	if ((noConflictChanges / MARKERFIELD_OTHER_METHOD) % 2 == 0) {
		mutateHere = MARKERFIELD_MUTATE_NUMBER * (sqrtl(size) / 10 + 1) * (sqrtl(cost / size) / 10 + 1) / (noMutations == 0 ? 1 : noMutations);
		mutateMaxHere = MARKERFIELD_MUTATE_MAX * (sqrtl(size) / 10 + 1) * (sqrtl(cost / size) / 10 + 1) * (sqrtl(noMutations) == 0 ? 1 : sqrtl(noMutations));
		if (mutateMaxHere > conflictedSize) {
			mutateMaxHere = conflictedSize;
		}
		if (mutateMaxHere > size) {
			mutateMaxHere = size;
		}
		if (mutateHere < MARKERFIELD_MUTATE_NUMBER) {
			mutateHere = MARKERFIELD_MUTATE_NUMBER;
		}
	} else {
		mutateMaxHere = kernelSize*kernelSize;
		if (mutateMaxHere > conflictedSize) {
			mutateMaxHere = conflictedSize;
		}
		if (mutateMaxHere > size) {
			mutateMaxHere = size;
		}
		if (conflicts.size() < MARKERFIELD_CONFLICT_CHANGE) {
			if (conflicts.size() > 0) {
				mutateHere = mutateMaxHere / conflicts.size();
			} else {
				mutateHere = mutateMaxHere;
			}
		} else {
			mutateHere = mutateMaxHere / MARKERFIELD_CONFLICT_CHANGE;
		}
	}
	
	// get randoms
	fillWithRandom(&whichOneToChoose, mutateHere, mutateMaxHere);
	fillWithRandom(&newValues, mutateHere, range);
	if ((noConflictChanges / MARKERFIELD_OTHER_METHOD) % 2 == 0) {
		setWorstPoints(mutateMaxHere);
	} else {
		setConflictPoints(mutateMaxHere);
	}
	
	vector<unsigned long> oldConflicts = conflicts;
	unsigned int oldConflictCount = conflictCount;
	unsigned int *oldField = (unsigned int *) malloc(size * sizeof(unsigned int));
	memcpy(oldField, dataField, size * sizeof(unsigned int));
	float* oldcostField = (float *) malloc(size * sizeof(float));
	memcpy(oldcostField, costField, size * sizeof(float));
	float oldActualPenalty = cost;
	
	if (attemptedMutations % 2 == 0)
	{
			dataField[worstPoints[0]] = newValues[mutateHere-2];
			dataField[worstPoints[1]] = newValues[mutateHere-1];
		for (unsigned int i = 0; i < mutateHere-2; i++) {
			dataField[worstPoints[whichOneToChoose[i]]] = newValues[i];
		}
	}
	else
	{
		for (unsigned int i = 0; i < mutateHere; i++) {
			dataField[worstPoints[whichOneToChoose[i]]] = newValues[i];
		}
	}
	
	recalculateCost();
	if (oldActualPenalty < cost) {
		conflicts = oldConflicts;
		memcpy(dataField, oldField, size * sizeof(unsigned int));
		memcpy(costField, oldcostField, size * sizeof(unsigned int));
		cost = oldActualPenalty;
		conflictCount = oldConflictCount;
		noMutations++;
	} else {
		successfulMutations++;
		noMutations = 0;
	}
	
	if (conflicts.size() == oldConflicts.size() && conflicts.size() != 0) {
		noConflictChanges++;
	} else {
		noConflictChanges = 0;
	}
	
	free(whichOneToChoose);
	free(newValues);
}*/

/***************************************************************
 * ORIENTATION IN THE HEXAGONAL MARKERFIELD
 **************************************************************/

pair<long, bool> MFHexagonal::getAbsolutePosition(pair<long, bool> index, HEXA_ORIENTATION orientation, unsigned distance)
{
	pair<long, bool> relative = getRelativePosition(index.second, orientation, distance);
	return make_pair(index.first+relative.first, relative.second);
}

pair<long, bool> MFHexagonal::getRelativePosition(bool lineStartSooner, HEXA_ORIENTATION orientation, unsigned distance)
{
	if (distance == 0)
	{
		return make_pair(0, lineStartSooner);
	}
	else
	{
		distance--;
		pair<long, bool> tmp;
		switch (orientation)
		{
			case NORTHWEST:
				tmp = getRelativePosition(!lineStartSooner, orientation, distance);
				return make_pair(tmp.first-(long)width+(lineStartSooner ? -1 : 0 ), tmp.second);
			case NORTHEAST:
				tmp = getRelativePosition(!lineStartSooner, orientation, distance);
				return make_pair(tmp.first-(long)width+(lineStartSooner ? 0 : 1 ), tmp.second);
			case EAST:
				tmp = getRelativePosition(lineStartSooner, orientation, distance);
				return make_pair(tmp.first+1, tmp.second);
			case SOUTHEAST:
				tmp = getRelativePosition(!lineStartSooner, orientation, distance);
				return make_pair(tmp.first+(long)width+(lineStartSooner ? 0 : 1 ), tmp.second);
			case SOUTHWEST:
				tmp = getRelativePosition(!lineStartSooner, orientation, distance);
				return make_pair(tmp.first+(long)width+(lineStartSooner ? -1 : 0 ), tmp.second);
			case WEST:
				tmp = getRelativePosition(lineStartSooner, orientation, distance);
				return make_pair(tmp.first-1, tmp.second);
			default:
				return make_pair(0, lineStartSooner);
		}
	}
}
