#!/bin/bash
if [[ $# -lt 1 ]];then
    echo "Usage:%0 DyAver"
fi

Time=`date +"%F__%T"`
DyAver=$1

#判断DyAver是否为空
if [[ ${DyAver}x == x ]];then
    exit 1
fi

#用数组的方式把需要的信息存储起来
MemValues=(`free -m | head -2 | tail -1 | awk '{printf("%s %s", $2, $3);}'`)

#内存当前占用率
MemUsedPerc=`echo "scale=1; ${MemValues[1]}*100/${MemValues[0]}" | bc`

#占用百分比动态平均值
#动态平均值=0。3动态平均值(上一次) + 0.7当前占用比
NowAver=`echo "scale=1; 0.7*${MemUsedPerc}+03.*${DyAver}" | bc`
echo "${Time} ${MemValues[0]}M $[${MemValues[0]}-${MemValues[1]}]M ${MemUsedPerc}% ${NowAver}%"
