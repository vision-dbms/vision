#ifndef Vsa_VOdometer
#define Vsa_VOdometer

/*********************
 *****  Library  *****
 *********************/
#include "Vsa.h"

#include "Vca_VTimer.h"
#include "Vca_VTrigger.h"

/*************************
 *****  Definitions  *****
 *************************/

namespace Vsa {
    /****************************
     *----  VOdometer----*
     ****************************/
    template <class T> class Vsa_API VOdometer : public VReferenceable{
	//  Run Time Type
	DECLARE_CONCRETE_RTTLITE (VOdometer, VReferenceable);
	public:
	class OdometerImproperInputException {
	public:
	    OdometerImproperInputException(char const *message) {
		m_message = message;
	    }
	    VString getMessage() { return m_message; }
			
	private:
	    VString m_message;
	};


    //  Construction
    public:
		
	VOdometer (	
	   T value,
	   bool recordChanges = false,
	   float displayMultiplier = 1
	);
	VOdometer() {}
	
    //  Destruction
    public:
	~VOdometer ();
//
	
    public:
	// For the following, inputs are in microseconds
	Vca::S64 statSum(Vca::U64 timeBeforeNow, Vca::U64 timeRange) const;
	T statMin(Vca::U64 timeBeforeNow, Vca::U64 timeRange, V::VTime &time) const;
	T statMax(Vca::U64 timeBeforeNow, Vca::U64 timeRange, V::VTime &time) const;

	T getValue() const;

	float displayMultiplier() const { return m_displayMultiplier; }

	VString print() const;

	void valChange(T oldValue) {
	    T candidate;
	    if(m_recordChanges) {
		candidate = m_value - oldValue;
	    } else
		candidate = m_value;

	    if(candidate < m_currentMin || (!m_valChangeHappened && m_recordChanges))
		m_currentMin = candidate;
	    if(candidate> m_currentMax)
		m_currentMax = candidate;
	    m_valChangeHappened = true;
	    m_currentSum += m_value;
	    m_tempValueList.Append(1);
	    m_tempValueList[m_tempValueList.cardinality() - 1] = m_value;
	    m_numAdded++;
	}
		
	operator T() const { 
	    return m_value; 
	}

	bool operator>= (const T &other) const {
	    return m_value >= other;
	}

	bool operator>= (const int &other) const {
	    return m_value >= other;
	}

	bool operator> (const T &other) const {
	    return m_value > other;
	}

	bool operator> (const int &other) const {
	    return m_value > other;
	}

	bool operator< (const T &other) const {
	    return m_value < other;
	}

	bool operator< (const int &other) const {
	    return m_value < other;
	}

	bool operator<= (const T &other) const {
	    return m_value <= other;
	}

	bool operator<= (const int &other) const {
	    return m_value <= other;
	}

	bool operator== (const T &other) const {
	    return m_value == other;
	}

	bool operator== (const int &other) const {
	    return m_value == other;
	}

	T operator++ () {
	    m_value++;
	    valChange(m_value-1);
	    return m_value;
	}

	T operator++ (int x) {
	    m_value++;
	    valChange(m_value-1);
	    return m_value - 1;
	}

	T operator-- () {
	    m_value--;
	    valChange(m_value+1);
	    return m_value;
	}

	T operator-- (int x) {
	    m_value--;
	    valChange(m_value+1);
	    return m_value + 1;
	}

	T operator+= (const Vca::U64 other) {
	    m_value += other;
	    valChange(m_value-other);
	    return m_value;
	}		

	T operator-= (const Vca::U64 other) {
	    m_value -= other;
	    valChange(m_value+other);
	    return m_value;
	}

	void setTimer();

	void boxMonsterValues(VkDynamicArrayOf<Vca::U64>&, VkDynamicArrayOf<V::VTime>&, VkDynamicArrayOf<Vca::U64>&, VkDynamicArrayOf<Vca::U64>&, VkDynamicArrayOf<Vca::F64>&);
	
    private:
	void getUncompressedValues (VkDynamicArrayOf<T> const &toCopy, VkDynamicArrayOf<Vca::U64> &toCopyTo, int index);
	void getUncompressedTimes (VkDynamicArrayOf<V::VTime> &toCopyTo, int index);
	void getUncompressedSigmas (VkDynamicArrayOf<Vca::F64> &toCopyTo, int index);
	static Vca::U64 const MULT = 1000 * 1000;
	void pushValue(Vca::VTrigger<ThisClass> *pTrigger);
	void initValueList();
	void compressDataPoint();
	void freeUncompressedSpace();
	void getTimeAttributes(Vca::U64 const &timeBeforeNow, Vca::U64 const &timeRange, V::VTime &startTime, V::VTime &endTime, int &startI, int &endI) const;
		
	//State
    protected:
	T				m_value;
	Vca::U64			m_currentSum;
	Vca::U64			m_numAdded;
	Vca::U64			m_timerTime;
	Vca::U64			m_compressedBucketSize;
	Vca::U64			m_compressedTotalTimeRange;
	Vca::U64			m_uncompressedTotalTimeRange;

	int				m_lastBoxMonsterIndex;

	T				m_currentMin;
	T				m_currentMax;

	int 				m_compressLimitIndex;

	Vca::VTimer::Reference 		m_pTimer;

	VkDynamicArrayOf<T>		m_valueList;
	VkDynamicArrayOf<V::VTime> 	m_timeList;
	VkDynamicArrayOf<T>		m_minList;
	VkDynamicArrayOf<T>		m_maxList;
	VkDynamicArrayOf<Vca::F64>	m_sigmaList;

	// a list of every value change that we reset each time values are pushed into m_valueList
	// we use this to calculate sigma properly
	VkDynamicArrayOf<T>		m_tempValueList;

	// If true, we record min and max changes in m_value rather than min and max values of m_value directly
	bool				m_recordChanges;
	bool				m_valChangeHappened;

	// Multiply by this amount when printing the value as a string
	// Used to change the way a value is displayed, for example, 
	// displaying a millisecond value in seconds
	float				m_displayMultiplier;

    public:
	Vca::U64 getTimerTime() const {
	    return m_timerTime; 
	}

	Vca::U64 getCompressedBucketSize() const {
	    return m_compressedBucketSize;
	}

	Vca::U64 getCompressedTotalTimeRange () const {
	    return m_compressedTotalTimeRange;
	}

	Vca::U64 getUncompressedTotalTimeRange () const {
	    return m_uncompressedTotalTimeRange;
	}

	void setTimerTime(Vca::U64 value) {
	    if(value > m_uncompressedTotalTimeRange)
		m_timerTime = m_uncompressedTotalTimeRange;
	    else
		m_timerTime = value * MULT;
	}

	void setCompressedBucketSize(Vca::U64 value) {
	    if(value * MULT > m_compressedTotalTimeRange)
		m_compressedBucketSize = m_compressedTotalTimeRange;
	    else
		m_compressedBucketSize = value * MULT;
	}

	void setTotalTimeRange(Vca::U64 value) {
	    if(value * MULT < m_uncompressedTotalTimeRange) {
		m_uncompressedTotalTimeRange = value * MULT;
		m_compressedTotalTimeRange = 0;
	    } else {
		m_compressedTotalTimeRange = (value * MULT - m_uncompressedTotalTimeRange);
	    }
	}

	void setUncompressedTotalTimeRange(Vca::U64 value) {
	    m_uncompressedTotalTimeRange = MULT * value;
	}	
    };
}

#endif
