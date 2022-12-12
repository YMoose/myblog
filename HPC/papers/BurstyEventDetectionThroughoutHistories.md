# Bursty Event Detection Throughout Histories
## 创新点
1. 提出了从流量历史数据中找到爆发性事件这方面研究的缺失
2. 提出了定量分析爆发性事件的概念值brustiness及其计算方法
3. 提出了从历史数据中查找一定brustiness值的事件的算法
4. 优化了上述算法
## 基本概念
### 概念解释
![一些基本概念](pics/BEDTH_0.png)

![example](pics/BEDTH_1.png)

结合这张简单的示例图来说

简单地理解就是一条数据流中，对于同一事件e，其出现的次数总数为Cumulative frequency(F(t))，单位事件内出现的次数为Burst frequency(f(t))，两者的关系很好理解，对Cumulative frequency求导即为Burst frequency(b(t))。进一步来说，文章定义对Burst frequency 求导所得的值就是Burstiness。这个值可以体现事件是否在短时间内大量爆发（即在数据流中的出现频率突增）。但是对于数据流中对应的时间戳是离散的。要在此定义一个单位时间区间$\tau$进行统计计算。因此，F(t)图像在应用场景中将是一个单调增长的阶梯线。

通常我们会需要得出以下三个数据
1. Point query $q(e,t,\tau)$: 在时间点t上事件e的burstiness值
2. Bursty time query $q(e,\theta, \tau)$: e的burstiness值大于阈值$\theta$的所有时间戳集合
3. Bursty event query $q(t,\theta, \tau)$: 多事件流场景下，burstiness值大于阈值$\theta$的所有事件e 

### 算法与数据结构
为得出上述数据，很简单只要得到事件b(t)在t各个时刻的值（即b(t)函数的图像）即可，可通过以下步骤达成
1. 按照时间戳存储事件流数据，或者进一步来说，存储F(t)在t各个时刻的值（即F(t)图像）
2. 根据存储的F(t)，可以根据下列公式很容易地计算得出b(t)
$$
\begin{aligned}
b(t) & =b f(t)-b f(t-\tau) \\
& =f(t-\tau, t)-f(t-\tau, t-2 \tau) \\
& =(F(t)-F(t-\tau))-(F(t-\tau)-F(t-2 \tau)) \\
& =F(t)-2 F(t-\tau)+F(t-2 \tau)
\end{aligned}
$$

最基本的方案是
1. 按照时间戳全量存储整条事件流，空间占用为O(n)
2. 对于Point query的时间复杂度为O(log n);没预先计算和存储Burstiness值的情况下Bursty time query的时间复杂度为O(n)，预计算的情况下为O(log n);bursty event query的时间复杂度为O(log n)

从实际应用来看，基本方案会随着时间增长导致事件流变得异常庞大，导致查询效率低下，最终使得基本方案维护及使用成本极大。但是往往在实际应用中，可以接受一定的误差以获取一个近似值，这让我们有机会拿可接受的误差换取查询效率。接下来就是相应的优化压缩数据的方案

#### 单事件流
先以单事件流为例
##### 方案一
方案一通过利用F(t)阶梯线的性质，从中丢弃部分数据点，选用最具代表性的数据点来实现数据压缩。
###### 算法实现原理
假设压缩后的近似F(t)的函数$\widehat{F}(t) \leq F(t)$，存在一个近似函数与原函数的差值积分$\Delta=\int_0^T(F(t)-\widehat{F}(t))$，当这个差值积分最小的时候我们可以认定此时近似函数$\widehat{F}(t)$最接近F(t)，最能体现F(t)原有的数据。

根据上述假设，
###### 优劣势分析
##### 方案二
方案二通过算法将阶梯线近似为多段分段函数来实现数据压缩
###### 算法实现原理
###### 优劣势分析
#### 多事件流  