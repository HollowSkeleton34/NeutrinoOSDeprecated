#!/bin/bash
make
make_result=$?

echo Make Result: $make_result

if [ "$make_result" = "0" ]
then
    cp os ./os.img
    dd if=/dev/zero of=os.img bs=1 count=1 seek=1474560

    fsize=$(wc -c < os.img)
    sectors=$(( $fsize / 512 ))

    echo "Build finished successfully"
    echo "ALERT: Adjust boot sector to load $sectors sectors"
else
    result=`expr $boot_result + $make_result`
    echo "Build failed with error code $result. See output for more info."
fi