# An improved data stream summary: the count-min sketch and its applications
## 
Count-Min Sketch是一个概率数据结构。其可以在很小空间占用的基础上返回一个在集合A中出现x的频率近似值。
## 
一个CM Sketch维护多个($O(log \frac{1}{\delta})$)计数器队列，每一个队列有$O(\frac{1}{\epsilon})$个计数器。对于每一个来到的元素x被hash映射到每个队列中的某个计数器上，计数器加一，估算x的频率只需要CM sketch 数据结构返回x映射过的计数器中最小的值。