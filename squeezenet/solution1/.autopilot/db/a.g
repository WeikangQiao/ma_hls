#!/bin/sh
lli=${LLVMINTERP-lli}
exec $lli \
    /home/sem14h17/MA/ma_hls/squeezenet/solution1/.autopilot/db/a.g.bc ${1+"$@"}
