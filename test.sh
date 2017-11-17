#!/bin/bash
for i in {1..1000}
do
   ./svr_c -d $1 -p $2 &
done

killall ./svr_c