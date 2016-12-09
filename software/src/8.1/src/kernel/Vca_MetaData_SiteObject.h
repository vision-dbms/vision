#ifndef Vca_MetaData_SiteObject_Interface
#define Vca_MetaData_SiteObject_Interface

/************************
 *****  Components  ***** 
 ************************/

#include "Vca_MetaData_Object.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "Vca_MetaData_Site.h"

/*************************
 *****  Definitions  *****
 *************************/

namespace Vca {
    namespace MetaData {
        /**
         * Base class for classes of site-specific objects.
         *
         * @see Vca::MetaData::Peer
         */
	class Vca_MetaData_API SiteObject : public Object {
	    DECLARE_ABSTRACT_RTTLITE (SiteObject, Object);

	//  Construction
	protected:
            /** Constructor expects the 'owner', or context, of this SiteObject. */
	    SiteObject (Site* pSite);

	//  Destruction
	protected:
	    ~SiteObject ();

	//  Roles
	public:
	    using BaseClass::getRole;

	//  Access
	public:
            /** Returns the 'owner', or context, of this SiteObject. */
	    Site* site () const {
		return m_pSite;
	    }

	//  State
	private:
            /** The 'owner', or context, of this SiteObject. */
	    Site::Reference const m_pSite;
	};
    }
}


#endif
