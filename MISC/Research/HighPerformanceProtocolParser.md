1. Zhichun Li, Gao Xia, Hongyu Gao, Yi Tang, Yan Chen, Bin Liu, NetShield: Matching with a Large Vulnerability Signature Ruleset for High Performance Network Defense
这篇文章转换了以往采用正则匹配作为规则匹配的思路，转而使用了Vulnerablility Signature作为识别恶意流量的标准。Vulnerabilitiy Signature 是一系列更具协议语义信息预定义的符号，为此需要根据协议定义进行解析获取协议字段和协议状态机获取协议解析状态。因为Vulnerability Signature包含了正则匹配没有的semantic context，所以更为准确（事实上我们目前的规则结合两种方式的，正则表达式之外还补充了协议语义相关的内容（所以也就需要更深入的解析了））。文章采用了一个UltraPAC的解析器自动生成器。以往采用Vulnerablility Signature的识别引擎存在速率不高的问题，本文采用

2. K. Zhang, J. Wang, B. Hua and X. Tang, "Building High-Performance Application Protocol Parsers on Multi-core Architectures," 2011 IEEE 17th International Conference on Parallel and Distributed Systems, 2011, pp. 188-195, doi: 10.1109/ICPADS.2011.37.
这篇文章采用描述了一种利用编译前端识别器生成器flex生成协议解析器，并结合一个高性能高并发解析框架解析流量（以http为例子）

3. https://en.wikipedia.org/wiki/Comparison_of_parser_generators parser生成器
