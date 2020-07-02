#!/bin/bash
Time=`date +"%F__%T"`
HostName=`hostname`
OsType=`cat /etc/issue.net | tr " " "_"`
KernelVersion=`uname -r`
LoadAvg=`cut -d " " -f 1-3 /proc/loadavg`
UpTime=`uptime -p | tr -s " " "_"`
eval `df -T -x devtmpfs -x tmpfs -m --total | tail -n 1 | awk \
    '{printf("DisTotal=%s;DiskUsedP=%s", $3, $6)}'`
DisWarningLevel="normal"

if [[ ${DisUsedP} -gt 90 ]];then
    DisWarningLevel="warning"
elif [[ ${DiskUsedp} -gt 80 ]];then
    DisWarningLevel="note"
fi

MemWarningLevel="normal"

eval `free -m | head -n 2 | tail -n 1 | awk \
    '{printf("MemTotal=%s;MemUsed=%s", $2, $3)}'`
MemUsedP=$[ ${MemUsed}*100/${MemTotal} ]

if [[ ${MemUsedP} -gt 80 ]];then
    MemWarningLevel="warning"
elif [[ ${MemUsedP} -gt 70 ]];then
    MemWarningLevel="note"
fi

CpuTemp=`cat /sys/class/thermal/thermal_zone0/temp`
CpuTemp=`echo "scale=2;${CpuTemp}/1000" | bc`

CpuWarning="normal"
if [[ `echo "${CpuTemp} >= 70" | bc -l` -eq 1 ]];then
    CpuWarning="warning"
elif [[ `echo "${CpuTemp} >= 50" | bc -l` = 1 ]];then
    CpuWarning="note"
fi

echo "${Time} ${HostName} ${OsType} ${KernelVersion} ${UpTime} ${LoadAvg} ${DiskTotal} ${DiskUsedp} ${MemTotal} ${MemUsedP} ${CpuTemp} ${DisWarningLevel} ${MemWarningLevel} ${CpuWarning}"
