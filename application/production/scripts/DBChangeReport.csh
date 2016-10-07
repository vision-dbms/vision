#!/bin/csh 
####################
# $osvProd/scripts/DBChangeReport.csh
#
# This script can be used to display the Schema change history of a database.
#
# Input Options:  
#   -n  <NDF file to validate>   {Default: $NDFPathName}
#   -b  <days back>              {Default: 1}
#   -s                           Summary only
#
####################

#-------------
# Environment
#-------------
set ndfToUse = $NDFPathName
set daysBack = 1
set summary = 0

#----------
#  get arguments
#----------
while ( $#argv > 0)
   switch ( $1 )
      case "-N":
      case "-n":
         shift
         set ndfToUse = $1
         shift
         breaksw
      case "-b":
         shift
         set daysBack = $1
         shift
         breaksw
      case "-s":
         shift
         set summary = 1
         breaksw
      default:
         shift
         breaksw
   endsw
end

##$DBbatchvision -n $ndfToUse << EOD |& sed 's/V> //g' >>& $processLog
$DBbatchvision -n $ndfToUse << EOD |& sed 's/V> //g'
[
!lastUpdate <- Schema lastPosting asLowResolutionTime asDate ;
!daysBack <- "$daysBack" asInteger ;

"Schema Change Report as of " print ; Utility UnixSeconds currentTime printNL ;
"userid:       " print ; Utility username printNL ;
"OSD:          " print ; osdPathName printNL ;
"Last Update:  " print ; lastUpdate printNL ;
"Last Posting: " print ; Schema lastPosting printNL ;
newLine print ;

#Schema processAllMessages ;

"Created in last posting:   " print ; Schema MID activeList select: [ createdInLastPosting ] . count printNL ;
"Refreshed in last posting: " print ; Schema MID activeList select: [ refreshedInLastPosting ] . count printNL ;
Schema MID activeList select: [ createdInLastPosting ] .
groupPrintUsing: [ :implementationSource lastDate ] ;
newLine print ;

!masterList <- Schema MID masterList #exclude: Schema MID activeList .
#Schema MID masterList select: [ code = "canonicalizedForSort" ] .
extendBy: [ !action ; ] .
groupedBy: [ deletionDate else: [ :implementationSource lastDate ] . else: [ ^today ] ] .
extendBy: [ ^self 
            evaluate: [ #^date printNL ;
                        groupList
                        do: [ #code print: 20 ;
                              #:implementationSource effectiveDate print: 9 ;
                              #:implementationSource firstDate print: 9 ;
                              #:implementationSource lastDate print: 9 ;
                              #isActive print: 9 ;
                              #deletionDate print: 9 ;
                              :action <- isActive not
                                    ifTrue: [ "deleted" ] .
                                    elseIf: [ :implementationSource count = 1 ]
                                    then: [ "new" ] .
                                    else: [ "changed" ] ;
                              #action print ;
                              #newLine print ;
          ]
] 
] ;

"Action by Date" printNL ;
"Date" print: 9 ; "new" print: -9 ; "deleted" print: -9 ; "changed" print: -9 ; "Total" printNL: -9 ;
masterList
do: [ ^self print: 9 ;
      groupList select: [ action = "new" ] . count print ;
      groupList select: [ action = "deleted" ] . count print ;
      groupList select: [ action = "changed" ] . count print ;
      groupList count print ;
      newLine print ;
    ] ;
newLine print ;
"Total       " print ; Schema MID masterList count printNL ;
"Total active" print ; Schema MID activeList count printNL ;
newLine print ;

"Deleted by Last Implementation Date" printNL ;
Schema MID masterList exclude: Schema MID activeList . groupPrintUsing: [ :implementationSource lastDate ] ;
newLine print ;

#------------------------------------------------------

$summary = 0
ifTrue:
[
"New in Last Posting" printNL ;
"===================" printNL ;
Schema MID activeList select: [ createdInLastPosting ] .
#select: [ :implementationSource lastDate = ^today ] .
sortUpBy: [ messageClassDescriptor name ] then: [ code ] .
do:
 [ messageClassDescriptor name print ; ": " print ;
   message print ;
   " - " print ; type code printNL ;
   implementationSource printNL ;
   "-" fill: 50 . printNL ;
 ] ;
newLine print ;

"Diffs in Last Posting" printNL ;
"=====================" printNL ;
Schema MID activeList select: [ refreshedInLastPosting ] .
  sortUp: [ code ] .
  sortUp: [ messageClassDescriptor name ] .
  sortUp: [ createdInLastPosting ifTrue: [ 1 ] ifFalse: [ 0 ] ] .
iterate: 
 [ messageClassDescriptor name print ; ": " print ;
   message print ;
   " - " print ; type code printNL ;
   !bef <- :implementationSource asOf: :implementationSource effectiveDate predecessor ; #- prior version
   !aft <- implementationSource ;                    #- new version
   "/tmp/bef.$$" substituteOutputOf: [ bef printNL ] ;       #- temporary files
   "/tmp/aft.$$" substituteOutputOf: [ aft printNL ] ;
   !diff <-  [ "diff -w /tmp/bef.$$ /tmp/aft.$$" filterOutputOf: [ ] ; ] divertOutput ;
   diff printNL ;
   "-" fill: 50 . printNL ;
 ] ;
newLine print ;
#------------------------------------------------------

masterList
sortUp: [ ^self ] . last: daysBack .
iterate:
[ "====================" printNL ;
  "Changes for " print ; ^self printNL ;
  "====================" printNL ;
  newLine print ;

  "New" printNL ;
  "===" printNL ;
  groupList select: [ action = "new" ] .
  sortUp: [ code ] .
  do: [ messageClassDescriptor name print ; ": " print ;
        message print ;
        " - " print ; type code printNL ;
        implementationSource printNL ;
        "-" fill: 50 . printNL ;
  ] ;
  newLine print ;

  "Deleted" printNL ;
  "=======" printNL ;
  groupList select: [ action = "deleted" ] .
  sortUp: [ code ] .
  do: [ messageClassDescriptor name print ; ": " print ;
        message print ;
        " - " print ; type code printNL ;
  ] ;
  newLine print ;

  "Changes" printNL ;
  "=======" printNL ;
  groupList select: [ action = "changed" ] .
  sortUpBy: [ code ] then: [ messageClassDescriptor name ] .
  iterate: 
  [ messageClassDescriptor name print ; ": " print ;
    message print ;
    " - " print ; type code printNL ;
    !bef <- :implementationSource asOf: ^my asSelf predecessor ; #- prior version
    !aft <- implementationSource ;                    #- new version
    "/tmp/bef.$$" substituteOutputOf: [ bef printNL ] ;       #- temporary files
    "/tmp/aft.$$" substituteOutputOf: [ aft printNL ] ;
    !diff <-  [ "diff -w /tmp/bef.$$ /tmp/aft.$$" filterOutputOf: [ ] ; ] divertOutput ;
    diff printNL ;
    "-" fill: 50 . printNL ;
  ] ;
] ;
] ;

] value ;
?g
EOD
\rm -f /tmp/bef.$$ /tmp/aft.$$
