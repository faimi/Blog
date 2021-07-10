bert是unsupervise train的transformer，transformer是Seq2seq model（补），transformer特殊的地方就是在Seq2seq model中用到了大量的self-attention layer。

## self-attention layer 做的是什么

### 背景
#### RNN
要处理一个sequence，最常想到要拿来用的network架构就是RNN（补），RNN的输入就是一串sequence，输出又是另外一串sequence，RNN的问题是不容易被并行化。
#### CNN

每一个三角形代表一个filter，filter的输入就是sequence其中的一小段，跟filter里面的参数做inner product，输出一个数值。filter扫过sequence，产生一排数值。filter不止一个，每一个会产生一排不同的数值。
CNN可以做到和RNN同样的输入和输出。缺点是CNN只能考虑非常有限的内容，它也可以考虑比较长的咨询，就是叠加很多层CNN，上层的filter就可以考虑比较多的资讯。CNN的好处是可以并行化，不需要等第一个三角形算完再算第二个，也不需要等红色的builder算完再算黄色的builder。

![](./images/RNN%20CNN.png)

### self-attention

self-attention layer要做的事情就是取代RNN，他也是输入sequence输出一个sequence