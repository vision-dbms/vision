#!/bin/csh
set discname = /export/app

if (`hostname` == visdev01 || `hostname` == visstage01) then
   set discname = /export/data
endif

df -k $discname
