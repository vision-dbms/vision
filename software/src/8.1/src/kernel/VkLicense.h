#ifndef VkLicense_Include
#define VkLicense_Include

/***********************
 ****  Components  *****
 ***********************/

/**************************
 *****  Declarations  *****
 **************************/

/*************************
 *****  Definitions  *****
 *************************/

class VkLicense {
//  Construction
public:
    void construct (VkLicense const& rOther);
    void construct (char const* pLicenseFile);
    void construct ();

//  Properties
protected:
    unsigned int* pp0 () { return m_iValue +  0; }
    unsigned int* pp1 () { return m_iValue +  4; }
    unsigned int* pp2 () { return m_iValue +  8; }
    unsigned int* pp3 () { return m_iValue + 12; }

    unsigned int const* pp0 () const { return m_iValue +  0; }
    unsigned int const* pp1 () const { return m_iValue +  4; }
    unsigned int const* pp2 () const { return m_iValue +  8; }
    unsigned int const* pp3 () const { return m_iValue + 12; }

    unsigned int size () const {
	return sizeof (m_iValue) / sizeof (m_iValue[0]);
    }
    unsigned int type	    () const { return pp0()[0] % 33; }
    unsigned int expiration () const { return pp0()[0] / 33; }

//  Initialization
public:
    void setValue (unsigned int xValue, unsigned int iValue) {
	if (xValue < size ())
	    m_iValue[xValue] = iValue;
    }

//  Comparison
public:
    bool operator == (VkLicense const& rOther) const;

//  Validation
public:
    bool isValid () const;

//  Display
public:
    void print () const;

//  State
protected:
    unsigned int m_iValue[16];
};

#endif
