#ifndef VSorter_Interface
#define VSorter_Interface

/************************
 *****  Components  *****
 ************************/

#include "VTransient.h"

/**************************
 *****  Declarations  *****
 **************************/

class VCollectionOfOrderables;

/*************************
 *****  Definitions  *****
 *************************/

class VSorter : public VTransient {
//  Construction
public:
    VSorter (VCollectionOfOrderables const& rOrderableCollection);

//  Destruction
public:
    ~VSorter ();

//  Heap Sort
public:
    void hsort (void *base, size_t nel, size_t sel) {
	hsort ((char*)base, nel, sel);
    }

private:
    void hsort (char *base, size_t nel, size_t sel);

    void buildheap (char *base, size_t nel, size_t sel);

    void heapify (
	char *		base,
	unsigned int	i,
	unsigned int	j,
	size_t		sel
    );

//  Algorithm Support
private:
    void interchange (
	char * base, unsigned int el1, unsigned int el2, size_t sel
    );

//  State
protected:
    VCollectionOfOrderables const&	m_rOrderableCollection;
    void*				m_pElementBuffer;
    size_t				m_sElementBuffer;
};


#endif
