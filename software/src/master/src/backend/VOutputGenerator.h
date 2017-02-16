#ifndef VOutputGenerator_Interface
#define VOutputGenerator_Interface

/************************
 *****  Components  *****
 ************************/

#include "VTransient.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "VTask.h"

/*************************
 *****  Definitions  *****
 *************************/

class VOutputGenerator : public VTransient {
//  Construction
public:
    VOutputGenerator (VTask* pTask)
    : m_pOutputBuffer (pTask->getOutputBuffer ()), m_ppRingHead (m_pOutputBuffer->rings ())
    {
    }

//  Destruction
public:
    ~VOutputGenerator () {
    }

//  Access
public:
    VOutputBuffer* outputBuffer () const {
	return m_pOutputBuffer;
    }

//  Output Generation
public:
    void __cdecl printf (size_t size, char const* format, ...) const;

    void __cdecl printf (
	size_t size, int fieldWidth, int overflowChar, char const* format, ...
    ) const;

    void vprintf (size_t size, char const* format, va_list ap) const {
	m_pOutputBuffer->vprintf (m_ppRingHead, size, format, ap);
    }

    void vprintf (
	size_t		size,
	int		fieldWidth,
	int		overflowChar,
	char const*	format,
	va_list		ap
    ) const {
	m_pOutputBuffer->vprintf (m_ppRingHead, size, fieldWidth, overflowChar, format, ap);
    }

    void printWithCommas (
	size_t		size,
	int		overflowChar,
	int		fieldSpecification,
	int		precision,
	double		number
    ) const {
	m_pOutputBuffer->printWithCommas (
	    m_ppRingHead, size, overflowChar, fieldSpecification, precision, number
	);
    }

    void putString (char const* string) const {
	m_pOutputBuffer->putString (m_ppRingHead, string);
    }

//  Cursor Movement
public:
    void advance () {
	m_ppRingHead++;
    }

//  State
protected:
    VOutputBuffer* const	m_pOutputBuffer;
    VOutputBuffer::Cell**	m_ppRingHead;
};


#endif
