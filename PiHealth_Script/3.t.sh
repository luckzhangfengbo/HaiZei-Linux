#!/bin/bash
#Time=${date +"%F__%T"}
#echo ${Time}
df -T -m -x tmpfs -x  devtmpfs | tail -n +2 | awk -v time=$(date +"%F__%T") '
BEGIN{
    total=0;
    used=0;
    available=0;
}
{
    total += $3;
    used += $4;
    available += $5;
    printf("%s 1 %s %sM %sM %s\n", time, $7, $3, $5, $6);
}
END{
    printf("%s 0 disk %dM %dM %.2f%%\n", time, total, available, used / total * 100);
}'


