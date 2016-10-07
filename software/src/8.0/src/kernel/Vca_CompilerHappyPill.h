#ifndef Vca_CompilerHappyPill
#define Vca_CompilerHappyPill

//  The cure for the lobotomized compiler...

#if defined(_WIN32)

//  ... and only Microsoft has an attitude :-<>

#include "V_VThread.h"	// omit:Linux omit:SunOS

#include "Vca_IError.h"		// omit:Linux omit:SunOS
#include "Vca_IListener.h"	// omit:Linux omit:SunOS
#include "Vca_ITrigger.h"	// omit:Linux omit:SunOS

#include "Vca_VcaConnection.h"	// omit:Linux omit:SunOS
#include "Vca_VcaPeer.h"	// omit:Linux omit:SunOS
#include "Vca_VcaSelf.h"	// omit:Linux omit:SunOS
#include "Vca_VcaThreadState.h"	// omit:Linux omit:SunOS
#include "Vca_VcaTransport.h"	// omit:Linux omit:SunOS

#include "Vca_VGofering.h"	// omit:Linux omit:SunOS

#include "Vca_VTimer.h"		// omit:Linux omit:SunOS

#endif	// defined(_WIN32)

#endif
