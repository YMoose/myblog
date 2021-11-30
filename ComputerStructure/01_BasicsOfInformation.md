# Basics of Information
## Quartifying Information 量化信息
根据香农的信息论(Claude Shannon, 1948)中提出了如下量化信息的方法：  
给定一个不连续的随机变量X，其
- 有N种可能的值: x1,x2,x3...
- 各种值发生可能性: p1,p2,p3...

则要表示一个确定的xi需要的信息量I(xi)的值可以用如下公式得出  
$$I(x_i) = log_2(\frac{1}{p_i}) $$
信息的熵(Entropy)是在对于给定的变量范围X内，平均每个变量包含的信息量：
$$H(X) = E(I(X)) = \sum_{i=1}^{N} p_i * log_2(\frac{1}{p_i})$$
可以通过信息的熵来查看我们对整体信息使用的信息量是否合理，当小于熵时就说明信息量不够可能会丢失信息，当大于熵时就说明有信息冗余。  
# Error Detection and Correction 错误检测与修正

