#!/usr/bin/env bash
set -x

rm -rf *.elf

#for binary in coap-proxy-cache coap
for binary in coap-proxy-cache
do
    if [ $binary = "coap-proxy-cache" ]; then
        COAP_PROXY=1
    else
        COAP_PROXY=0
    fi
    for forwarders in 1 2 3 4 5
    do
        for producers in 2 4 6 8 10
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
                naddr=$2
                printf "ROUTE(${node#tap},\"${hwaddr}\",\"${gaddr}\",\"${naddr}\")\n" >> routes${dir}.inc
            done
            for i in "${!NODES[@]}"; do
                set -- ${NODES[$i]}
                gaddr=${2/#fe80/2001:db8}
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

            CFLAGS="${NARR} ${GWARR}" make -j5 clean all BOARD=iotlab-m3 COAP_PROXY=$COAP_PROXY
            cp bin/iotlab-m3/app.elf $binary-$forwarders-$producers.elf
        done
    done
done
set +x
