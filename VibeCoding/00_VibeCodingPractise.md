# Vibe Coding Practise

## 软件工程
软件系统有实现上和业务上的复杂度，这两方面处理决定了软件系统的设计水平和代码质量。对于此可以有一个假设性的评价方法，即是否可以让新参与的开发人员在短时间内理解系统的业务逻辑并快速实现新的功能，同时系统不至于劣化。《软件设计的哲学》书中提到Facebook鼓励刚大学毕业的新工程师立即深入公司的代码库并提交代码且几乎没有任何规则和限制，这使得其功能得以快速实现，但随着系统复杂度提高，反而使得系统变得脆弱。google则使用了大量的测试来保证系统基本功能的下限，至少在实现新功能时不至于破坏现有的功能。
我在看乐群老师的文章时，提到了其在google的实习期间的一些工程实践，我关注到的点包括有
1. 完成代码前的设计文档：google在开始写代码之前的两周，会要求先写一份设计文档(Design Docs)。设计文档大概介绍一下打算新加的这个产品或者新功能的动机是什么，提供了什么功能，系统是怎么设计的，需要修改什么已有的接口，提供什么样新的接口，小细节上面有什么复杂的点，以及不同的替代方案的优缺点。
2. 代码注释就地性：每个头文件的最前面都有很长篇幅的代码注释用来介绍这个类(Class)是用来做什么的，有的甚至还包含了示例。每个公有方法(Public Method)也都有详细的注释。大部分的库都没有专门的文档，文档就是写在头文件里面。配合代码搜索使用其实非常舒服。
3. 代码覆盖率指导测试(Code Coverage)：在不知道要写什么单元测试的时候，看一眼代码覆盖可以找到那些没被测试到的情况。不过代码覆盖只能起到辅助作用，哪怕做到了100%覆盖也不一定代表程序是正确的，因为可能错误是在更高层次的地方。

## AI Code Agent
我是看了参考4的文章（写得很不错，本文很大程度源于这篇文章），开始了Vibe coding的尝试。参考了文中使用的Vibe Coding方式：使用[spec-kit](https://github.com/github/spec-kit)来驱动[claude-code](https://github.com/anthropics/claude-code)，然后我使用的底层大模型服务用的是[智谱](https://www.bigmodel.cn/claude-code?ic=XBYB8SR4V9)的。
可以说现在的从大模型到AI Agent，其工作能力已经有了长足的进步，之前我使用的时候还常常给出无法运行的代码。就像参考4文中提到的AI可能降低效率的思考，这里我觉得有个平衡点，就是假设AI辅助生成的代码准确率是80%，但解决这剩下的20%的错误，需要花开发人员原本实现的还要久的时间，或者差不多的时间，那开发人员就不会选择用AI辅助来辅助工作（这有点像性能优化中优化效益与成本的tradeoff）。通过将人类学习认知科学方面（控制论+信息论，像是《实践论》，认知作用于实践，实践又反作用于认知，可以看看参考6）的成果通过一些结构化的手段应用到了AI Agent中，我认为当前的AI Agent在多数场景下已经跨过了这个平衡点。
另一方面，开发人员在面对一个需求时，需求本身具有其复杂性，开发人员对其的描述可能想对简单和模糊，并不能完全体现需求本身的复杂性，这一步首先会遗漏一些信息，损失一些精度。而同时ai agent在参考开发人员的描述后，在其实现过程中的多个步骤中又存在多重 ai 幻觉的叠加，我觉得这是vibe coding当前的困境。
也就是说，AI辅助编程已经进入了一个新的发展阶段。当前Vibe Coding的主要矛盾已经从开发人员对高效完成编码工作的需求与开发人员使用AI Agent时代码的低容错率及后续纠错成本过高之间的矛盾转化为了软件系统需求的复杂性和开发人员使用AI Agent时信息损耗最终导致实现错误之间矛盾。
但是！这层矛盾并不是只存在于Vibe Coding中的矛盾，是软件开发历史中一贯始终，普遍存在的。也就是说当前发展阶段的Vibe Coding的这个矛盾是可以用软件工程的一些已有方法来解的。
文章中就提到了[Specification-Driven Development (SDD)](https://github.com/github/spec-kit/blob/main/spec-driven.md)，在以往的软件开发过程中，也有文档方面的要求，但在开发过程中更注重的还是代码。个人感觉有多个因素导致了这样的结果，一方面是开发人员的最终交付物是代码，主要对代码负责，所以更注重代码；一方面是小型项目上，开发人员直接理解需求可能会跳过文档实现部分直接生产代码，代码即文档，大型项目上，则由架构师来产出设计文档，而很大程度上的开发人员理解需求除了文档还有直接和架构师的沟通，这也导致了文档的失位。
``` c
// todo add graph
/*
需求-> 设计文档 -> 代码 -> 产品
*/
```
这也就是文章提到的第二点——角色转换，使用Vibe Coding需要将开发人员从以往的编码实现人员转换为架构师的角色，关注点从具体的实现转换为需求的理解、系统的设计和架构，产出从代码变为文档+代码的组合（并不是放弃代码）。之前提到的跨过平衡点后，开发人员被AI Agent能力解放的时间，就可以用来做和原来编码实现不一样的这些事情。同样的，在进行这些新的需求的理解、系统的设计和架构工作中，AI Agent仍然可以发挥他的能力（spec-kit项目辅助），也仍伴随着准确率的问题，解决这些新工作的新问题将成为新的工作重心。这有点像以前写汇编，高级语言及其编译器出现后大家可能就很少写汇编了，但有些要优化的还是要人工修改汇编，汇编写得多好不再是考察一个程序员能力的核心标准，但懂汇编语言的程序员会比不懂汇编的程序员多一种视角。懂底层代码的AI架构师，也会比不懂的AI架构师多一种视角。

## 具体实践上的一些感悟
角色转换后，将AI Agent看作是上文那个用来评价你软件系统的设计水平和代码质量的那一个（或几个）新员工（可以练习作为管理层的技能）。AI Agent和人相似，当复杂度提升的时候，一样会犯错，需要通过一些手段来控制AI Agent当前工作的复杂度和清晰度，具体来说就是其上下文管理。
1. 从spec出发，像管理代码一样管理文档。spec-kit会有一些模板，AI Agent可以根据模板来生成文档，但需要人工的介入优化文档，在执行SDD的早期过程中尽量避免牵涉代码细节。要像维护代码一样维护文档，注意版本管理、文档文件模块化等，注意这些工作是全生命周期的，需要不断地将需求的变更、系统理解的加深及时补充到合适文档的合适位置中中，这样新员工才可以依据文档快速实现代码。文档的类型（设计文档、代码规范等）和内容越详实，需求到文档的信息损耗越少（甚至可以做一些信息的冗余）。
2. 注重模块化拆分。我自己实验的小项目是一个网站，有简单的前后端分离，但比如提出一些后端代码问题时，AI Agent可能会尝试修改前端代码。结合参考5中提出SSD宪法第一条——Library-First Principle，设计时提前模块化，以单个模块为工作单位，一是可以更好的管理AI Agent的上下文，二是有效控制AI Agent的操作空间，三可以被动得让设计更谨慎。
3. 充分利用不同AI Agent/大模型的能力和限制。和生产力提升和社会化分工一样，要提升AI Agent的生产力，可以让多个不同的AI Agent分工合作完成复杂任务，比如分别完成调研、设计、规划、生成测试与代码、review等工作。不同的工作阶段可以用不同的AI Agent员工，还可以并行进行，可以减少各方面成本（大模型token、时间以及后续人工的工作量），不过也需要花时间和力气去了解AI Agent的能力就是了，可以看一些评测，自己做一些权衡。
4. 注重测试。结合Test-Driven Development（TDD）的思想，可以有效的减少AI Agent以及开发人员自身的实现错误，及时的发现问题，减少后续的维护成本。
5. 及时用git保存修改。AI Agent还是有实现错误的可能，当AI Agent的实现朝你想要的方向进了一步时，及时用版本管理工具保存备份，因为AI Agent的链条长，有可能跑飞，及时报存这样跑飞了还可以回滚重跑。
6. 调试小tip：可以样llm生成大量的日志来调试，后面直接回滚删除。

### speckit 使用
1. `/speckit.constitution`：对agent的人设（对代码、开发的态度）进行一定的约束，比如
```
/speckit.constitution Create principles focused on code quality, testing standards, user experience consistency, and performance requirements
```
就是说agent需要更关注代码质量、可测标准等，也可以加一些其他的描述，比如我喜欢加以TDD作为开发方法。
这个命令修改的是项目.specify目录里的内容，一般是一些文档模板
2. `/speckit.specify`和`/speckit.clarify`：描述项目需求，这一步尽量减少对具体技术栈的描述，描述项目要做到的功能、使用场景、可以对模块进行划分，
```
/speckit.specify Develop Taskify, a team productivity platform. It should allow users to create projects, add team members,
assign tasks, comment and move tasks between boards in Kanban style. In this initial phase for this feature,
let's call it "Create Taskify," let's have multiple users but the users will be declared ahead of time, predefined.
I want five users in two different categories, one product manager and four engineers. Let's create three
different sample projects. Let's have the standard Kanban columns for the status of each task, such as "To Do,"
"In Progress," "In Review," and "Done." There will be no login for this application as this is just the very
first testing thing to ensure that our basic features are set up. For each task in the UI for a task card,
you should be able to change the current status of the task between the different columns in the Kanban work board.
You should be able to leave an unlimited number of comments for a particular card. You should be able to, from that task
card, assign one of the valid users. When you first launch Taskify, it's going to give you a list of the five users to pick
from. There will be no password required. When you click on a user, you go into the main view, which displays the list of
projects. When you click on a project, you open the Kanban board for that project. You're going to see the columns.
You'll be able to drag and drop cards back and forth between different columns. You will see any cards that are
assigned to you, the currently logged in user, in a different color from all the other ones, so you can quickly
see yours. You can edit any comments that you make, but you can't edit comments that other people made. You can
delete any comments that you made, but you can't delete comments anybody else made.
```
这样会生成以下文件
```
specs
  └── 001-create-taskify
      └── spec.md
```
其中就包括对项目需求的描述有使用场景等（这个文档的内容可能产品经理会熟悉一些，todo了解一下）
要review这个文档，修改不符合你期待的地方，这一步也可以使用`/speckit.clarify`命令或者直接让agent帮你完善，也可以直接修改文档。个人喜欢用这个
```
/speckit.clarify 看看还有没有什么没说清楚的或者有矛盾的
/speckit.clarify 看看还有没有什么不重要的可以提前去除
```
claude-code 可以使用这个进一步检查
```
Read the review and acceptance checklist, and check off each item in the checklist if the feature spec meets the criteria. Leave it empty if it does not.
```
3. `/speckit.plan`

## 一点小问题
1. 在windows10下的WSL使用claude-code的时候，有时候他会使用grep命令来搜索代码，这一grep常常给我整个硬盘IO占满，不知道咋解。
2. 管理上下文，如何控制AI Agent的上下文内容，有些时候在沟通的时候会理解错，这个时候如果可以删除一些没用的上下文可能可以帮助理解，具体怎么做呢？(claude好像有/context)
3. 在使用sdd的情况下，如果仅用llm来生成spec可能会有一些安全问题或者特殊场景（这有点像是[知识的诅咒](https://en.wikipedia.org/wiki/Curse_of_knowledge)），这个时候应该有一些模板提供不同类型的项目，应该注意的细节，类似于备忘录。这有点像设计文档和排障手册的区别，排障手册是对系统问题处理经验累积的成果。
## 参考
1. [《软件设计的哲学》](https://cactus-proj.github.io/A-Philosophy-of-Software-Design-zh/)
2. [谷歌实习体验-陈乐群](https://zhuanlan.zhihu.com/p/84367838)
3. [《Google软件工程》](https://www.oreilly.com/library/view/googleruan-jian-gong-cheng/9787519864705/)
4. [两个月重度使用 AI Code Agent：普通一线程序员的思考和感想](https://jt26wzz.com/posts/0013-ai-coding/)
5. [Specification-Driven Development (SDD)](https://github.com/github/spec-kit/blob/main/spec-driven.md)
6. [从ChatGPT到AI Agent，一文讲透 Agent 的底层逻辑](https://mp.weixin.qq.com/s/tewBKHgbyrjxUjAOmkXI7A)