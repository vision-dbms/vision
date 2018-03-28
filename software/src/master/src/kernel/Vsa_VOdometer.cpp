/*****  Vsa_VOdometer Implementation  *****/

/********************
 *****  System  *****
 ********************/

#include "Vk.h"

/******************
 *****  Self  *****
 ******************/

#include "Vsa_VOdometer.h"

/*********************************
 *********************************
 *****                             *****
 *****  Vsa::VOdometer   *****
 *****                             *****
 *********************************
 *********************************/

/**************************
 **************************
 *****  Construction  ***
 **************************
 **************************/ 

template <class T> Vsa::VOdometer<T>::VOdometer(
	T value,
	bool recordChanges,
	float displayMultiplier
) 
: m_value (value)
, m_timerTime (1000000)				//  1 second
, m_compressedBucketSize(1000000 * 5)		//  5 seconds
, m_compressedTotalTimeRange(1000000 * 15)	// 15 seconds
, m_uncompressedTotalTimeRange(1000000 * 10)	// 10 seconds
, m_compressLimitIndex(0)
, m_currentMin(value)
, m_currentMax(value)
, m_recordChanges(recordChanges)
, m_valChangeHappened(false)
, m_displayMultiplier(displayMultiplier)
, m_lastBoxMonsterIndex(0)
, m_currentSum(0)
, m_numAdded(0)
{
    //traceInfo ("Creating Vsa::VPoolBroadcast");
	initValueList();
}

/*************************
 *************************
 *****  Destruction  ****
 *************************
 *************************/ 

template <class T> Vsa::VOdometer<T>::~VOdometer() {
    //traceInfo ("Destroying Vsa::VPoolBroadcast");
}

template <class T> void Vsa::VOdometer<T>::setTimer() {
	VReference<Vca::VTrigger<ThisClass> > pTrigger (
		new Vca::VTrigger<ThisClass> (this, &ThisClass::pushValue)
	);
	
	m_pTimer.setTo(
		new Vca::VTimer("VOdometer push timer", pTrigger, static_cast<Vca::U64>(m_timerTime))			// The last value is given in microseconds apparently....
	);
	m_pTimer->start();
}

template <class T> void Vsa::VOdometer<T>::initValueList() {
	m_valueList.Append(1);
	m_timeList.Append(1);
	m_maxList.Append(1);
	m_minList.Append(1);
	m_sigmaList.Append(1);
	m_valueList[0] = 0;
	m_maxList[0] = m_currentMax;
	m_minList[0] = m_currentMin;
	m_sigmaList[0] = 0;
	V::VTime iTime;
	m_timeList[0] = iTime;
}

template <class T> void Vsa::VOdometer<T>::boxMonsterValues(VkDynamicArrayOf<Vca::U64>& values, VkDynamicArrayOf<V::VTime>& times, VkDynamicArrayOf<Vca::U64>& maxs, VkDynamicArrayOf<Vca::U64>& mins, VkDynamicArrayOf<Vca::F64>& sigmas) {	
	int index = m_lastBoxMonsterIndex < m_compressLimitIndex? m_compressLimitIndex + 1 : m_lastBoxMonsterIndex + 1;
	
	getUncompressedValues(m_valueList, values, index);
	getUncompressedTimes(times, index - 1);
	getUncompressedValues(m_maxList, maxs, index);
	getUncompressedValues(m_minList, mins, index);
	getUncompressedSigmas(sigmas, index);
	
	m_lastBoxMonsterIndex = m_timeList.cardinality();
}

template <class T> void Vsa::VOdometer<T>::getUncompressedValues (VkDynamicArrayOf<T> const &toCopy, VkDynamicArrayOf<Vca::U64> &toCopyTo, int index) {
	for(int i = index; i < toCopy.cardinality(); i++) {
		toCopyTo.Append(1);
		toCopyTo[i - index] = toCopy[i];
	}
}

template <class T> void Vsa::VOdometer<T>::getUncompressedTimes (VkDynamicArrayOf<V::VTime> &toCopyTo, int index) {
	for(int i = index; i < m_timeList.cardinality(); i++) {
		toCopyTo.Append(1);
		toCopyTo[i - index] = m_timeList[i];
	}
}

template <class T> void Vsa::VOdometer<T>::getUncompressedSigmas (VkDynamicArrayOf<Vca::F64> &toCopyTo, int index) {
	for(int i = index; i < m_sigmaList.cardinality(); i++) {
		toCopyTo.Append(1);
		toCopyTo[i - index] = m_sigmaList[i];
	}
}

template <class T> void Vsa::VOdometer<T>::freeUncompressedSpace() {
	int size = m_valueList.cardinality();
	V::VTime uncompressed = m_timeList[size-1] - m_timeList[0];
	V::VTime toFree = uncompressed - m_uncompressedTotalTimeRange;
	V::VTime startRange = m_timeList[1] - m_timeList[0];
	while(startRange < toFree) {
		m_valueList.Delete(0);
		m_minList.Delete(0);
		m_maxList.Delete(0);
		m_timeList.Delete(0);
		m_sigmaList.Delete(0);
		toFree = toFree - startRange;
		startRange = m_timeList[1] - m_timeList[0];
	}
	
	//m_timeList[0] = m_timeList[0] + toFree;
	//m_valueList[1] = (Vca::U64)(m_valueList[1] * (1.0f - (float)(toFree.encodedValue()) / (float)(startRange.encodedValue())));
}

template <class T> void Vsa::VOdometer<T>::compressDataPoint() {
	// if our compressed total time range isn't enough to compress new values we should 
	// use special behavior
	if(m_compressedTotalTimeRange < m_timerTime) {
		freeUncompressedSpace();
		return;
	}
	
	if(m_compressLimitIndex <= 0) {
		// we haven't added a single item to our compression list. We just shift our index over to indicate that the first value in our list is available for compression
		m_compressLimitIndex = 1;
		return;
	} 
	
	
	
	// Delete enough space on the left to add the new time range without going over m_compressedTotalTimeRange
	V::VTime totalCompressed, range, nextRange;
	if(m_compressedTotalTimeRange == 0) {
		totalCompressed = 0;
		range = 0;
	} else {
		totalCompressed = m_timeList[m_compressLimitIndex-1] - m_timeList[0];
		range = m_timeList[m_compressLimitIndex] - m_timeList[m_compressLimitIndex-1];
	}
	nextRange = totalCompressed + range;
	if(nextRange >= V::VTime(m_compressedTotalTimeRange)) {
		V::VTime startRange = m_timeList[1] - m_timeList[0];
		V::VTime toFree = m_timeList[m_compressLimitIndex] - m_timeList[0];
		
		// Delete left-sided values until we've freed enough space
		while(startRange < toFree && m_compressLimitIndex > 1) {
			m_valueList.Delete(0);
			m_minList.Delete(0);
			m_maxList.Delete(0);
			m_timeList.Delete(0);
			m_sigmaList.Delete(0);
			m_compressLimitIndex--;
			toFree = toFree - startRange;
			startRange = m_timeList[1] - m_timeList[0];
		}
		//m_timeList[0] = m_timeList[0] + toFree;
		//m_valueList[1] = (Vca::U64)(m_valueList[1] * (1.0f - (float)(toFree.encodedValue()) / (float)(startRange.encodedValue())));
	}
	
	// Now append the new range on the right side
	if(range < V::VTime(m_compressedBucketSize) ){
		T avg = (m_valueList[m_compressLimitIndex] + m_valueList[m_compressLimitIndex+1]) / 2;
		T max = m_maxList[m_compressLimitIndex] > m_maxList[m_compressLimitIndex+1]? m_maxList[m_compressLimitIndex] : m_maxList[m_compressLimitIndex+1];
		T min = m_minList[m_compressLimitIndex] < m_minList[m_compressLimitIndex+1]? m_minList[m_compressLimitIndex] : m_minList[m_compressLimitIndex+1];
		Vca::F64 sigma = (m_sigmaList[m_compressLimitIndex] + m_sigmaList[m_compressLimitIndex+1]) / 2;
		m_valueList.Delete(m_compressLimitIndex);
		m_timeList.Delete(m_compressLimitIndex);
		m_maxList.Delete(m_compressLimitIndex);
		m_minList.Delete(m_compressLimitIndex);
		m_sigmaList.Delete(m_compressLimitIndex);
		m_valueList[m_compressLimitIndex] = avg;
		m_maxList[m_compressLimitIndex] = max;
		m_minList[m_compressLimitIndex] = min;
		m_sigmaList[m_compressLimitIndex] = sigma;
		
	} else {
		// technically we should compress a portion of what's coming in to the previous one if possible. To make things easy, let's not do that.
		m_compressLimitIndex++;
		return;
	}
}

template <class T> void Vsa::VOdometer<T>::pushValue(Vca::VTrigger<ThisClass> *pTrigger) {
	int size = m_valueList.cardinality();
	int index;
	V::VTime iTime;
	V::VTime uncompressed = m_timeList[size-1] - m_timeList[m_compressLimitIndex];
	V::VTime uncompressedTotal = uncompressed + (iTime - m_timeList[size-1]);
	m_valueList.Append(1);
	m_timeList.Append(1);
	m_maxList.Append(1);
	m_minList.Append(1);
	m_sigmaList.Append(1);
	
	// push index values back one
	m_lastBoxMonsterIndex--;
	
	if(uncompressedTotal < V::VTime(m_uncompressedTotalTimeRange)) {
		index = size;
	} else {
		compressDataPoint();
		size = m_valueList.cardinality();
		index = size - 1;
	}
	
	int mean;
	if(m_numAdded > 0) 
		mean = (float)m_currentSum / (float)m_numAdded;
	else
		mean = m_value;
		
	
	float sum = 0;
	
	for(int i = 0; i < m_tempValueList.cardinality(); i++) {
		int factor = m_tempValueList[i] - mean;
		sum += factor * factor;
	}
	
	float sigma;
	if(m_numAdded <= 1)
		sigma = 0;
	else
		sigma = sum / (float)(m_numAdded - 1);	
		
	m_valueList[index] = mean;
	m_maxList[index] = m_currentMax;
	m_minList[index] = m_currentMin;
	m_timeList[index] = iTime;
	m_sigmaList[index] = sigma;

	if(m_recordChanges) {
		m_currentMax = 0;
		m_currentMin = 0;
	} else {
		m_currentMax = m_value;
		m_currentMin = m_value;
	}
	m_valChangeHappened = false;
	m_currentSum = 0;
	m_numAdded = 0;
	int numTemp = m_tempValueList.cardinality();
	for(int i = 0; i < numTemp; i++)
		m_tempValueList.Delete(0);
	setTimer();
}

template <class T> T Vsa::VOdometer<T>::getValue()  const {
	return m_value; 
}

template <class T> V::VString Vsa::VOdometer<T>::print() const { 
	VString s;
	int i = 0;
	int num = m_valueList.cardinality();
	while(i < num) {
		VString timeString;
		m_timeList[i].asString(timeString);
		s << "Time: " << timeString << ", Value: " << m_valueList[i] << ", Min: " << m_minList[i] << ", Max: " << m_maxList[i] << "\n";
		if(i == m_compressLimitIndex) {
			s << "-------Compress limit--------------\n";
		}
		i++;
	}
	return s;
}

template <class T> void Vsa::VOdometer<T>::getTimeAttributes(Vca::U64 const &timeBeforeNow, Vca::U64 const &timeRange, V::VTime &startTime, V::VTime &endTime, int &startI, int &endI)  const{
	startTime = startTime - timeBeforeNow;
	endTime = startTime + timeRange;
	int numElements = m_valueList.cardinality();
	startI = 0;

	while(startI < numElements && m_timeList[startI] < startTime) {
		startI++;
	}
	
	endI = startI;
	while(endI < numElements && m_timeList[endI] < endTime) {
		endI++;
	}
}

template <class T> Vca::S64 Vsa::VOdometer<T>::statSum(Vca::U64 timeBeforeNow, Vca::U64 timeRange) const {	// the inputs are in microseconds
	V::VTime nowTime, startTime, endTime;
	int startI, endI;
	getTimeAttributes(timeBeforeNow, timeRange, startTime, endTime, startI, endI);
	int numElements = m_valueList.cardinality();
	if(startI == 0) {
		throw OdometerImproperInputException("Given timeBefore too large, goes beyond this odometer's memory");
	}
	// Not sure what any of these three values is for.
        //	V::VTime startSamplePost, startSamplePre;
        //	Vca::S64 startVal;
	
	if(startTime > nowTime) {
		throw OdometerImproperInputException("Given timeBefore is somehow later than the present moment");
	}
	
	if(startI == endI) {
		if(startI == numElements)
			throw OdometerImproperInputException("Given timeBefore is too small, please input larger value");
		V::VTime timeRange = endTime - startTime;
		float totalTime = m_timeList[startI] - m_timeList[startI-1];
		return (float)(timeRange.encodedValue()) / totalTime;// * startVal;		
	}

	if(endTime > nowTime) {
		throw OdometerImproperInputException("Given timeRange goes beyond the present time");
	}
	
	float sum = 0; 
	
	startI++;
	while(startI < endI && startI < numElements) {
		sum += (Vca::S64)m_valueList[startI] - (Vca::S64)m_valueList[startI-1];
		startI++;
	}
	if(sum - (Vca::S64)sum > 0.5f) 
		return (Vca::S64)sum + 1;
	else 
		return (Vca::S64)sum;
}


template <class T> T Vsa::VOdometer<T>::statMin(Vca::U64 timeBeforeNow, Vca::U64 timeRange, V::VTime &time) const {
	V::VTime nowTime, startTime, endTime;
	int startI, endI;
	getTimeAttributes(timeBeforeNow, timeRange, startTime, endTime, startI, endI);
	int numElements = m_valueList.cardinality();
	if(startI == endI) {
		if(startI < numElements) {
			time = m_timeList[startI];
			return m_minList[startI];
		} else {
			if(startTime > nowTime || endTime > nowTime) {
				throw OdometerImproperInputException("Given time range goes beyond the present time");
			}
			time = nowTime;
			return m_currentMin;
		}
	} 
	
	
	
	int i = endI - 1;
	int minVal;
	V::VTime minTime;
	if (endI == numElements) {
		minVal = m_currentMin;
	} else {
		minVal = m_minList[endI];
		minTime = m_timeList[endI];
	}
	while(i >= startI) {
		if(minVal > m_minList[i]) {
			minVal = m_minList[i];
			minTime = m_timeList[i];
		}
		i--;
	}
	time = minTime;
	return minVal;
}

template <class T> T Vsa::VOdometer<T>::statMax(Vca::U64 timeBeforeNow, Vca::U64 timeRange, V::VTime &time) const {
	V::VTime nowTime, startTime, endTime;
	int startI, endI;
	getTimeAttributes(timeBeforeNow, timeRange, startTime, endTime, startI, endI);
	int numElements = m_valueList.cardinality();
	if(startI == endI) {
		if(startI < numElements) {
			time = m_timeList[startI];
			return m_maxList[startI];
		} else {
			if(startTime > nowTime || endTime > nowTime) {
				throw OdometerImproperInputException("Given time range goes beyond the present time");
			}
			time = nowTime;
			return m_currentMax;
		}
	} 
	
	int i = endI - 1;
	int maxVal;
	V::VTime maxTime;
	if (endI == numElements) {
		maxVal = m_currentMax;
	} else {
		maxVal = m_maxList[endI];
		maxTime = m_timeList[endI];
	}
	while(i >= startI) {
		if(maxVal < m_maxList[i]) {
			maxVal = m_maxList[i];
			maxTime = m_timeList[i];
		}
		i--;
	}
	time = maxTime;
	return maxVal;
}

// This is where we explicitly instantiate the class templates we intend to use
// Huzzah!
template class Vsa_API Vsa::VOdometer<Vca::U32>;
template class Vsa_API Vsa::VOdometer<Vca::U64>;
