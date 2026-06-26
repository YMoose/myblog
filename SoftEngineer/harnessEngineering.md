# harness

## openAi

https://openai.com/index/harness-engineering/ 

Redefining the role of the engineer
The lack of hands-on human coding introduced a different kind of engineering work, focused on systems, scaffolding, and leverage.

Early progress was slower than we expected, not because Codex was incapable, but because the **environment was underspecified.** The agent lacked the tools, abstractions, and internal structure required to make progress toward high-level goals. The primary job of our engineering team became enabling the agents to do useful work.
agent运行环境是指什么？感觉这个说法很大，但可以肯定的是逐步搭建出来的。claude.md 还是 skill的架构？
tools 我理解，abstractions, and internal structur e是什么？

In practice, this meant working depth-first: breaking down larger goals into smaller building blocks (design, code, review, test, etc), prompting the agent to construct those blocks, and using them to unlock more complex tasks. When something failed, the fix was almost never “try harder.” Because the only way to make progress was to get Codex to do the work, human engineers always stepped into the task and asked: “what capability is missing, and how do we make it both legible and enforceable for the agent?”
首先大拆小，要有个大致的架构，在这个大致架构下深度优先搜索，深入到某一步可以完成所谓的Ralph Wiggum Loop的时候开始使用循环广度优先搜索来逐步完善基础组件
失败时考虑“究竟还需要什么样的能力，我们又该如何让这个能力对智能体来说既清晰可读又可强制执行？”

Humans interact with the system almost entirely through prompts: an engineer describes a task, runs the agent, and allows it to open a pull request. To drive a PR to completion, we instruct Codex to review its own changes locally, request additional specific agent reviews both locally and in the cloud, respond to any human or agent given feedback, and iterate in a loop until all agent reviewers are satisfied ,(effectively this is a [Ralph Wiggum Loop](https://ghuntley.com/loop/) ). Codex uses our standard development tools directly (gh, local scripts, and repository-embedded skills) to gather context without humans copying and pasting into the CLI.
收集信息（认知）（不改变外部状态）->探索实现（实践）（改变外部状态），这里是不是可以将工具（甚至agent）按照这个进行分类
这里其实应该是，agent干一个事情，然后他发现他这个任务缺少一个下层组件或前置任务时，把他自己当前的任务先suspend，然后把前置任务挂到任务列表中，然后这个前置任务应该会出发这个他自己当前的任务。

Increasing application legibility
As code throughput increased, our bottleneck became human QA capacity. Because the fixed constraint has been human time and attention, we’ve worked to add more capabilities to the agent by making things like the application UI, logs, and app metrics themselves directly legible to Codex.
优化系统时，主要时优化瓶颈部分

这一段有些实践操作没看懂，要回来看看

We made repository knowledge the system of record

Context management is one of the biggest challenges in making agents effective at large and complex tasks. One of the earliest lessons we learned was simple: give Codex a map, not a 1,000-page instruction manual.
记忆系统，渐进式披露，老生常谈了

We tried the “one big AGENTS.md” approach. It failed in predictable ways:

- Context is a scarce resource. A giant instruction file crowds out the task, the code, and the relevant docs—so the agent either misses key constraints or starts optimizing for the wrong ones.
- 约束是最高优先级的，丢掉约束就会出问题，也就是说上下文有一部分是天然要交给约束的，剩下那部分才是真正的上下文长度，同时上下文中的注意力机制导致上下文本身就是虚标的。
- Too much guidance becomes non-guidance. When everything is “important,” nothing is. Agents end up pattern-matching locally instead of navigating intentionally.
- 有指导/约束的同时又不能太多（真·领导的艺术，懂得放权，还是要根据任务的具体要求来看）
- It rots instantly. A monolithic manual turns into a graveyard of stale rules. Agents can’t tell what’s still true, humans stop maintaining it, and the file quietly becomes an attractive nuisance.
- 文档太大超出上下文（包括人类的上下文），就会难以维护，丢失一致性。所以要分层，指导/约束同样也要分层。
- It’s hard to verify. A single blob doesn’t lend itself to mechanical checks (coverage, freshness, ownership, cross-links), so drift is inevitable.

Design documentation is catalogued and indexed, including verification status and a set of core beliefs that define agent-first operating principles. ⁠[Architecture documentation](https://matklad.github.io/2021/02/06/ARCHITECTURE.md.html) provides a top-level map of domains and package layering. A quality document grades each product domain and architectural layer, tracking gaps over time.
他们代码库中 QUALITY_SCORE.md，专门给 AI Agent（Codex） 看的质量评分手册。这个了解一下，就像我要对claude.md优化一样，其实要不断优化所有的问题，如果使用代码有问题要优化代码，那么使用文档有问题也要优化文档，文档也应该又类似的clang-tidy/clang-format的规则文件/criteria，且不同目的的文档应该又不同的标准。

Plans are treated as first-class artifacts. Ephemeral lightweight plans are used for small changes, while complex work is captured in [execution plans](https://developers.openai.com/cookbook/articles/codex_exec_plans) with progress and decision logs that are checked into the repository. Active plans, completed plans, and known technical debt are all versioned and co-located, allowing agents to operate without relying on external context.
看看他们是如何实践这个plan的。plan中写之前的设计决策也要包含在plan中，执行中的问题也要有地方记录，要能trace plan。

This enables progressive disclosure: agents start with a small, stable entry point and are taught where to look next, rather than being overwhelmed up front.

We enforce this mechanically. Dedicated linters and CI jobs validate that the knowledge base is up to date, cross-linked, and structured correctly. A recurring “doc-gardening” agent scans for stale or obsolete documentation that does not reflect the real code behavior and opens fix-up pull requests.
太像skill-creator了

Agent legibility is the goal

This framing clarified many tradeoffs. We favored dependencies and abstractions that could be fully internalized and reasoned about in-repo. Technologies often described as “boring” tend to be easier for agents to model due to composability, api stability, and representation in the training set. In some cases, it was cheaper to have the agent reimplement subsets of functionality than to work around opaque upstream behavior from public libraries. For example, rather than pulling in a generic p-limit-style package, we implemented our own map-with-concurrency helper: it’s tightly integrated with our OpenTelemetry instrumentation, has 100% test coverage, and behaves exactly the way our runtime expects.
选用可完全内化的、可在仓库中进行推理的依赖项、第三方库和抽象。在轮子使用起来太复杂的情况下，可以按需造轮子。

Enforcing architecture and taste

Documentation alone doesn’t keep a fully agent-generated codebase coherent. By enforcing invariants, not micromanaging implementations, we let agents ship fast without undermining the foundation. For example, we require Codex to [parse data shapes at the boundary](https://lexi-lambda.github.io/blog/2019/11/05/parse-don-t-validate/), but are not prescriptive on how that happens (the model seems to like Zod, but we didn’t specify that specific library).
在一些重要节点（里程碑管理）有一些强制约束（而且在这些节点可以回退，这和搜索也有点像，是个分叉节点），比如linter工具&文档检查工具&代码测试全部完成检查
todo 文章看一下

Agents are most effective in environments with strict [boundaries and predictable structure](https://bits.logic.inc/p/ai-is-forcing-us-to-write-good-code), so we built the application around a rigid architectural model. Each business domain is divided into a fixed set of layers, with strictly validated dependency directions and a limited set of permissible edges. These constraints are enforced mechanically via custom linters (Codex-generated, of course!) and structural tests.
架构设计发挥了更重要的作用，其中模块的依赖方向也要稳定。
todo 文章看一下

This is the kind of architecture you usually postpone until you have hundreds of engineers. With coding agents, it’s an early prerequisite: the constraints are what allows speed without decay or architectural drift.
agent时代下的架构设计需要花费更多心思，但一样的，应该结合反馈循环、里程碑逐步丰富固化才对。这应该有个更具体的方法论todo!

In practice, we enforce these rules with custom linters and structural tests, plus a small set of **“taste invariants.”** For example, we statically enforce structured logging, naming conventions for schemas and types, file size limits, and platform-specific reliability requirements with custom lints. Because the lints are custom, we write the error messages to inject remediation instructions into agent context.
品味不变式，好词。深入了解一下不变式。
另一方面在设计检查工具时，要暴露具体错误，提供修复指南。这是一种反馈，可以用户优化系统

In a human-first workflow, these rules might feel pedantic or constraining. With agents, they become multipliers: once encoded, they apply everywhere at once.
确实，思路打开了

At the same time, we’re explicit about where constraints matter and where they do not. This resembles leading a large engineering platform organization: enforce boundaries centrally, allow autonomy locally. You care deeply about boundaries, correctness, and reproducibility. Within those boundaries, you allow teams—or agents—significant freedom in how solutions are expressed.
领导的艺术，联系起来了。boundaries, correctness, and reproducibility 这几个概念深化一下。有边界的自由

The resulting code does not always match human stylistic preferences, and that’s okay. As long as the output is correct, maintainable, and legible to future agent runs, it meets the bar.
有边界的自由，这个边界不是以人类为标准，（可以以人类标准为参考）

Human taste is fed back into the system continuously. Review comments, refactoring pull requests, and user-facing bugs are captured as documentation updates or encoded directly into tooling. When documentation falls short, we promote the rule into code
品味->规则->代码 逐步固化

Throughput changes the merge philosophy

As Codex’s throughput increased, many conventional engineering norms became counterproductive.

The repository operates with minimal blocking merge gates. Pull requests are short-lived. Test flakes are often addressed with follow-up runs rather than blocking progress indefinitely. In a system where agent throughput far exceeds human attention, corrections are cheap, and waiting is expensive.

This would be irresponsible in a low-throughput environment. Here, it’s often the right tradeoff.

举个例子，正常公司的 CI 可能有一堆拦路检查：
1. 代码风格检查必须过
2. 所有单元测试必须 100% 通过
3. 覆盖率必须达标
4. 必须人工审核，点赞
5. 必须安全扫描
6. 必须性能测试
一个挂了 → PR 直接卡住，不让合入，这叫 heavy blocking merge gates（重门禁）。
OpenAI 的做法：minimal blocking merge gates
在一些非里程碑的pr中，只拦真正致命、会破坏系统根基的东西，其他都不拦：
1. 必须 blocking（绝对拦死）—— 也就是你的 invariants
2. 编译不过
3. 架构分层违规
4. 安全规则破坏
5. 会导致程序直接崩溃的错误
不 blocking（不拦，放行）
1. 格式小问题
2. 非核心测试偶尔挂了
4. 某些覆盖率不达标
4. 可后续优化的小瑕疵
5. 不稳定测试 flaky test
这是对约束分层，约束的复杂度也进一步提升了。

What “agent-generated” actually means

When we say the codebase is generated by Codex agents, we mean everything in the codebase.

Agents produce:

- Product code and tests
- CI configuration and release tooling
- Internal developer tools
- Documentation and design history
- Evaluation harnesses
- Review comments and responses
- Scripts that manage the repository itself
- Production dashboard definition files

Humans always remain in the loop, but work at a different layer of abstraction than we used to. We prioritize work, translate user feedback into acceptance criteria, and validate outcomes. When the agent struggles, we treat it as a signal: identify what is missing—tools, guardrails, documentation—and feed it back into the repository, always by having Codex itself write the fix.

Agents use our standard development tools directly. They pull review feedback, respond inline, push updates, and often squash and merge their own pull requests.

从工作流中的实施部分抽身，到更高层次的维度。主要就是从关注细节到关注更宏观层次的。这是一个逐步替换的过程，不是一蹴而就的。这就说明，生长通道没有关闭。ai在每一步都有帮助。

Entropy and garbage collection

Full agent autonomy also introduces novel problems. Codex replicates patterns that already exist in the repository—even uneven or suboptimal ones. Over time, this inevitably leads to drift.

Initially, humans addressed this manually. Our team used to spend every Friday (20% of the week) cleaning up “AI slop.” Unsurprisingly, that didn’t scale.

Instead, we started encoding what we call “golden principles” directly into the repository and built a recurring cleanup process. These principles are opinionated, mechanical rules that keep the codebase legible and consistent for future agent runs. For example: (1) we prefer shared utility packages over hand-rolled helpers to keep invariants centralized, and (2) we don’t probe data “YOLO-style”—we validate boundaries or rely on typed SDKs so the agent can’t accidentally build on guessed shapes. On a regular cadence, we have a set of background Codex tasks that scan for deviations, update quality grades, and open targeted refactoring pull requests. Most of these can be reviewed in under a minute and automerged.

This functions like garbage collection. Technical debt is like a high-interest loan: it’s almost always better to pay it down continuously in small increments than to let it compound and tackle it in painful bursts. Human taste is captured once, then enforced continuously on every line of code. This also lets us catch and resolve bad patterns on a daily basis, rather than letting them spread in the code base for days or weeks.

agent开发时同样有一些design pattern，这些design pattern 用于维护项目复杂度

What we’re still learning
This strategy has so far worked well up through internal launch and adoption at OpenAI. Building a real product for real users helped anchor our investments in reality and guide us towards long-term maintainability.

What we don’t yet know is how architectural coherence evolves over years in a fully agent-generated system. We’re still learning where human judgment adds the most leverage and how to encode that judgment so it compounds. We also don’t know how this system will evolve as models continue to become more capable over time.

What’s become clear: building software still demands discipline, but the discipline shows up more in the scaffolding rather than the code. The tooling, abstractions, and feedback loops that keep the codebase coherent are increasingly important.

Our most difficult challenges now center on designing environments, feedback loops, and control systems that help agents accomplish our goal: build and maintain complex, reliable software at scale.

人们应该控制的重点确实已经改变了，已经慢慢从底层工作慢慢上移。这里上面简要说明了一下，他觉得重要的部分

As agents like Codex take on larger portions of the software lifecycle, these questions will matter even more. We hope that sharing some early lessons helps you reason about where to invest your effort so you can just build things.

## atlassian work management
https://www.atlassian.com/work-management

### strategic planning
It involves setting priorities and deciding how resources will be allocated to support that overall vision. 

It’s easy to confuse strategic planning with project planning, but the two are quite different. Project planning is important for singular tasks or assignments that your team is completing (how you will move that project from inception to completion).

Strategic planning is broader. It pulls all individual projects into a cohesive strategy that supports the company’s overarching goal.
这部分还太高屋建瓴了，暂时还不需要

### project management
Project management ensures that a team's work aligns with [SMART goals](https://www.atlassian.com/blog/productivity/how-to-write-smart-goals) and meets the criteria for success within given constraints. 

The core phases of project management include: 

- Planning: In the first stage, you'll define project scope, objectives, and steps to achieve them. This stage involves creating a detailed roadmap, planning resources, and identifying potential risks. 
- Executing: This is where you put the plan into action by coordinating people and resources, managing stakeholder expectations, and overseeing the completion of tasks. 
- Closing: This is the formal ending of the project. In this stage, you'll evaluate the project's success, document lessons learned, and release resources.
- Monitoring: Monitoring involves tracking progress, measuring performance against key indicators, and adjusting as needed to keep the project on track. 
Monitoring 可以说是上面三步进行中的并行的工作，并不是单独的一个phase

### 敏捷开发
https://www.atlassian.com/agile

``` mermaid
graph TD
    A[Agile 敏捷<br/>价值观 & 原则]
    B[Lean 精益<br/>源自丰田<br/>消除浪费]

    %% 敏捷体系
    A --> A1[Scrum<br/>项目管理框架]
    A --> A2[XP 极限编程<br/>技术实践]

    %% 精益体系
    B --> B1[Kanban 看板<br/>可视化流动工具]
```

``` mermaid
graph TD
    C[实际项目常用搭配]

    C --> C1[Scrum + Kanban = Scrumban]
    C --> C2[Scrum + XP]
    C --> C3[Lean + Kanban]
    C --> C4[Scrum + Lean + Kanban 全混合]
```

## milestone management

milestone就是checkpoint或者关键事件。使团队更容易评估项目是否按计划推进，并让利益相关者在不陷入任务细节的情况下审查进度。

里程碑制定——过程动态跟进——上线前收尾确认

一些重要的事情发生并可以被签署为完成（确定里程碑交付物交付）时，它的记号或标识才被认为是里程碑。里程碑只有“100%完成”和“无法完成”两个状态——不存在部分完成的情况。（这样可以将一些无法量化的东西量化为简单的完成，未完成）

里程碑记录在一个进度计划的列表中及甘特图

项目管理术语要避免混淆
通常情况下，项目里程碑会与其他项目管理术语混淆，下面是一些常见的混淆情况，如果您想创建一个任务、里程碑或目标时，请记住他们：

1. 里程碑与目标：项目目标是您试图通过您的项目实现的总体目标。而里程碑只是具体的检查点，可以帮助您跟踪项目的进展，确保您可以实现项目目标。
2. 里程碑与项目交付物:交付物是项目过程中产生的具体成果或结果。这些包括“最终报告”、“已完成的营销活动”或“软件版本发布”等。交付物反映了已完成的工作，而里程碑则表明已到达某个特定节点。可以将里程碑视为进度指示器，而交付物则是该进度的产出。例如，“原型交付”如果标志着项目时间线中的重大转折点，那么它既可以是交付物，也可以是里程碑。理解这种关系有助于在规划时清晰地定义项目里程碑和交付物的示例。在网站开发项目中，里程碑可能包括“设计审批”、“内容上传完成”和“客户审查”，而可交付成果可能是“上线网站”、“样式指南”或“发布资源”。它们共同确保过程和结果都能被高效跟踪。
3. 里程碑与任务:任务是两个里程碑节点上的线段

如何制定里程碑的参考
有可行性，有验收价值
项目预算：根据项目预算，确保您的里程碑时间节点是现实的。如果不现实，您需要调整您的项目规划表或寻找额外的项目资金；
项目进度：确保您的里程碑时符合项目进度表的。如果不符合，您需要调整您的里程碑或项目进度表；
项目范围：确保您的里程碑是在项目范围内可实现的。如果无法实现，您需要调整里程碑或项目范围。

重点关注有依赖的节点
进行阶段性复盘。里程碑完成了，好的方面是什么；里程碑完成可能有风险，着重复盘偏差的原因，再针对性地去解决。

每个项目都包含多个阶段，可以通过项目管理中的里程碑进行跟踪。这些项目管理里程碑充当参考点，以确保项目按照计划推进。识别项目中的关键里程碑有助于从管理者到利益相关者的每个人了解团队距离完成目标的进度。
以下是一些适用于各行业和项目类型的常见项目里程碑示例。
启动里程碑：这些标志着项目的正式开始。示例包括项目审批、资源分配或预算确认。在此阶段，团队会定义项目里程碑，以构建后续的时间表。达到这一第一个检查点，确认项目可行并已准备好进入规划阶段。
规划里程碑：这些侧重于制定时间表、识别依赖关系以及评估风险。规划里程碑通常包括“风险评估完成”或“时间表最终确定”。通过设定明确的规划里程碑，团队可以确保在执行开始前准确跟踪进度。
执行里程碑：这些代表在主要工作阶段中的实际进展。示例包括设计评审、原型交付或MVP发布。在此阶段跟踪项目里程碑可确保每个交付成果与预先设定的目标和质量基准保持一致。例如，完成“beta版本发布审批”表明开发进度正常。
测试里程碑：每个项目都应包含一个验证成果的阶段。测试里程碑可能包括“QA测试完成”或“用户验收测试（UAT）已通过”。这些检查点在发布前验证交付成果是否符合功能和质量预期。
收尾里程碑：这些标志着项目交付的最后步骤。收尾里程碑包括客户验收、文档定稿以及项目后评审。达到这些项目管理里程碑意味着项目已成功完成，并使团队能够反思绩效和总结经验。

避免里程碑过载：过多的检查点会分散注意力。只优先考虑真正代表进展的关键项目里程碑。

了解什么是项目里程碑
只是过程的一部分。下一步是学习如何定义可实现、可衡量并与整体目标战略一致的项目里程碑。有效的里程碑规划可确保更顺畅的跟踪、更好的协作以及按时交付。
以下是设置真正有效的项目里程碑模板和检查点的方法。
将项目分解为逻辑阶段： 将项目划分为明确的阶段，例如启动、规划、执行和收尾。每个阶段都应包含反映重要转变的关键项目里程碑。这种方法简化了项目里程碑的跟踪，并能在每个阶段进行清晰的汇报。
识别任务之间的关键依赖关系： 里程碑通常依赖于其他任务先完成。在规划时，识别这些依赖关系，以便有效管理延误。例如，在项目管理的里程碑中，“设计审批”必须在“开发开始”之前。可视化依赖关系有助于防止后续出现瓶颈。
为里程碑检查点分配负责人：问责制对于推进工作至关重要。将里程碑的所有权分配给特定团队成员或部门。这确保了跟踪项目里程碑成为一种共同责任，从而促进团队内部的透明度。
估算切实可行的完成日期：设定过于激进的日期可能会导致压力和不一致。使用过去的数据或历史项目里程碑示例来预测可实现的时间表。切实可行的截止日期有助于保持一致性，并使团队能够有效规划资源。
使用工具可视化里程碑：甘特图、日历或项目里程碑模板可以让时间线更易于跟进。能够直观展示项目里程碑的工具有助于利益相关者即时了解进展。像 Lark 这样的数字化工作空间，可以以动态方式可视化项目的关键里程碑，并实时更新。
在项目发展过程中进行审查和调整：项目的范围、时间表或资源经常会发生变化。定期重新审视你的里程碑清单可确保其保持相关性。动态跟踪项目里程碑可确保依赖关系或交付物的变化能够即时反映。
为不同类型的项目（如产品发布或软件开发）建立项目里程碑模板，有助于团队在各项目和部门之间标准化成功跟踪。
