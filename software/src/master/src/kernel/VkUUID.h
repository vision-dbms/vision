#ifndef VkUUID_Interface
#define VkUUID_Interface

/*********************
 *****  Library  *****
 *********************/

#include "V.h"

/************************
 *****  Components  *****
 ************************/

/**************************
 *****  Declarations  *****
 **************************/


/*************************
 *****  Definitions  *****
 *************************/

/*---------------------*
 *----  V::uuid_t  ----*
 *---------------------*/

namespace V {
  struct uuid_t {
    typedef unsigned int	u32;
    typedef unsigned short	u16;
    typedef unsigned char	u8;

    u32	time_low;
    u16	time_mid;
    u16	time_hi_and_version;
    u8	clock_seq_hi_and_reserved;
    u8	clock_seq_low;
    u8	node[6];
  };
}


/*------------------*
 *----  VkUUID  ----*
 *------------------*/

class V_API VkUUID {
//  Aliases
public:
    typedef V::VString VString;

//  uuid_t
public:
    typedef V::uuid_t uuid_t;

//  Construction
public:
    static bool GetUUID (uuid_t &rUUID, char const *pString);
    static bool GetUUID (uuid_t &rUUID);

    static uuid_t ReturnUUID (
	unsigned int  iD1, unsigned short  iD2, unsigned short  iD3,
	unsigned char iD4_0, unsigned char iD4_1, unsigned char iD4_2, unsigned char iD4_3,
	unsigned char iD4_4, unsigned char iD4_5, unsigned char iD4_6, unsigned char iD4_7
    );
    static uuid_t ReturnUUID (char const *pString);
    static uuid_t ReturnUUID ();

protected:
    VkUUID (uuid_t const &rUUID);
    VkUUID (
	unsigned int iD1,
	unsigned short iD2, unsigned short iD3,
	unsigned char iD4_0, unsigned char iD4_1, unsigned char iD4_2, unsigned char iD4_3,
	unsigned char iD4_4, unsigned char iD4_5, unsigned char iD4_6, unsigned char iD4_7
    );
    VkUUID (char const *pString);
    VkUUID ();

//  Access
public:
    operator uuid_t const& () const {
	return m_iUUID;
    }

    bool GetString (VString &rString) const;
    static bool GetString (VString &rString, const uuid_t &rUUID);

//  Query
public:
    bool isNil () const;
    bool isntNil () const {
	return !isNil ();
    }

    static bool lt (uuid_t const &rUUID1, uuid_t const &rUUID2);
    static bool le (uuid_t const &rUUID1, uuid_t const &rUUID2);
    static bool eq (uuid_t const &rUUID1, uuid_t const &rUUID2);
    static bool ne (uuid_t const &rUUID1, uuid_t const &rUUID2);
    static bool ge (uuid_t const &rUUID1, uuid_t const &rUUID2);
    static bool gt (uuid_t const &rUUID1, uuid_t const &rUUID2);

    bool operator< (uuid_t const &rOther) const {
	return lt (m_iUUID, rOther);
    }
    bool operator<= (uuid_t const &rOther) const {
	return le (m_iUUID, rOther);
    }
    bool operator== (uuid_t const &rOther) const {
	return eq (m_iUUID, rOther);
    }
    bool operator!= (uuid_t const &rOther) const {
	return ne (m_iUUID, rOther);
    }
    bool operator>= (uuid_t const &rOther) const {
	return ge (m_iUUID, rOther);
    }
    bool operator> (uuid_t const &rOther) const {
	return gt (m_iUUID, rOther);
    }

//  Byte Order Reversal
public:
    static void reverseByteOrder (uuid_t &rUUID);

//  Display
public:
    static void displayInfo (char const *pWhat, uuid_t const &rUUID);
    void displayInfo (char const *pWhat);

//  State
protected:
    uuid_t m_iUUID;
};


/*------------------------*
 *----  VkUUIDHolder  ----*
 *------------------------*/

/******************************************************************
 *****  Use this class if the default constructor should NOT  *****
 *****  generate a new UUID and/or the UUID value is mutable. *****
 ******************************************************************/

class V_API VkUUIDHolder : public VkUUID {
//  Family Values
public:
    typedef VkUUID		BaseClass;
    typedef VkUUIDHolder	ThisClass;

//  Construction
public:
    VkUUIDHolder (uuid_t const &rUUID) : BaseClass (rUUID) {
    }
    VkUUIDHolder (
	unsigned int iD1,
	unsigned short iD2, unsigned short iD3,
	unsigned char iD4_0, unsigned char iD4_1, unsigned char iD4_2, unsigned char iD4_3,
	unsigned char iD4_4, unsigned char iD4_5, unsigned char iD4_6, unsigned char iD4_7
    ) : BaseClass (iD1, iD2, iD3, iD4_0, iD4_1, iD4_2, iD4_3, iD4_4, iD4_5, iD4_6, iD4_7) {
    }
    VkUUIDHolder (char const *pString) : BaseClass (pString) {
    }
    VkUUIDHolder () {
    }

//  Update
public:
    void setTo (uuid_t const &rOther)  {
        m_iUUID = rOther;
    }
    ThisClass &operator= (uuid_t const &rOther)  {
        setTo (rOther);
	return *this;
    }
};


/*--------------------------*
 *----  VkUUIDGenerate  ----*
 *--------------------------*/

/********************************************************************************
 *****  Use this class if the default constructor should generate a new UUID  *****
 ********************************************************************************/

class V_API VkUUIDGenerate : public VkUUID {
//  Family Values
public:
    typedef VkUUID		BaseClass;
    typedef VkUUIDGenerate	ThisClass;

//  Construction
public:
    VkUUIDGenerate () : BaseClass (ReturnUUID ()) {
    }
};


#endif
