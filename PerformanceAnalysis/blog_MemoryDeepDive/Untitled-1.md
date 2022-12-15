## 
内存之所以有多通道是因为cpu集成了多个内存控制器
## term
region：如果一个region没有用满所有的memory channel 那么这个region的吞吐量就没用满的那么高，所以插内存的时候尽量在一个region插满，而不是插到两个region上