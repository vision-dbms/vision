#ifndef CConnectionController_Interface
#define CConnectionController_Interface

/************************
 *****  Components  *****
 ************************/

#include "ConnectionData.h"

/**************************
 *****  Declarations  *****
 **************************/

/*************************
 *****  Definitions  *****
 *************************/

class CConnectionController : public CConnectionData
{
//  Construction
public:
    CConnectionController (CConnectionData& rConnectionData);

//  Operations
public:
//    BOOL operator== (CConnectionController& operand) const;

//  State
protected:
};

#endif
