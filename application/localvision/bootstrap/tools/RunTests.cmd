#!/bin/csh

set homeDir = `pwd`
set testArea = $osvAdmin/localvision/bootstrap/tools/tests

echo -n "Use DBbatchvision [$DBbatchvision]? "
set input = $<
if ($input == "") then
  set version = $DBbatchvision
else
  set version = $input
endif

echo -n "Use NDF [$NDFPathName]? "
set input = $<
if ($input == "") then
  set database = $NDFPathName
else
  set database = $input
endif

echo -n "Output to [output]? "
set input = $<
if ($input == "") then
  set output = $homeDir/output
else
  set output = $input
endif
if (! -d $output) then
  echo creating $output directory
  mkdir $output
endif

cd $testArea
set filelist = (`ls` *.S)
cd $output

echo "i am in `pwd` "

cat /dev/null > test.log
echo	 "********************************************************">> test.log
echo	 "*  Test Suite Execution"			>> test.log
echo	 "********************************************************">> test.log


foreach file ($filelist)
$version -n $database >>& test.log << @@EOD@@
?? $testArea/$file

@@EOD@@

end


