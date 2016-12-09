#ifndef Vca_VDirectoryBuilder_Interface
#define Vca_VDirectoryBuilder_Interface

/*********************
 *****  Library  *****
 *********************/

#include "Vca.h"

/************************
 *****  Components  *****
 ************************/

#include "VReferenceable.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "Vca_VDirectory.h"

/*************************
 *****  Definitions  *****
 *************************/

namespace Vca {
    /** Used to populate a directory. Subclasses implement rules by which population is governed. */
    class ABSTRACT Vca_API VDirectoryBuilder : public VReferenceable {
	DECLARE_ABSTRACT_RTTLITE (VDirectoryBuilder, VReferenceable);

    //  Construction
    public:
        /** Empty default constructor. */
	VDirectoryBuilder ();

    //  Destruction
    protected:
        /** Empty destructor restricts scope. */
	~VDirectoryBuilder ();

    //  Directory Creation
    private:
	/**
	 * Virtual member called to create a new directory on behalf of create(VDirectory::Reference& rpDirectory).
	 * The default implementation of this function returns a new VDirectory.
	 */
	virtual VDirectory* newDirectory_() const;
    public:
        /**
         * Used to create a directory according to the rules specified by this directory builder.
         * This is the meat of VDirectoryBuilder and should be implemented by subclasses.
         *
         * @param rpDirectory a reference which will be set to the created and populated directory.
         */
	void create (VDirectory::Reference& rpDirectory);

        /**
         * Used to populate a directory according to the rules specified by this directory builder.
         * This is the meat of VDirectoryBuilder and should be implemented by subclasses.
         *
         * @param pDirectory the directory that should be populated.
         */
	virtual void build (VDirectory *pDirectory) = 0;
    };
}


#endif
