#ifndef VComWrapperCache_Interface
#define VComWrapperCache_Interface

#if defined (WIN32_COM_SERVER)
#pragma once

/*********************
 *****  Library  *****
 *********************/

/************************
 *****  Components  *****
 ************************/

/**************************
 *****  Declarations  *****
 **************************/

/*************************
 *****  Definitions  *****
 *************************/

class VComWrapperCache {
public:
    VComWrapperCache () : m_pComWrapper (0) {
    }

    ~VComWrapperCache () {
	ATLASSERT (m_pComWrapper == NULL);
    }

public:
    void* comWrapper () const {
	return m_pComWrapper;
    }
    void setComWrapperTo (void* pComWrapper) {
	ATLASSERT (m_pComWrapper == NULL);
	m_pComWrapper = pComWrapper;
    }
    void clearComWrapper () {
	ATLASSERT (m_pComWrapper != NULL);
	m_pComWrapper = NULL;
    }

private:
    void* m_pComWrapper;
};
#endif

#endif
