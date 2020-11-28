#!/bin/bash

TFILE=/tmp/shuffle3-test-$(uuidgen)
TFILE2=$TFILE-cmp

FSIZE=${FSIZE:-$(( 1024 * 1024 * 10))}
FCNT=${FCNT:-4}

TRUE_SIZE=$(( FSIZE * FCNT))

function cleanup {
	rm -f $TFILE $TFILE2
}

trap cleanup EXIT

stest() {
	if fcmp $TFILE $TFILE2; then
		echo ": shuffling $TRUE_SIZE bytes"
		$1 -s $TFILE
		if fcmp $TFILE $TFILE2; then
			echo "Failed: Shuffle did nothing"
			exit 1
		else
			echo ": unshuffling $TRUE_SIZE bytes"
			$1 -u $TFILE
			if fcmp $TFILE $TFILE2; then
				echo "OK"	
			else
				echo "Failed: Unshuffle didn't reproduce original file"
				exit 1
			fi
		fi
	else
		echo "Invalid init: Files weren't identical"
		exit -1
	fi
}

echo ">>> Initialising"
dd if=/dev/urandom of=$TFILE bs=$FSIZE count=$FCNT >> /dev/null 2>&1 || exit -1
cp $TFILE $TFILE2 || exit -1

for ex in "$@"; do
	if [[ -f "$ex" ]]; then
		echo ">>> Testing $ex"
		stest "$ex" || exit 1
	fi
done

echo "Passed."
