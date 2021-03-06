# 细化阶段
大多数需求分析，并伴有具有产品品质的早期编程和测试。  
现代软件分工后的结果是——框架(library&framwork)和应用程序的划分。
## 面向对象设计原则
1. 依赖倒置（置换）原则(DIP)
   - （稳定的）高层模块不应该依赖于（变化的）低层模块，二者都应该依赖于（稳定的）抽象：可以隔离变化
   - （稳定的）抽象不应该依赖于（变化的）实现细节，（变化的）实现细节应该依赖于（稳定的）抽象
   - 变量不可以持有具体类的引用
   - 不要让类派生自具体类
   - 不要覆盖基类中已经实现的方法
2. 开放封闭原则(OCP)
   - 对扩展开放，对更改封闭
   - 类模块应该是可扩展的，但不可修改
3. 单一职责原则(SRP)
   - 一个类应该仅有一个引起它变化的原因
   - 变化的方向隐含着类的职责
4. Liskov替换原则(LSP)
   - 子类必须能够替换他们的基类（子类 IS A 基类）
   - 继承表达类型抽象
5. 接口隔离原则(ISP)
   - 不应该强迫客户程序依赖他们不用的方法
   - 接口应该小而完备
6. 优先使用对象组合而不是类继承
   - 类继承通常为“白箱复用”，对象组合通常为“黑箱复用”
   - 继承在某种程度上破坏了封装性，子类父类耦合度高，而对象组合则之要求被组合对象具有良好定义的接口，耦合度低
7. 封装变化点（封装分界层次）
   - 使用封装来创建对象间的分界层，让设计者可以在分界的一侧进行修改而不对另一测产生不良的影响，从而降低实现层次间的耦合
8. 针对接口编程，而不是针对实现编程
   - 不将变量类型声明为某个特定的具体类，而是声明为某个接口
   - 客户程序无需获知对象的具体类型，只需知道对象所拥有的接口。这样可以减少系统中各部分的依赖关系，从而实现”高内聚，松耦合“的类型设计方案
## GoF-23 设计模式的分类
- 从目的来看：
  1. 创建型(Creational)模式：对象的创建时需要
  2. 结构型(Structural)模式：对象在应对变化的需求时需要
  3. 行为型(Behavioral)模式：多个类之间责任划分时需要
- 从范围（扩展性）（实现手段）来看：
  1. 类模式：处理类与子类的静态关系
  2. 对象模式：处理对象间的动态关系
- 李建忠老师的分类方法：
   |类别|解决的问题|典型模式|
   |-|-|-|
   |组件协作|通过晚期绑定，实现松耦合以解决协作问题|Template Method<br>Strategy<br>Observer/Event|
   |单一职责|子类急剧膨胀，需要有责任划分，使对象类有清晰的（最好是单一）职责|Decorator<br>Bridge|
   |对象创造|通过此类模式，绕开new，解决对象创建过程中的依赖关系，从而支持对象创建的稳定。是接口实现的第一步|Factory Method<br>Abstract Factory<br>Prototype<br>Builder|
   |对象性能|处理面对对象带来的成本|Singleton<br>Flyweight|
   |接口隔离|通过添加一层（稳定的）间接层接口来隔离紧耦合的接口|Facade<br>Proxy<br>Adapter<br>Mediator|
   |状态变化|||
   |数据结构|||
   |行为变化|解耦组件行为的变化导致组件本身的剧烈变化|Command<br>Visitor|
   |领域问题|||
## 设计模式的要点
设计模式是针对需求的频繁变化而存在的  
所以其主要任务就是寻找变化点，在变化点处应用设计模式（将变化与稳定分离）  
### 重构 Refactoring to Patterns
利用重构迭代优化对象设计  
重构的技巧：
1. 静态 -> 动态
2. 早绑定 -> 晚绑定
3. 继承 -> 组合
4. 编译时依赖 -> 运行时依赖
5. 紧耦合 -> 松耦合