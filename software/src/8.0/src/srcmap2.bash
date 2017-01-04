#!/bin/bash

files=$(ls */*.cpp */*.[dhily] */*.[ly]o)

for file in $files; do
    echo "$(basename $file) > ../$file"
done
