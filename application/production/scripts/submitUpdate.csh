#!/bin/csh

#--------------------
#  Script to submit vision code or data feed to production update queue.
#
#  Parameters can be supplied as arguments or interactively
#
#  The following arguments are available:
#       -f      feed submit
#       -i      incremental update
#       -c      code submit
#       -n      no prompt 
#
# -c   vision code submit
#     Additional arguments are {file name} {ospace} {test it}
#
# -f  data feed submit
#    Additional arguments are {file name} {feed name} {config file}
#
# -i  with -f, does an incremental data feed submit
#    No arguments
#--------------------

set userDir = (`pwd`)
set submitArea  =   $updateArea/submits
set wkspace = $submitArea/inProgress
set networkDir = $LocalVisionRoot/network
set user = $LOGNAME

set submitType = ""
set incremental = ""
set okayToPrompt = "yes"

###  Process arguments
set hasArgs = "no"
if ($#argv > 0 ) set hasArgs = "yes"
while ( $hasArgs == "yes" )
   set hasArgs = "no"
   switch ( $1 )
      case "-F":
      case "-f":
         set submitType = "Feed"
         set hasArgs = "yes"
         shift
         breaksw
      case "-I":
      case "-i":
	 set incremental = "yes"
         set hasArgs = "yes"
         shift
         breaksw
      case "-C":
      case "-c":
         set submitType = "Code"
         set hasArgs = "yes"
         shift
         breaksw
      case "-N":
      case "-n":
         set okayToPrompt = "no"
         set hasArgs = "yes"
         shift
         breaksw
   endsw
end

#---  Get Submit Type
if ($submitType == "" && $okayToPrompt == "yes") then
   echo -n "Are you submitting Vision code?  [Yes]: "
   set input = $<
   if ($input == "No" || $input == "NO" || $input == "no" ) then
      set submitType = "Feed"
   else
      set submitType = "Code"
   endif
   echo "--> This is a Vision $submitType submit."
endif

if ( $submitType == "") then
   echo ">>> No Submit Type Supplied.  Submit Terminated."
   echo "Usage:  visionSubmit [-c|-C] [-n] {file} {ospace} {test}"
   echo "        visionSubmit [-f|-F] [-i] [-n] {file} {feed} {config}"
   echo "where -c or -C indicates Vision Code File submit"
   echo "      -f or -F indicates Vision Feed File submit"
   echo "      -i or -I indicates Vision Incremental Feed File"
   echo "  and -n or -N inicates if user should be prompted for missing info."
   exit 1
endif

#----------
#  Get Input File
#----------
set inputName = "" 
if ($#argv > 0 ) set inputName = "$1"

if ($inputName == "" && $okayToPrompt == "yes") then
   while (0 == 0)
      echo -n "Vision Input $submitType File: "
      set inputName = $<
      if ($inputName != "") break
   end
endif


set tmpInputFile = tmpInput.$user
if ( $inputName == "") then
   echo ">>> No $submitType File Name Supplied. Submit Terminated."
   exit 1
else if (-f $inputName) then
  cat $inputName > $wkspace/$tmpInputFile  
else
   echo ">>> $submitType File $inputName Does Not Exist. Submit Terminated."
   exit 1
endif

#----------
#  Setup
#----------
cd $wkspace
set lastUse = $user
set tmpFile = tmp.$user

#--  file number is 0 or next available number.  The lastUse file
#--    contains the last number used for this user
set count = 0
if (-e $lastUse) then
  touch $tmpFile
  "rm" -f $tmpFile
  echo `cat $lastUse`+1 | /usr/bin/bc > $tmpFile
  set count = (`cat $tmpFile`)
  "rm" -f $tmpFile
endif
set fname = submit$submitType.$user.$count

####################
#  Submit Code
####################

if ($submitType == "Code") then

#----------
#  Get Object Space
#----------
   set ospace = 3
   if ($#argv > 1) set ospace = $2
   if ($okayToPrompt == "yes") then
      while (0 == 0 )
         echo -n "Run in Space [$ospace]: "
         set input = $<
         if ($input == "") break
         if (-d $networkDir/$input) then
            set ospace = $input
            break
         endif
        echo ">>> Not A Valid Object Space. Please Re-Enter."
      end
   endif

#----------
#  Create input file
#----------
   echo "#-- Job: $fname Submitted `date`" > $fname
   echo "#-- Run in U$ospace" >> $fname
   echo "#-- From: $userDir" >> $fname
   echo "#-- Input File: $inputName" >> $fname
   cat $tmpInputFile >> $fname
   "rm" -f $tmpInputFile

#----------
#  Test it unless user requests otherwise
#----------
   set fileOkay = 0
   set runTest = "yes"
   if ($#argv > 2) then
       set runTest = $3
   else if ($okayToPrompt == "yes") then
      echo -n "Test File Before Submitting? (Yes unless No, NO, or no): "
      set input = $<
      if ($input == "No" || $input == "NO" || $input == "no" ) then
         set runTest = "no"
      endif
   endif
   if ($runTest == "yes") then
      echo "...  Checking File - Please Wait"
      $DBbatchvision -U$ospace << EOD >& $tmpFile
         "$fname" asFileContents evaluate ;
?g
EOD
      if (`grep "<<<" $tmpFile | wc -l` != 0) set fileOkay = 1
      "rm" -f $tmpFile
   else if ($okayToPrompt == "yes") then
      echo "...  File Not Tested." 
   endif

#----------
#  Submit it if okay
#----------
   if ($fileOkay == 0) then
      mv $fname $statusArea/pending
      echo $count > $lastUse
      echo "***  Vision Code File Update Pending. File: $fname"
      date
   else
      echo ">>> Error Encountered in Code File. Not Processed.  <<<"
      "rm" -f $fname
   endif

   exit      #- done with vision code submit
endif


####################
#  Submit Feed
#    have file name - need feed name and optional config file name
####################

#----------
#  Get Feed Name
#----------
getfeed:

set feedName = "" ;
if ($#argv > 1) set feedName = $2
if ($feedName == "" && $okayToPrompt == "yes") then
   while (0 == 0)
      echo -n "Data Feed Name: "
      set feedName = $<
      if ($feedName != "") break
   end
endif
if ($feedName == "") then
   echo ">>> No Feed Name Supplied.  Submit Terminated."
   exit 1
endif

if ($incremental == "" && $okayToPrompt == "yes") then
   echo -n "Incremental? (No unless YES, Yes, or yes): "
   set input = $<
   if ($input == "YES" || $input == "Yes" || $input == "yes" ) then
      set incremental = "yes"
   else
      set incremental = "no"
   endif
endif

#----------
#  validate feed name
#----------
$DBbatchvision << EOD >& $tmpFile
    Interface BatchFeedManager validateFeedName: "$feedName" . printNL ;
?g
EOD

set feedOkay = 1
if (`grep "TRUE" $tmpFile | wc -l` == 1) set feedOkay = 0
"rm" -f $tmpFile
if ($feedOkay == 1 && $okayToPrompt == "yes") then
   echo ">>> Bad Data Feed Name $feedName Supplied.  Try Again."
   goto getfeed
else if ($feedOkay == 1) then
   echo ">>> Bad Data Feed Name $feedName Supplied.  Submit Terminated."
   exit 1
endif


#----------
#  Get optional Config File
#  Modified 1/4/2001 - to check for existence of passed config file
#----------
set configFile = "" 
if ($#argv > 2 ) then
  set testCfgFile = $3
  if (-f $testCfgFile) then
     set configFile = $3
  else
     set configFile = "" 
  endif
endif

if ($configFile == "" && $okayToPrompt == "yes") then
   while (0 == 0)
      echo -n "Optional Configuration File Name: "
      set configFile = $<
      if ($configFile == "") break
      if (-f $configFile) break
      if (-f $userDir/$configFile) then
        set tmp = $userDir/$configFile
        set configFile = $tmp
        break
      endif
      echo "File $configFile Not Found.  Try Again."
   end
endif

#----------
#  add path name to input file if needed
#----------
if (! -f $inputName) then
  set tmp = $userDir/$inputName
  set inputName = $tmp
endif

#----------
#  Create Vision Code File to submit 
#----------
echo "#-- Job: $fname Submitted `date`" > $fname
echo "#-- Run in U3" >> $fname
echo "#-- From: $userDir" >> $fname
echo "#-- Input File: $inputName" >> $fname
echo "" >> $fname
if ($incremental == "yes") then
  echo "$feedName enableIncremental;" >> $fname
endif
echo "Interface BatchFeedManager"      >> $fname
echo 	         upload: \"$feedName\"    >> $fname
echo	       usingFile: \"$inputName\"   >> $fname
echo	      withConfig: \"$configFile\"  >> $fname
echo ";"                               >> $fname

echo $fname

mv $fname $statusArea/pending
echo $count > $lastUse
echo "***  Vision Feed File Update Pending. File: $fname"
date
"rm" -f $tmpInputFile
