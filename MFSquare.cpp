#include "MFSquare.h"
#include "stdio.h"



void readable(string s)
{
	for (unsigned i = 0; i < s.size(); i++)
	{
		switch (s[i])
		{
			case MARKERFIELD_COMPARE_EQUAL:
				cout << "=";
				break;
			case MARKERFIELD_COMPARE_SMALLER:
				cout << "<";
				break;
			case MARKERFIELD_COMPARE_GREATER:
				cout << ">";
				break;
		}
	}
}

/***************************************************************
 * (DE)CONSTRUCTOR(S)
 **************************************************************/

MFSquare::MFSquare(unsigned int w, unsigned int h, unsigned int k, unsigned int kt, unsigned int t, unsigned char sE, const char* cn, float cs, const char* c, const char* ia): MarkerField(w, h, k, kt, t, MARKERFIELD_SQUARE_SYMMETRY_DIRECTIONS, c, ia, k * k, sE, cn, cs)
{
}

MFSquare::~MFSquare()
{
}

/***************************************************************
 * INITALIZATION METHODS
 **************************************************************/

void MFSquare::initKernelModules()
{
	for (unsigned int i = 0; i < kernelSize; i++)
	{
		for (unsigned int j = 0; j < kernelSize; j++)
		{
			kernelModules.push_back(i * width + j);
		}
	}
}

void MFSquare::initPassesTemplate()
{
	passesTemplate = vector<vector<pair<unsigned long, unsigned long> > >(MARKERFIELD_SQUARE_SYMMETRY_DIRECTIONS, vector<pair<unsigned long, unsigned long> >());
	
	unsigned long k = kernelType; //(kernelType == 0 ? kernelSize * (kernelSize-1) / 2 : kernelType);
	
	// default value
	if (k == 0)
	{
		for (unsigned int i = 0; i < kernelSize * (kernelSize-1) / 2; i++)
		{
			k <<= 1;
			k |= 1;
		}
	}
	
	// check all levels
	for (unsigned n = 0; n < kernelSize-1; n++)
	{
		unsigned long n_2 = n/2;
		unsigned long ws_n_2 = kernelSize - 1 - n/2;
		
		// different approach for odd or even levels
		if (n % 2 == 0)
		{
			for (unsigned i = 0; i < kernelSize-n-1; i++)
			{
				// adding only the edges set in kernelType
				if ((k & 1) == 1)
				{
					passesTemplate[0].push_back(make_pair(n_2 * width + n_2 + i, n_2 * width + n_2 + i+1));
					passesTemplate[1].push_back(make_pair((n_2 + i) * width + ws_n_2, (n_2 + i+1) * width + ws_n_2));
					passesTemplate[2].push_back(make_pair(ws_n_2 * width + ws_n_2 - i, ws_n_2 * width + ws_n_2 - i-1));
					passesTemplate[3].push_back(make_pair((ws_n_2 - i) * width + n_2, (ws_n_2 - i-1) * width + n_2));
				}
				
				k >>= 1;
			}
			
		}
		else
		{
			for (unsigned i = 0; i < kernelSize-n-1; i++)
			{
				// adding only the edges set in kernelType
				if ((k & 1) == 1)
				{
					passesTemplate[0].push_back(make_pair(n_2 * width + n_2 + i+1, (n_2+1) * width + n_2 + i+1));
					passesTemplate[1].push_back(make_pair((n_2 + i+1) * width + ws_n_2, (n_2 + i+1) * width + ws_n_2-1));
					passesTemplate[2].push_back(make_pair(ws_n_2 * width + ws_n_2 - i-1, (ws_n_2-1) * width + ws_n_2 - i-1));
					passesTemplate[3].push_back(make_pair((ws_n_2 - i-1) * width + n_2, (ws_n_2 - i-1) * width + n_2+1));
				}
				
				k >>= 1;
			}
			
		}
	}
}

/***********************************************************************
 * COST CALCULATIONS
 **********************************************************************/

void MFSquare::checkForConflicts()
{
	map<string, unsigned long> uniqueWindows;
	conflicts.clear();
	
	unsigned long tmpIndex;
	string tmpEdgeVal = "";
	
	unsigned winDistance = 0;
	
	for (unsigned y = 0; y < height - kernelSize + 1; y++)
	{
		unsigned long y_times_width = y * width;
		for (unsigned x = 0; x < width - kernelSize + 1; x++)
		{
			tmpIndex = x + y_times_width;
			
			vector<string> tmpPassVal;
			for (vector<vector<pair<unsigned long, unsigned long> > >::iterator pass = passesTemplate.begin(); pass < passesTemplate.end(); pass++)
			{
				tmpEdgeVal = "";
				for (vector<pair<unsigned long, unsigned long> >::iterator p = pass->begin(); p < pass->end(); p++)
				{
					tmpEdgeVal += compareTwo(dataField[tmpIndex + p->first], dataField[tmpIndex + p->second]);
				}
				
				tmpPassVal.push_back(tmpEdgeVal);
			}
			
			vector<string> tmpPassValForUse;
			tmpPassValForUse.push_back(tmpPassVal[0] + tmpPassVal[1] + tmpPassVal[2] + tmpPassVal[3]);
			tmpPassValForUse.push_back(tmpPassVal[1] + tmpPassVal[2] + tmpPassVal[3] + tmpPassVal[0]);
			tmpPassValForUse.push_back(tmpPassVal[2] + tmpPassVal[3] + tmpPassVal[0] + tmpPassVal[1]);
			tmpPassValForUse.push_back(tmpPassVal[3] + tmpPassVal[0] + tmpPassVal[1] + tmpPassVal[2]);
			
			if (0 < winDistance)
			{
				bool notFound = true;
				unsigned long conflictingIndex = 0;
				string conflictingString = "";
				for (map<string, unsigned long>::iterator window = uniqueWindows.begin(); notFound && window != uniqueWindows.end(); window++)
				{
					if (levenshtein_distance(window->first, tmpPassValForUse[0]) <= winDistance)
					{
						conflictingIndex = window->second;
						conflictingString = window->first;
						notFound = false;
					}
				}
				
				if (notFound)
				{
					uniqueWindows[tmpPassValForUse[0]] = tmpIndex;
					for (unsigned c = 1; c < tmpPassValForUse.size(); ++c)
					{
						if (levenshtein_distance(tmpPassValForUse[c], tmpPassValForUse[0]) <= winDistance)
						{
							notFound = false;
						}
						uniqueWindows[tmpPassValForUse[c]] = tmpIndex;
					}
					
					if (!notFound)
					{
						incCostForConflict(tmpIndex, true);
						
						conflicts.push_back(tmpIndex);
					}
				}
				else
				{
					conflicts.push_back(tmpIndex);
					incCostForConflict(tmpIndex);
					
					if (find(conflicts.begin(), conflicts.end(), conflictingIndex) == conflicts.end())
					{
						conflicts.push_back(conflictingIndex);
						incCostForConflict(conflictingIndex);
					}
				}
			}
			else
			{
				for (vector<string>::iterator c = tmpPassValForUse.begin(); c < tmpPassValForUse.end(); c++)
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
	}
	
	conflictCount = conflicts.size();
}

void MFSquare::recalculateCost()
{
	MarkerField::recalculateCost();
}

/***************************************************************
 * IMPLEMENTED PURE VIRTUAL FUNCTIONS FROM MARKERFIELD
 **************************************************************/

vector<long> *MFSquare::getKernelModules(unsigned long)
{
	return &kernelModules;
}

void MFSquare::show()
{
	Mat image = Mat::zeros(height * MARKERFIELD_DEF_SQUARE_PIXELS, 2 * width * MARKERFIELD_DEF_SQUARE_PIXELS + MARKERFIELD_DEF_SQUARE_PIXELS, CV_8UC3);
    
	for (unsigned int y = 0; y < height; y++)
	{
		for (unsigned int x = 0; x < width; x++)
		{
			rectangle(image, Rect(x * MARKERFIELD_DEF_SQUARE_PIXELS, y * MARKERFIELD_DEF_SQUARE_PIXELS, MARKERFIELD_DEF_SQUARE_PIXELS, MARKERFIELD_DEF_SQUARE_PIXELS), getRGBValue(dataField[y*width + x]), CV_FILLED);
		}
	}
	
	for (vector<unsigned long>::iterator it=conflicts.begin(); it < conflicts.end(); it++)
	{
		rectangle(image, Rect(((*it) % width) * MARKERFIELD_DEF_SQUARE_PIXELS, ((*it) / width) * MARKERFIELD_DEF_SQUARE_PIXELS, kernelSize * MARKERFIELD_DEF_SQUARE_PIXELS, kernelSize * MARKERFIELD_DEF_SQUARE_PIXELS), Scalar(0,0,255), 2);
	}
	
	//imshow("Marker field", image);
	
	float maxCost = 0.0;
	for (unsigned long x = 0; x < size; x++)
	{
		if (maxCost < costField[x])
		{
			maxCost = costField[x];
		}
	}
	
	for (unsigned int y = 0; y < height; y++)
	{
		for (unsigned int x = 0; x < width; x++)
		{
			rectangle(image, Rect((x + width + 1) * MARKERFIELD_DEF_SQUARE_PIXELS, y * MARKERFIELD_DEF_SQUARE_PIXELS, MARKERFIELD_DEF_SQUARE_PIXELS, MARKERFIELD_DEF_SQUARE_PIXELS), getRGBCostValue(costField[y * width + x]), CV_FILLED); //Scalar(0, 0, 255*costField[y * width + x]/maxCost), CV_FILLED); //costField[y * width + x] > 255.0 ? 255 : ceil(costField[y * width + x])), CV_FILLED);
		}
	}
	
	imshow("Marker field + cost", image);
			
	
	int ret = waitKey(30);
	if ((ret & 255) == 27)
	{
		ret = waitKey();
	}
}

// @TODO review mutate
/*void MFSquare::mutate()
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
	
	for (unsigned int i = 0; i < mutateHere; i++) {
		dataField[worstPoints[whichOneToChoose[i]]] = newValues[i];
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
