#!/bin/bash
# this script does what?
EXPECTED_ARGS=2 # change value to suit!
# some quick "argument accounting"
if [ $# -ne $EXPECTED_ARGS ]; then
        echo "Correct usage: $0 <TARGETFILE> <FIELD>"
        exit
fi

TARGETFILE=$1
FIELD=$2
cat $TARGETFILE | tr ':' ',' |cut -d"," -f${FIELD}
