#ifndef Vca_Registry_VAuthority_Interface
#define Vca_Registry_VAuthority_Interface

/************************
 *****  Components  *****
 ************************/

#include "Vca_Registry_IAuthority.h"

/**************************
 *****  Declarations  *****
 **************************/

/*************************
 *****  Definitions  *****
 *************************/

namespace Vca {
    namespace Registry {
	class Vca_Registry_API VAuthority {
	    DECLARE_NUCLEAR_FAMILY (VAuthority);

	//  Path
	public:
	    class Vca_Registry_API Path : public VReferenceable {
		DECLARE_CONCRETE_RTTLITE (Path, VReferenceable);

	    //  Construction
	    public:
		Path (ThisClass *pPrefix, IAuthority *pAuthority);

	    //  Destruction
	    private:
		~Path ();

	    //  Access
	    public:
		IAuthority *authority () const {
		    return m_pAuthority;
		}
		unsigned int length () const {
		    return m_iLength;
		}
		ThisClass *prefix () const {
		    return m_pPrefix;
		}

	    //  State
	    private:
		IAuthority::Reference const m_pAuthority;
		Reference             const m_pPrefix;
		unsigned int          const m_iLength;
	    };

	//  Serializer
	public:
	    class Serializer;
	    friend class Serializer;

	//  Construction
	public:
	    VAuthority (ThisClass const &rPrefix, IAuthority *pAuthority);
	    VAuthority (ThisClass const &rOther);
	    VAuthority ();

	//  Destruction
	public:
	    ~VAuthority ();

	    //  Access
	public:
	    Path *path () const {
		return m_pPath;
	    }
	    unsigned int pathLength () const {
		return m_pPath ? m_pPath->length () : 0;
	    }

	//  State
	private:
	    Path::Reference m_pPath;
	};
    }  // namespace Registry
}  // namespace Vca

/****************************
 *****  Instantiations  *****
 ****************************/

#  if defined(USING_HIDDEN_DEFAULT_VISIBILITY) || defined (Vca_Registry_VAuthority_Implementation)
#    include "Vca_VTypeInfoHolder.h"
#    ifndef Vca_Registry_VAuthority_Implementation
#      define Vca_Registry_VAuthority_Implementation extern
#    endif

Vca_Registry_VAuthority_Implementation template class Vca_Registry_API Vca::VTypeInfoHolder<Vca::Registry::VAuthority>;
Vca_Registry_VAuthority_Implementation template class Vca_Registry_API Vca::VTypeInfoHolder<Vca::Registry::VAuthority const&>;

#  endif


#endif
