# TSR version 0.1
this software relys on fprobe,fprobe is a libpcap-based tool that collect network traffic data and 
emit it as NetFlow flows towards the specified collector.you can install it by source code:
./configure
make
make install

NOTE:
1.some error may happen when you install it,but you can easily find the solution on the internet.
2.flows emitted by fprobe have incorrect start time and end time,if you are interested in these 
  two fields,you can modify the source code before you compile it.

TSR is consist of two parts:storage module and retrial module.
storage command:sudo ./test opt name
opt:
    -i:correspond to interface name
    -f:correspond to pcap file name
NOTE:
    1.opt is essential
    2.storage result is stored in the dataDir/ and index/

retrial command:sudo ./testRetrial opt indexname indexvalue
opt:
    -p:print format result on the screen besides raw result
    -q:store format result in the file besides raw result
indexname:
    srcIp
    srcPort
    dstIp
    dstPort
NOTE:
    1.opt is not essential
    2.retrial result is stored in the result/

