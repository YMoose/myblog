# 行为型模式 - 策略模式
- Template Method ：稳定的算法框架中存在某些可改变的单一步骤
- **Strategy** ：同一类对象中不同的对象使用不同的策略
- Observer ：事件通知
## 动机
**变化的**算法， **稳定的**算法作用对象（算法上下文）
## 实现
![UML](pics/38_Strategy_UML.png)
## 代码实现
### C
``` C
const static mime_process_decoder decoder[] = {
  process_none_decode,	 /* DECODE_NONE*/
  process_none_decode,	 /* DECODE_7BIT*/
  process_none_decode,	 /* DECODE_8BIT*/
  process_none_decode,	 /* DECODE_BINARY*/
  process_base64_decode, /* DECODE_B64*/
  process_none_decode,	 /* DECODE_QP*/
  process_uu_decode,	 /* DECODE_UU*/
};

u32
mime_process_decode (decode_ctx_t *ctx)
{
  return ctx && decoder[ctx->type] && ctx->buf_need_decode_len ?
	   decoder[ctx->type](ctx) :
	   0;
}
```
## 相对应的坏味道
Switch Statement(大量的if-elif-else)
