#! /bin/bash

_ICCF=0 _CHESS960=0 _NOEDIT=1 bash bin/build.sh
sudo install -m 0755 adzchess /usr/local/bin
(adzchess &>anl/$(date +%y%m%d-%H%M).anl &)
