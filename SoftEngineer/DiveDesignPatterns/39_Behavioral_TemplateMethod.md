# 行为型模式-模板方法
组件协作类
背景: 现代软件专业分工后得到了"框架/应用"的划分。框架是稳定的部分，应用是变化的部分。组件协作类模式通过晚期绑定，来实现框架与应用间的松耦合。
- **Template Method** ：稳定的算法框架中存在某些可改变的单一步骤
- Strategy ：同一类对象中不同的对象使用不同的策略
- Observer ：事件通知

## 动机
**稳定的**整体操作结构和**变化的**子步骤（或者说是**晚绑定**的子步骤）。

## 实现
![UML](pics/39_TemplateMethod_UML.png)

## 应用到的原则
1. 早绑定 -> 晚绑定

## 代码实现

### C
``` C
static_always_inline void
pop_ctx_init (···)
{
  ···
  ret_ctx->pop_mime = mime_ctx_init (sess->thread_index, sess, 0, APP_TYPE_POP,
				     File_Proto_POP, ret_ctx);
  mime_ctx_register_header_line_handler (
    ret_ctx->pop_mime, pop_header_line_handler, (u8 *) ret_ctx);
  mime_ctx_register_body_line_handler (ret_ctx->pop_mime,
				       pop_body_line_handler, (u8 *) ret_ctx);
  ···
}
```