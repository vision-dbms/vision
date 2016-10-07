#!/bin/csh

####################
#  Script: installSpace.cmd
#     Create a new object space of supplied type
#  arg1 is name of space
#  arg2 is one of {DB, DBA, User}  - DB by default
#  derive description from arg1
####################

if ($#argv == 0) then
  echo ">>> Bad Inputs Supplied.  Must Include a Space Name."
  echo "usage: $argv[0] spaceName {spaceType}"
  exit 1
endif

set spaceName = $1
set type = "DB"

if ($#argv > 1 ) then
   set type = $2
endif

if ($type != "DBA" && $type != "DB" && $type != "User") then
  echo ">>> Bad Type Supplied.  Must Be One of {DBA DB User}."
  exit 1
endif

#-- make sure no space is preset for this user code
unsetenv UserOSI

#--  todo: what if ospace directory has been left over from a prior rollback???


#--  create new space if not already defined
$DBbatchvision << EOD | sed 's/V> //g'
   newLine print ;
   !space <- Environment at: "$type" . at: "$spaceName" ;
   space isntNA
     ifTrue: 
       [ ">>> $type Space " print ;
        "$spaceName" print ;
        " Already Defined.  Install Aborted." printNL ;
       ] 
    ifFalse:
       [ !desc <- "$spaceName Data" ;
        "$type" = "User"
         ifTrue:
           [ Environment MaintenanceTools 
               setupPrivateSpaceFor: "$spaceName" fullName: desc ;
           ]
         ifFalse:
           [ Environment MaintenanceTools 
                setupSpaceFor: "$spaceName" as: "$type" description: desc ;
           ] ;
         Utility createSpaceInNetworkWithAnnotation: 
            ("New Space Created for " concat: desc)  ;
        ] ; 
?g
EOD
