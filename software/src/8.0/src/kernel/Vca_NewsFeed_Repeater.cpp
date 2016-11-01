/*****  Vca NewsFeed Repeater  *****/

/********************
 *****  System  *****
 ********************/

#include "Vk.h"

/******************
 *****  Self  *****
 ******************/

#include "Vca_NewsFeed_Repeater.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vca_VOneAppMain_.h"

#include "Vps_VRelayPublicationDirectory.h"


/*************************************
 *************************************
 *****                           *****
 *****  Vca::NewsFeed::Repeater  *****
 *****                           *****
 *************************************
 *************************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::NewsFeed::Repeater::Repeater (Context *pContext) : BaseClass (pContext) {
    aggregate (new Vps::VRelayPublicationDirectory (commandStringValue ("source", "VcaNewsSource", "NEWS_SERVICE")));
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vca::NewsFeed::Repeater::~Repeater () {
}


/**********************************
 **********************************
 *****  Run State Transition  *****
 **********************************
 **********************************/

bool Vca::NewsFeed::Repeater::start_() {
    if (BaseClass::start_() && !offerSelf ()) {
	displayUsage ();
	setExitStatus (ErrorExitValue);
    }
    return isStarting ();
}

/*******************
 *******************
 *****  Usage  *****
 *******************
 *******************/

void Vca::NewsFeed::Repeater::displayUsage () const {
    fprintf (stderr, "Usage: No server or callback address supplied.\n");
}

/**************************
 **************************
 *****  Main Program  *****
 **************************
 **************************/

int main (int argc, char *argv[]) {
    Vca::VOneAppMain_<Vca::NewsFeed::Repeater> iMain (argc, argv);
    return iMain.processEvents ();
}
