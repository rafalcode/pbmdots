#!/bin/bash
# this script does what?
EXPECTED_ARGS=1 # change value to suit!
# some quick "argument accounting"
if [ $# -ne $EXPECTED_ARGS ]; then
        echo "Correct usage: $0 <TARGETFILE>"
        exit
fi
 
sed -e /^#/d $1 | tr ':' ',' |cut -d',' -f1-2
