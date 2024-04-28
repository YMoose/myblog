# 日志
日志属于可观测性的信息来源之一
按照从功能来说，可分为诊断日志、统计日志、审计日志。

- 错误诊断日志：帮助定位错误
- 业务统计日志：帮助优化业务
- 系统审计日志：对系统的行为进行记录，和监控摄像头类似（安全需求）

日志和代码一样是演进的

## 1. 日志等级
需要注意 避免重复日志 要把握一个信息丰富程度的度， 可以根据日志等级调节

1. TRACE – The TRACE Level designates finer-grained informational events than the DEBUG
2. DEBUG – The DEBUG Level designates fine-grained informational events that are most useful to debug an application.
3. INFO – The INFO level designates informational messages that highlight the progress of the application at coarse-grained level.
4. WARN – The WARN level designates potentially harmful situations.
5. ERROR – The ERROR level designates error events that might still allow the application to continue running.
6. FATAL – The FATAL level designates very severe error events that will presumably lead the application to abort.
## 2. 容易遗漏的日志
