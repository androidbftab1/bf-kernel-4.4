#!/bin/sh
#
# $Id$
#
# usage: cpubysu.sh
#

echo ""
echo "!! 'killall cpubusy.sh' to Break !!"
echo ""

file="/tmp/dummy"
cpfile="/tmp/testcopy"
i=0
x=0

while true; do
  i=`expr $i + 1`
  echo "--- $i ---" > ${file}
  j=0
  while [ $j -lt 1000 ]; do
    j=`expr $j + 1`
    echo " cpu dummy echo for the $i, $j times" >> ${file}
    cp /bin/busybox ${cpfile}
  done
  sleep 1
done

