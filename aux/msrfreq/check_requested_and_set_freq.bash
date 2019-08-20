#!/bin/bash 

CPUs=$(seq 0 24)

#SPEEDSTEP_ENABLE=$(sudo rdmsr -f 16:16 0x1A0 2>/dev/null)
#TURBOMODE_DISABLE=$(sudo rdmsr -f 38:38 0x1A0 2>/dev/null)
#echo -n "SpeedStep "
#echo $SPEEDSTEP_ENABLE
#
#echo -n "TurboMode "
#echo $TURBOMODE_DISABLE

echo -n "Req (0x199): "
for i in $CPUs
do
    V199=$(sudo rdmsr -u -f16:8 -p $i 0x199 2>/dev/null)
    if [ "${V199}x" != "x" ] ; then
        printf "%4u (0x%2x)  " $(($V199 * 100)) ${V199} 
    fi
done

sleep 0.1
# Note that it is not executed all at the same time so p-state can change in between
echo ""
echo -n "Cur (0x198): "
for i in $CPUs
do
    V198=$(sudo rdmsr -u -p $i -f16:8 0x198 2>/dev/null)
    if [ "${V198}x" != "x" ] ; then
        printf "%4u (0x%2x)  " $(($V198 * 100)) ${V198} 
    fi
done
echo ""


echo "-------------------------------------------------------------------------------------------------\n"
