#!/bin/bash
make
make_result=$?

echo Make Result: $make_result

if [ "$make_result" = "0" ]
then
    cp os ./os.img

    fsize=$(wc -c < os.img)
    sectors=$(( $fsize / 512 ))

    echo "ALERT: Adjust boot sector to load $sectors sectors"

    dd if=/dev/zero of=os.img bs=1 count=1 seek=1474560
    
    echo "Build finished successfully"
else
    result=`expr $boot_result + $make_result`
    echo "Build failed with error code $result. See output for more info."
fi