#!/usr/bin/env bash
set -x

rm -rf *.elf

for binary in ndn
do
    for forwarders in 5 4 3 2 1
    do
        for producers in 10 8 6 4 2
        do
            NARR=""
            GWARR=""
            CFLAGS=""
            source ./testbed-$forwarders-$producers.sh

            rm -f idaddr.inc
            rm -f routesdown.inc
            rm -f routesup.inc

            for i in "${ROUTES[@]}"; do
                set -- $i
                node=$1; dest=$2; next=$3; dir=$4;
                set -- ${NODES[$node]}
                hwaddr=$1;
                set -- ${NODES[$dest]}
                gaddr=${2/#fe80/2001:db8}
                set -- ${NODES[$next]}
                naddr=$1
                printf "ROUTE(${node#tap},\"${hwaddr}\",\"${gaddr}\",\"${naddr}\")\n" >> routes${dir}.inc
            done
            for i in "${!NODES[@]}"; do
                set -- ${NODES[$i]}
                gaddr=${1/#fe80/2001:db8}
                printf "MYMAP(${i#tap},\"${gaddr}\")\n" >> idaddr.inc
                if [ "$3" -eq "0" ];then
                    GWARR="\"${gaddr}\"",${GWARR}
                fi
                if [ "$3" -eq "2" ];then
                    NARR="${i#tap},${NARR}"
                fi
            done

            NARR="-DNARR='{ ${NARR::-1} }'"
            GWARR="-DGWARR='{ ${GWARR::-1} }'"

            CFLAGS="${NARR} ${GWARR}" make -j5 clean all BOARD=iotlab-m3
            cp bin/iotlab-m3/app.elf $binary-$forwarders-$producers.elf
        done
    done
done
set +x
