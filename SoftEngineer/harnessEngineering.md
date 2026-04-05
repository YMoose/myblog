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

## martinfowler 

https://martinfowler.com/articles/harness-engineering.html

