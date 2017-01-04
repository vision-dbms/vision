#!/bin/bash

plists=$(find . -name "make.plist*" | grep -v ./frontend)

# vcxprojs=$(find . -name "*.vcxproj")

for plist in $plists; do
    ppath=$(dirname $plist);
    pbase=$(basename $plist);
    files=$(cat $plist);
    if [[ "$pbase" == "make.plist" ]]; then
    	pname=$(basename $ppath)
    else
    	pname=${pbase#make.plist.}
    fi
#    echo "=== $pname $ppath"
    for file in $files; do
    	sbase=${file%.o};
	sfinds=$(cd $ppath; find .. -name ${sbase}.cpp -o -name ${sbase}.c -o -name ${sbase}.h | grep -v ../frontend);
	for sfind in $sfinds; do
	    echo "$(basename $sfind) > ${sfind/.$ppath\//}"
	done
    done;
done
