#ifndef Vsa_CompilerHappyPill
#define Vsa_CompilerHappyPill

//  The cure for the lobotomized compiler...

//  Only Microsoft has an attitude
#if defined(_WIN32)

#include "Vca_VTimer.h"		// omit:Linux omit:SunOS
#include "Vca_VTrigger.h"	// omit:Linux omit:SunOS

#include "Vca_VBSConsumer.h"	// omit:Linux omit:SunOS
#include "Vca_VBSProducer.h"	// omit:Linux omit:SunOS

#include "Vsa_VEvaluation.h"	// omit:Linux omit:SunOS
#include "Vsa_VEvaluatorPool.h"
#include "Vsa_VPoolWorkerGeneration.h" // omit:Linux omit:SunOS
#include "Vsa_VPoolBroadcast.h" // omit:Linux omit:SunOS

#endif

#endif
