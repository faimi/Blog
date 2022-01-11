# TCP 流量控制

一般来说，我们总是希望数据传输得更快一些。但如果发送方把数据发送得过快，接收方就可能来不及接收，这就会造成数据的丢失。所谓**流量控制**(flow control)就是让发送方的发送速率不要太快，要让接收方来得及接收。利用**滑动窗口机制**可以很方便地在TCP连接上实现对发送方的流量控制。

<img src="./image/运输层/TCP流量控制.png" style="flex:1;" width="100%">

（1）
<img src="./image/运输层/正常字节发送.png" style="flex:1;" width="100%">
<img src="./image/运输层/第一次流量控制.png" style="flex:1;" width="100%">
<img src="./image/运输层/删除1-200字节数据.png" style="flex:1;" width="100%">

（2）
<img src="./image/运输层/正常发送2.png" style="flex:1;" width="100%">
<img src="./image/运输层/重传计时器超时.png" style="flex:1;" width="100%">

（3）
<img src="./image/运输层/第二次流量控制.png" style="flex:1;" width="100%">
<img src="./image/运输层/删除201-500字节数据.png" style="flex:1;" width="100%">

（4）
<img src="./image/运输层/第三次流量控制.png" style="flex:1;" width="100%">

死锁：
<img src="./image/运输层/死锁.png" style="flex:1;" width="100%">

<img src="./image/运输层/打破死局.png" style="flex:1;" width="100%">
