# Claude Code Tips and Tricks 
出于多种原因，我目前多数Vibe Coding实践是和Claude Code深度绑定的。
现在根据Claude的官网和自己使用情况将一些小技巧做一些整理
我在对当前agent（特别是code agent）的使用中有以下基本前提，
1. agent的上下文（记忆）是有限的（ context window）
2. agent是专用的不是万能的
3. agent是会犯错的不是完美的
基于上述前提，我将agent看作是一个远程协作的团队成员。在此基础上，我们可以利用现代的科学的管理、工程等知识来更好的管理团队以达成目标。

## agent 客制化
agent的角色是你赋予的。在前期可能团队成员可能身兼多职，但人员分工一定会随着项目实践变得更加清晰。这个角色分工的定义方式在不同的agent实现上有不同的方法（在Claude code是用`CLAUDE.md`实现，别的比如`AGENTS.md`）。

我理解原理上应该是the context the agent have decide who agent are。todo参考学者对记忆的研究，记忆会分为长期记忆和短期记忆。可以将这种角色分工的定义定位为项目相关的长期的工作记忆。

`CLAUDE.md`存放在不同的目录下有不同的作用，可根据角色分工的细化程度决定。项目根目录，则和项目绑定，可以将`CLAUDE.md`纳入项目仓库，让共同开发的同事也从中获得有用的信息,`CLAUDE.local.md`也和项目绑定但是和个人相关的，不上传项目仓库；`~/.claude/CLAUDE.md`则和操作系统用户绑定，适用于当前用户的全部claude agent。

项目仓库的`CLAUDE.md`推荐存放以下信息：
1. 核心文件和实用函数简介（简要设计文档）
2. 项目会常使用到的bash命令
3. 代码风格指南
4. 测试说明（测试方法及原则）
5. 仓库管理规范（例如，分支命名、合并与变基等）
6. 开发环境的相关设置信息（例如，如何使用pyenv，适用的编译器路径）
7. 项目特有的任何意外行为或警告信息（对错误的描述，避免agent浪费资源解决此类错误）

根据[参考2](#参考-2)的观察，`CLAUDE.md`文件应该保证较小的规模（毕竟我们将其视为长期记忆），而不是塞入所有的信息。这个规模其在当下，[参考2](#参考-2)提到这个规模是对agent的指示应该小于等于50条，文件行数应该在60行以内，行业普遍共识是300行（我感觉也应该是短小精悍，特别是越上级的越概括）。

所以，**`CLAUDE.md`是需要跟随项目实践进度迭代**的（纳入git管理的实践会很不错）。为保证其规模，我们需要将信息整理归纳，将信息整理到新的文档文件中，并将文档文件的内容压缩归纳，在`CLAUDE.md`建立索引。

> 在claude code中使用`#`可以将后续输入的重要内容加入到上下文中（一种主动的上下文管理手段）。也可以按两次`Esc`让claude code 的上下文回到之前某一个点。还可以使用`/compact`命令主动压缩上下文

> claude code agent在实际运行中可能会忽视`CLAUDE.md`，有时可以主动提醒agent注意`CLAUDE.md`中的信息。
> 不要用agent去做代码风格检查和修改，会占用agent有限且珍贵的上下文
## 拓展 agent 能力
agent 是可以通过补充工具来拓展其能力的。工具有多种集成的形式，可以根据其用途，以合适的形式集成。
### sh commands
shell命令是多数agent集成的工具。对于常见的工具和命令agent会根据需求自动调用并分析结果。同时也可以根据项目需求将项目常用的工具和命令的用法记录用`CLAUDE.md`记录管理，让agent了解其用法。甚至可以针对项目开发小型工具（事实上使用中，会发现agent确实会在执行时就地编写python脚本直接调用已完成某些工作）以进一步补充。
使用场景广泛。
优点是灵活，针对性高，且可以组合多个达成不错的效果。
缺点是需要对自定义工具的用法有比较实用的描述，让agent使用此工具也需要调教。另一方面有一点门槛，需要有一定的开发能力。
### MCP(Model Context Protocol)
多数agent也已实现了MCP的集成。我觉得MCP是一种相对成熟的特定领域的工具，举一些例子
1. [playwright mcp](https://github.com/microsoft/playwright-mcp):和浏览器交互
2. [MCP Toolbox for Databases](https://googleapis.github.io/genai-toolbox/getting-started/introduction/):和数据库交互
使用场景和领域相关。
优点是功能强大，部分agent已经做了不错的mcp协议支持，可以使得原本较为复杂的任务，变得简单。
缺点是领域相关，如果有需要但市面上没有，需要自己开发一个，相对sh commands开发的门槛就高一些了。
### Skills
使用场景也挺广泛，因为是工作流的抽象。
### 权限管理
[参考1](#参考-1)1.c Curate Claude's list of allowed tools
## 迭代 workflow
## 产能扩张
tmux
## 参考
1. [Claude Code: Best practices for agentic coding](https://www.anthropic.com/engineering/claude-code-best-practices)
2. [Writing a good CLAUDE.md](https://www.humanlayer.dev/blog/writing-a-good-claude-md)