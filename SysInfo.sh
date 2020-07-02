#!/bin/bash

#时间
nowtime=$(date +"%Y-%m-%d__%H:%M_%S")

#主机名
hostname=`hostname`

#OS版本
os_v=`cat /etc/redhat-release | awk '{printf("%s/%s", $1, $2);}'`

#内核版本
kernel_v=`uname -r`

#运行时间
run=`uptime -p | tr -s " " "\n" | awk '{printf("%s_", $1);}'`

#平均负载
loadavg=`cat /proc/loadavg | awk '{printf("%s %s %s", $1, $2, $3);}'`

#磁盘总量
disk_total=`df -m | grep "^/dev/" | awk 'BEGIN{total=0; used=0}{if (NR > 1) {total += $2; used += $3}}END{printf("%d:%d", total, used);}'`
disk_used=`echo $disk_total | cut -d ":" -f2`
disk_total=`echo $disk_total | cut -d ":" -f1`

#磁盘已用%
per_disk_used=`echo "$disk_used $disk_total" | awk '{printf("%d", $1 / $2 * 100);}'`

#内存大小
mem_total=`free -m | grep "^Mem" | awk '{printf("%s", $2);}'`

#内存已用%
per_mem_used=`free -m | grep "^Mem" | awk '{printf("%d", $3 / $2 * 100);}'`

#CPU温度
cpu_temp=`cat /sys/class/thermal/thermal_zone0/temp | awk '{printf("%d", $1 / 1000);}'`

#磁盘报警级别
if [[ $per_disk_used -ge 0 && $per_disk_used -le 80 ]];then
    disk_warn="normal"
elif [[ $per_disk_used -ge 81 && $per_disk_used -le 90 ]];then
    dirk_warn="note"
else
    dirk_warn="warning"
fi

#内存报警级别
if [[ $per_mem_used -ge 0 && $per_mem_used -le 70 ]];then
    mem_warn="normal"
elif [[ $per_mem_used -ge 71 && $per_mem_used -le 80 ]];then
    mem_warn="note"
else
    mem_warn="warning"
fi

#CPU报警级别
if [[ $cpu_temp -ge 0 && $cpu_temp -le 50 ]];then
    cpu_warn="normal"
elif [[ $cpu_temp -ge 51 && $cpu_temp -le 70 ]];then
    cpu_warn="note"
else
    cpu_warn="warning"
fi

echo "$nowtime $hostname $os_v $kernel_v $run $loadavg ${disk_total} ${per_disk_used}% ${mem_total} ${per_mem_used}% ${cpu_temp} ${disk_warn} ${mem_warn} ${cpu_warn}"
