# TSR version 0.1
this software relys on fprobe,fprobe is a libpcap-based tool that collect network traffic data and 
emit it as NetFlow flows towards the specified collector.you can install it by source code:</br>
./configure</br>
make</br>
make install</br>
</br>
NOTE:</br>
1.some error may happen when you install it,but you can easily find the solution on the internet.</br>
2.flows emitted by fprobe have incorrect start time and end time,if you are interested in these 
  two fields,you can modify the source code before you compile it.</br>

TSR is consist of two parts:storage module and retrial module.</br>
storage module setup command:sudo ./test opt name [-t] </br>
opt:</br>
  -i:correspond to interface name</br>
  -f:correspond to pcap file name</br>
name: interface name or pcap file name</br>
-t: this option is optional.when you use this option,it means you want to use DB to manage the index file
and data file,DB will record the timestamp info of file,so you can specify the time block in retrial. </br>
NOTE:</br>
  1.opt is essential</br>
  2.storage result is stored in the dataDir/ and index/</br>
  3.make sure you have installed mysql and created corresponding database before you use option -t.</br>
</br>
retrial module setup command:sudo ./testRetrial [-t]</br>
-t:you can specify the time block in retrial command when you use this option.</br>
</br>
retrial command: retrieve opt retrialExpression [-s startTime] [-e endTime]</br>
example: retrieve -p -s 2018-03-01 10:10:10 -e 2018-03-01 10:10:20 srcIp==192.168.1.1 && dstPort==80 </br>
opt:</br>
  -p:print format result on the screen besides raw result</br>
  -q:store format result in the file besides raw result</br>
retrialExpression:</br>
  support four index fields and three operators</br>
index field:</br>
  srcIp</br>
  srcPort</br>
  dstIp</br>
  dstPort</br>
operator:</br>
  == && || ()</br>
startTime or endTime:year-month-day hour:minute:second.</br>
if you do not specify date or year,the date will be current date.if you do not specify time, the time will be 00:00:00 in startTime,23:59:59 in endTime.for example,if current date is 2018-03-01,then 12:00:00 means 2018-03-01 12:00:00,03-01 12:00:00 means2018-03-01 12:00:00 too.2018-03-01 means 2018-03-01 00:00:00 in startTime,2018-03-01 23:59:59 in endTime</br>
</br>
NOTE:</br>
  1.opt is not essential</br>
  2.retrial result is stored in the result/</br>

