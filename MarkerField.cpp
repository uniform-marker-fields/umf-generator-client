#include "MarkerField.h"

/***************************************************************
 * (DE)CONSTRUCTOR(S)
 **************************************************************/
 
//MarkerField::MarkerField(unsigned int w, unsigned int h, unsigned int k, unsigned int kt, unsigned int t, const char* c, const char* ia, unsigned int kmc, unsigned char sE, unsigned char sN, float cE, float cD):
MarkerField::MarkerField(unsigned int w, unsigned int h, unsigned int k, unsigned int kt, unsigned int t, unsigned int nON, const char* c, const char* ia, unsigned int kmc, unsigned char sE, const char* cn, float cs):
	width(w), height(h), size(w*h), kernelSize(k), kernelType(kt), colorsText(c), imgAlgText(ia), range(0), typeTorus(0), typeColor(0), dataField(NULL), costField(NULL), setEqual(sE), costNeighborsText(cn), costSimilarity(cs), // setNoCost(128), costEqual(0.0), costDiff(0.0), // setNoCost(sN), costEqual(cE), costDiff(cD),
	conflictCount(0), cost(0.0f), attemptedMutations(0), successfulMutations(0), noMutations(0), noConflictChanges(0), kernelModuleCount(kmc), mutateMax(sqrtl(w*h) > MARKERFIELD_MUTATE_MAX ? MARKERFIELD_MUTATE_MAX : (sqrtl(w*h) )),
	maxCostForNeighbors(0.0f), numberOfNeighbors(nON)
{
	typeTorus = getTypeTorusFromType(t);
	typeColor = getTypeColorFromType(t);
	range = getRangeFromType(t);
	initCostNeighbors();
}

MarkerField::~MarkerField()
{
	if (dataField == NULL)
	{
		free(dataField);
	}
	if (costField == NULL)
	{
		free(costField);
	}
}

/***************************************************************
 * INIT FUNCTIONS
 **************************************************************/
 
void MarkerField::initCostNeighbors()
{
	vector<string> items;
	boost::split(items,costNeighborsText,boost::is_any_of(";"));
	unsigned char leftBound = 0;
	
	for (vector<string>::iterator it = items.begin(); it != items.end(); ++it)
	{
		vector<string> values;
		boost::split(values,*it,boost::is_any_of(",:"));
		
		tCostNeighbor t;
		t.leftBound = leftBound;
		t.rightBound = atoi(values[2].c_str());
		t.leftCost = atof(values[0].c_str());
		t.rightCost = atof(values[1].c_str());
		
		if (t.leftCost > maxCostForNeighbors)
		{
			maxCostForNeighbors = t.leftCost;
		}
		if (t.rightCost > maxCostForNeighbors)
		{
			maxCostForNeighbors = t.rightCost;
		}
		
		t.diffCost = t.rightCost - t.leftCost;
		t.diffBound = t.rightBound - leftBound;
		leftBound = t.rightBound;
		
			//cout << (int) t.leftBound << " " << (int) t.rightBound << " " << t.leftCost << " " << t.rightCost << " " << endl;
			//cout << t.diffCost << " " << (int) t.diffBound << endl;
		
		costNeighbors.push_back(t);
	}
}

void MarkerField::initEnd()
{
	recalculateCost();
	
	initRGBCostThresholds();
}

void MarkerField::initRGBCostThresholds()
{
	rgbCostBadThreshold = maxCostForNeighbors * numberOfNeighbors + costSimilarity * 255;
	rgbCostGoodThreshold = maxCostForNeighbors * numberOfNeighbors / 2 + costSimilarity * 64;
	
	/*cout << maxCostForNeighbors << endl;
	cout << costSimilarity << endl;
	cout << rgbCostBadThreshold << endl;
	cout << rgbCostGoodThreshold << endl;*/
}

void MarkerField::initStart(string d)
{
	checkAttributes();
	
	initKernelModules();
	
	initPassesTemplate();
	
	readData(d);
}

void MarkerField::readData(string d)
{
	dataField = (unsigned int *) realloc(NULL, size * sizeof(unsigned int));
	costField = (float *) realloc(NULL, size * sizeof(unsigned int));

	for (unsigned long i = 0; i < size; i++) {
		dataField[i] = (d.length() > i ? getNumericValue(d[i],range) : 0);
		costField[i] = 0.0f;
	}
}


/***************************************************************
 * CHECK ATTRIBUTES
 **************************************************************/

void MarkerField::checkAttributes()
{
	checkKernelSize();
	checkWidthHeight();
}

void MarkerField::checkKernelSize()
{
	if (kernelSize == 0)
	{
		throw MarkerFieldException("The size of the kernel must be greater than zero.");
	}
}

void MarkerField::checkKernelType()
{
	if (kernelType == 0)
	{
		cerr << "Warning: kernel type is set to 0, the proper default value will be used!" << endl;
	}
}

void MarkerField::checkRange()
{
	if (range == 0)
	{
		throw MarkerFieldException("Range must be greater than zero.");
	}
}

void MarkerField::checkWidthHeight()
{
	if (width == 0 || height == 0)
	{
		throw MarkerFieldException("Width and height must be greater than zero.");
	}
}

/***************************************************************
 * GETTER METHODS FOR STANDARD ATTRIBUTES
 **************************************************************/
 
string MarkerField::getColorsText()
{
	return colorsText;
}

unsigned int MarkerField::getConflictCount()
{
	return conflictCount;
}

float MarkerField::getCost()
{
	return cost / size;
}

unsigned int MarkerField::getHeight()
{
	return height;
}

string MarkerField::getImgAlgText()
{
	return imgAlgText;
}

unsigned int MarkerField::getKernelSize()
{
	return kernelSize;
}

unsigned long MarkerField::getKernelType()
{
	return kernelType;
}

unsigned int MarkerField::getType()
{
	return MarkerField::getSynthesisedType(typeTorus, typeColor, range);
}

unsigned int MarkerField::getWidth()
{
	return width;
}

/***************************************************************
 * SOME STATIC METHODS DEALING WITH COMPLEX TYPE FORM
 **************************************************************/
 
unsigned int MarkerField::getRangeFromType(unsigned int t)
{
	return TYPE_RANGE_START+((t & TYPE_RANGE_BITS) >> TYPE_RANGE_BIT_START);
}

unsigned int MarkerField::getSynthesisedType(unsigned int t, unsigned int c, unsigned int r)
{
	return t + (c << 1) + ((r-TYPE_RANGE_START) << TYPE_RANGE_BIT_START);
}

unsigned int MarkerField::getTypeColorFromType(unsigned int t)
{
	return ((TYPE_COLOR_BIT & t) >> 1);
}

unsigned int MarkerField::getTypeTorusFromType(unsigned int t)
{
	return (TYPE_TORUS_BIT & t);
}

int MarkerField::fillWithRandom(unsigned int **data, unsigned long size, unsigned int range) {
	if (size == 0 || range == 0) {
		return -1;
	}
	
	srand ( time(NULL) );
	
	(*data) = (unsigned int *) realloc(NULL, size * sizeof(unsigned int));

	for (unsigned long i = 0; i < size; i++) {
		(*data)[i] = rand() % range;
	}
	
	return 0;
}

/***************************************************************
 * COST AND CONFLICT CALCULATIONS
 **************************************************************/
		
Scalar MarkerField::getRGBCostValue(float c)
{
	if (c < rgbCostGoodThreshold)
	{
		return Scalar(0,255,255 * c / rgbCostGoodThreshold);
	}
	else if (c < rgbCostBadThreshold)
	{
		return Scalar(0, 64 + 191 * (rgbCostBadThreshold - c) / rgbCostGoodThreshold, 255);
	}
	else
	{
		return Scalar(0, (c > 356.0 ? 0 : 64.0 - (c - 100.0) / 4.0), 255);
	}
}

void MarkerField::incCostForConflict(unsigned long index, bool selfConflict)
{
	float penaltyConflictsHere = (selfConflict ? MARKERFIELD_PEN_SELF_CONFLICTS * MARKERFIELD_PEN_CONFLICTS : MARKERFIELD_PEN_CONFLICTS);
	
	cost += penaltyConflictsHere * kernelModuleCount;
	
	vector<long> *km = getKernelModules(index);
	for (vector<long>::iterator it = km->begin(); it != km->end(); ++it)
	{
		costField[index + (*it)] += penaltyConflictsHere;
	}
}

void MarkerField::incCostNeighbor(unsigned long i1, unsigned char v1, unsigned long i2, unsigned char v2)
{
	bool c_tmp = true;
	incCostNeighbor(i1, v1, i2, v2, &c_tmp);
}

void MarkerField::incCostNeighbor(unsigned long i1, unsigned char v1, unsigned long i2, unsigned char v2, bool *conflictFlag)
{
	int absolute = abs(v1 - v2);
	(*conflictFlag) = true;
	float costHere = 0.0;
	
	for (vector<tCostNeighbor>::iterator it = costNeighbors.begin(); it != costNeighbors.end(); ++it)
	{
		if (absolute <= it->rightBound)
		{
			costHere = (it->diffBound == 0 ? it->leftCost : (it->diffCost) * (absolute - it->leftBound)/ (it->diffBound) + it->leftCost);
			costField[i1] += costHere;
			costField[i2] += costHere;
			cost += costHere * 2;
			break;
		}
	}
	
	if (absolute > setEqual)
	{
		(*conflictFlag) = false;
	}
}

void MarkerField::recalculateCost()
{
	cost = 0;
	for (unsigned long i = 0; i < width*height; i++)
	{
		costField[i] = 0.0f;
	}
	
	checkForConflicts();
}

/***************************************************************
 * SELECTING POINTS FOR CHANGE
 **************************************************************/

void MarkerField::setWorstPoints(unsigned int max)
{
	worstPoints.clear();
	vector<float> worstValues;
	for (unsigned long i = 0; i < width*height; i++)
	{
		if (worstPoints.size() < max)
		{
			int counter = 0;
			vector<float>::reverse_iterator itV = worstValues.rbegin();
			while (itV < worstValues.rend())
			{
				if (*itV > costField[i])
				{
					break;
				}
				counter++;
				itV++;
			}
			
			worstValues.insert(itV.base(), costField[i]);
			vector<unsigned long>::reverse_iterator itP = worstPoints.rbegin()+counter;
			worstPoints.insert(itP.base(), i);
		}
		else if (worstValues.back() < costField[i])
		{
			int counter = 0;
			vector<float>::reverse_iterator itV = worstValues.rbegin();
			while (itV < worstValues.rend())
			{
				if (*itV > costField[i])
				{
					break;
				}
				counter++;
				itV++;
			}
			
			worstValues.insert(itV.base(), costField[i]);
			vector<unsigned long>::reverse_iterator itP = worstPoints.rbegin()+counter;
			worstPoints.insert(itP.base(), i);
			
			worstPoints.pop_back();
			worstValues.pop_back();
		}
	}
}

void MarkerField::setConflictPoints(unsigned int max)
{
	worstPoints.clear();
	
	if (conflicts.size() > 0)
	{
		unsigned int i = 0;
		while (i < max)
		{
			unsigned int n = rand() % conflicts.size();
			
			vector<long> *km = getKernelModules(conflicts[n]);
			
			for (vector<long>::iterator it = km->begin(); it != km->end() && i < max; ++it)
			{
				worstPoints.push_back((*it) + conflicts[n]);
				i++;
			}
		}
	}
	else
	{
		setWorstPoints(max);
	}
}

/***************************************************************
 * Making the changes
 **************************************************************/
 
void MarkerField::mutate()
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
}
 
/*
void MarkerField::mutate()
{
	attemptedMutations++;
	unsigned int *whichOneToChoose;
	unsigned int *newValues;
	
	static unsigned int mutateHere = 1;
	static unsigned int mutateMaxHere = 4;
	static unsigned int attemptsWithSettings = 0;
		// calculate max attempt count for these settings
	static unsigned int maxAttemptsWithSettings = MARKERFIELD_MUTATE_MAX_ATTEMPTS;
	
	static bool tryWorstPoints = true;
	static bool allowWorseCost = false;
	
	// TWO OPTIONS:
	// 1.) try to change modules with highest cost
	// 2.) try to change modules in conflicts
	if (tryWorstPoints)
	{
		// if out of attempts then change settings
		if (attemptsWithSettings > MARKERFIELD_MUTATE_MAX_ATTEMPTS)
		{
			// trying to increase the number of points to change
			mutateHere++;
			attemptsWithSettings = 0;
			
			// if it is more then allowed
			if (mutateHere > mutateMax)
			{
				if (conflictCount > 0)
				{
					tryWorstPoints = false;
					mutateHere = kernelModuleCount / 2;
					mutateMaxHere = kernelModuleCount;
					//maxAttemptsWithSettings = (unsigned int) pow((range > 15 ? 15 : range), 4);
				}
				else
				{
					mutateHere = 1;
					mutateMaxHere = 4;
					attemptsWithSettings = 0;
					//maxAttemptsWithSettings = range * mutateMaxHere * (mutateMaxHere - mutateHere) / mutateHere;
				}
			}
			else if (mutateHere > mutateMaxHere)
			{
				mutateHere = 1;
				mutateMaxHere = mutateMaxHere * 3 / 2;
				//maxAttemptsWithSettings = range * mutateMaxHere * (mutateMaxHere - mutateHere) / mutateHere;
			}
			else
			{
				//maxAttemptsWithSettings = range * mutateMaxHere * (mutateMaxHere - mutateHere) / mutateHere;
			}
		}
	}
	else
	{
		// if out of attempts then change settings
		if (attemptsWithSettings > MARKERFIELD_MUTATE_MAX_ATTEMPTS)
		{
			// trying to increase the number of points to change
			mutateHere++;
			attemptsWithSettings = 0;
			
			// if it is more then allowed
			if (mutateHere > mutateMax)
			{
				tryWorstPoints = true;
				mutateHere = 1;
				mutateMaxHere = 4;
				attemptsWithSettings = 0;
				//maxAttemptsWithSettings = range * mutateMaxHere * (mutateMaxHere - mutateHere) / mutateHere;
			}
			else
			{				
				if (mutateHere > mutateMaxHere)
				{
					mutateMaxHere += kernelModuleCount;
				}
				
				if (mutateMaxHere > conflictCount * kernelModuleCount)
				{
					if (conflictCount < 4)
					{
						mutateMaxHere -= kernelModuleCount;
						mutateHere = mutateMaxHere;
						allowWorseCost = true;
					}
					else
					{
						
					}
					
					
					
					if (conflictCount > 8)
					{
						mutateHere = kernelModuleCount / 2;
						mutateMaxHere = kernelModuleCount;
						//maxAttemptsWithSettings = (unsigned int) pow((range > 15 ? 15 : range), 4);
					}
					else
					{
						mutateMaxHere -= kernelModuleCount;
					}
				}
			}
		}
	}
	
	attemptsWithSettings++;
	
	cout << "++++++++++" << endl;
	cout << "ATTEMPTS: " << attemptsWithSettings << endl;
	cout << "mutateHere: " << mutateHere << endl;
	cout << "mutateMaxHere: " << mutateMaxHere << endl;
	//cout << "maxAttemptsWithSettings:" << maxAttemptsWithSettings << endl;
	
	// get randoms
	fillWithRandom(&whichOneToChoose, mutateHere, mutateMaxHere);
	fillWithRandom(&newValues, mutateHere, range);
	
	if (tryWorstPoints)
	{
		setWorstPoints(mutateMaxHere);
	}
	else
	{
		setConflictPoints(mutateMaxHere);
	}
	
	// save a copy...
	vector<unsigned long> oldConflicts = conflicts;
	unsigned int *oldField = (unsigned int *) malloc(size * sizeof(unsigned int));
	memcpy(oldField, dataField, size * sizeof(unsigned int));
	float* oldcostField = (float *) malloc(size * sizeof(float));
	memcpy(oldcostField, costField, size * sizeof(float));
	float oldActualPenalty = cost;
	
	for (unsigned int i = 0; i < mutateHere; i++)
	{
		dataField[worstPoints[whichOneToChoose[i]]] = newValues[i];
	}
	
	recalculateCost();
	
	if (oldActualPenalty < cost && !allowWorseCost)
	{
		conflicts = oldConflicts;
		memcpy(dataField, oldField, size * sizeof(unsigned int));
		memcpy(costField, oldcostField, size * sizeof(unsigned int));
		cost = oldActualPenalty;
		conflictCount = conflicts.size();
		noMutations++;
	}
	else
	{
		successfulMutations++;
		noMutations = 0;
	}
	
	if (allowWorseCost)
	{
		allowWorseCost = false;
		mutateHere = 1;
		mutateMaxHere = 4;
		attemptsWithSettings = 0;
		maxAttemptsWithSettings = MARKERFIELD_MUTATE_MAX_ATTEMPTS;
	}
	
	if (conflicts.size() == oldConflicts.size() && conflicts.size() != 0)
	{
		noConflictChanges++;
	}
	else
	{
		noConflictChanges = 0;
	}
	
	free(whichOneToChoose);
	free(newValues);
}*/

/***************************************************************
 * INFORMATIONS
 **************************************************************/

string MarkerField::getReadableData()
{
	string ret = "";
	
	for (unsigned long i = 0; i < width*height; i++)
	{
		ret += (unsigned char) getTextValue(dataField[i]);
	}
	
	return ret;
}

void MarkerField::printStatistics(bool brief)
{
	if (brief)
	{
		cout << conflictCount << ";";
		cout << cost << ";";
		cout << attemptedMutations << ";";
		cout << successfulMutations << ";";
		cout << noMutations << ";";
		cout << noConflictChanges << endl;
	}
	else
	{
		cout << "Number of conflicts: " << conflictCount << endl;
		cout << "Cost: " << cost / size << endl;
		cout << "Number of attempted mutations: " << attemptedMutations << endl;
		cout << "Number of successful mutations: " << successfulMutations << endl;
		cout << "No mutations since: " << noMutations << endl;
		if (conflictCount != 0)
		{
			cout << "No changes in conflict count since: " << noConflictChanges << endl;
		}
	}
}
