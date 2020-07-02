#!/bin/bash

Time=`date +"%Y-%m-%d__%H:%M:%S"`
LoadAvg=`cut -d " " -f 1-3 /proc/loadavg`
CpuTemp=`cat /sys/class/thermal/thermal_zone0/temp`

#bc 相当于一个计算器，bc 在 ``里面
CpuTemp=`echo "scale=2;${CpuTemp}/1000" | bc`

eval $(head -n 1 /proc/stat |awk -v Sum1=0 \
    '{for (i=2;i<=11;i++){Sum1+=$i} printf("Sum1=%d;Idle1=%d", Sum1, $5)}')

sleep 0.5

eval $(head -n 1 /proc/stat |awk -v Sum2=0 \
    '{for (i=2;i<=11;i++){Sum2+=$i} printf("Sum2=%d;Idle2=%d", Sum2, $5)}')

CpuUsedPerc=`echo "scale=4;(1-(${Idle2}-${Idle1})/(${Sum2}-${Sum1}))*100" | bc`

CpuUsedPerc=`printf "%.2f" "${CpuUsedPerc}"`

WarnLevel="normal"

#利用bc -l 比较小数的大小
if [[ `echo "${CpuTemp} >= 70" | bc -l` == 1 ]];then
    WarnLevel="warning"
elif [[ `echo "${CpuTemp} >= 50" | bc -l` == 1 ]];then
    WarnLevel="note"
fi

echo "${Time} ${LoadAvg} ${CpuUsedPerc} ${CpuTemp}℃  ${WarnLevel}"
