## 什么是 Pre-train Model

在没有 ELMO 、 BERT 之前，已经有了 Pre-train Model 。 Pre-train Model 要做的事情就是希望把输入的每一个 token 表示成一个 embedding vector ， vector 包含了 token 的语义，意思相近的 token 有比较相近的 embedding ， embedding 中的某些维度可以看出语义的关联性。

### Contextualized Word Embedding

ELMO 、 BERT 等都是 Contextualized Word Embedding 。过去的模型吃一个 token 就吐一个 embedding ，现在都是吃一整个句子（看过上下文）再给每一个 token 一个 embedding ，现在就可以根据上下文不同对相同的 token 给不同的 embedding 。

### Smaller Model

![](./images/smaller%20model.png)

问：怎么让 model 变小？
答：1、 Network Pruning 
    2、 Knowledge Distillation
    3、 Parameter Quantization
    4、 Architecture Design

问：Network Architecture的突破？
答：1、Transformer-XL （读很长的句子甚至文章甚至书）
    2、Reformer （减少 self-attention 带来的运算量）
    3、Longformer （减少 self-attention 带来的运算量）

## 如何做 fine-tune

### input

input 分两种可能： one sentence 和 multiple sentences 。

![](./images/multiple%20input.png)

### output

output 分四种可能： one class 、 class for each token 、 copy from input 和 general sequence 。
