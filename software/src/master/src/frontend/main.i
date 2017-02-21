/****************************************
 *****  System Option Descriptions  *****
 ****************************************/

/*---------------------------------------------------------------------------
 * This file is included by both the Research System and the Editor.  Both
 * programs need to have all the option descriptions or GOPT does not work
 * correctly.
 *
 * Lower case option letters usually apply to the language processor while
 * upper case option letters usually apply to the front end.
 *---------------------------------------------------------------------------
 */

GOPT_BeginOptionDescriptions

/*****  Language Processor Options  *****/

/*----  Network Specification  ----*/
    GOPT_ValueOptionDescription (
	"NDFPathName"			, 'n', "NDFPathName"	, "/vision/network/NDF"
    )

    GOPT_ValueOptionDescription (
	"UserObjectSpaceIndex"		, 'U', "UserOSI"	, "0"
    )

    GOPT_ValueOptionDescription (
	"NetworkVersion"		, 'v', NilOf(char *), NilOf (char *)
    )

    GOPT_ValueOptionDescription (
	"StartupExpression"		, 'x', "VisionStartExpr", ""
    )

/*----  Network Update and Maintenance  ----*/
    GOPT_ValueOptionDescription	(
	"XUSpecPathName"		, 'I', NilOf(char *), NilOf (char *)
    )

    GOPT_ValueOptionDescription (
	"CompactNetwork"   		, 'C', NilOf(char *), NilOf (char *)
    )
    GOPT_SwitchOptionDescription (
	"NetCompactionStatisticsOnly"	, 's', NilOf (char *)
    )

    GOPT_ValueOptionDescription (
	"OSDPathName"			, 'o', "OSDPathName"	, NilOf (char *)
    )
    GOPT_ValueOptionDescription (
	"RebuildNDF"	    		, 'R', NilOf(char *), NilOf (char *)
    )
    GOPT_SwitchOptionDescription (
	"BootStrap"	    		, 'b', "VisionBoot"
    )

/*----  Privileges  ----*/
    GOPT_SwitchOptionDescription (
	"Administrator"			, 'a', "VisionAdm"
    )
    GOPT_SwitchOptionDescription (
	"Developer"			, 'd', "VisionDev"
    )

/*----  Interrupt and Front-end Support  ----*/
    GOPT_SwitchOptionDescription (
	"IgnoreInterrupts"		, 'i', NilOf (char *)
    )
    GOPT_SwitchOptionDescription (
	"AllowQuit"			, 'q', NilOf (char *)
    )
    GOPT_SwitchOptionDescription (
	"FromEditor"			, 'E', NilOf (char *)
    )

/*----  Tuning Parameters  ----*/
    GOPT_ValueOptionDescription (
	"LargeTaskSize"		, 't', "LargeTaskSize"	, "50000"
    )

/*****  Front End Options  *****/
    GOPT_ValueOptionDescription (
	"WhichRS"			, 'F', "WhichRS"	, "batchvision"
    )
    GOPT_SwitchOptionDescription (
	"Debug"				, 'D', NilOf (char *)
    )
    GOPT_ValueOptionDescription (
	"ProfileFile"			, 'S', "VisionProfile", NilOf (char *)
    )

/*****  Combined Options  *****/

GOPT_EndOptionDescriptions;
