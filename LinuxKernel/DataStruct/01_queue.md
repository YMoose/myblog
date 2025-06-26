# queue
linux 内核中的（先进先出）队列的实现。这里的队列指的是环形缓冲区，通常来说环形缓冲区会被设置为固定大小，由生产者填充数据到一端，由消费者从另一端消耗数据。
目前我了解的有两种实现一种是`struct circ_buf`和`struct __kfifo`
## circ_buf
代码很短，实现非常简洁，下面就是所有的实现代码了。
设计思路应该就是从简单出发，缓冲区由外部分配，大小`size`由外部维护。同时由外部负责代码运行时共享数据的同步互斥关系。另外，`struct circ_buf`的实际缓存`buf`的长度必须是2的幂次方，因为其计算已缓存长度时的方法用的是按位与方法（为了加速和简洁），其前提就是`size`必须是2的幂次方。
``` C
// file: include/linux/circ_buf.h
struct circ_buf {
	char *buf;
	int head;   // 生产者插入更新head
	int tail;   // 消费者消费更新tail
};

/* Return count in buffer.  */
#define CIRC_CNT(head,tail,size) (((head) - (tail)) & ((size)-1))

/* Return space available, 0..size-1.  We always leave one free char
   as a completely full buffer has head == tail, which is the same as
   empty.  */
#define CIRC_SPACE(head,tail,size) CIRC_CNT((tail),((head)+1),(size))

/* Return count up to the end of the buffer.  Carefully avoid
   accessing head and tail more than once, so they can change
   underneath us without returning inconsistent results.  */
#define CIRC_CNT_TO_END(head,tail,size) \
	({int end = (size) - (tail); \
	  int n = ((head) + end) & ((size)-1); \
	  n < end ? n : end;})

/* Return space available up to the end of the buffer.  */
#define CIRC_SPACE_TO_END(head,tail,size) \
	({int end = (size) - 1 - (head); \
	  int n = (end + (tail)) & ((size)-1); \
	  n <= end ? n : end+1;})

```
### 使用
`struct circ_buf`在内核中的应用场景主要集中在串口驱动中。
``` C
static int uart_write(struct tty_struct *tty,
					const unsigned char *buf, int count)
{
	struct uart_state *state = tty->driver_data;
	struct uart_port *port;
	struct circ_buf *circ;
	unsigned long flags;
	int c, ret = 0;

	/*
	 * This means you called this function _after_ the port was
	 * closed.  No cookie for you.
	 */
	if (!state) {
		WARN_ON(1);
		return -EL3HLT;
	}

	port = state->uart_port;
	circ = &state->xmit;

	if (!circ->buf)
		return 0;

	spin_lock_irqsave(&port->lock, flags);
	while (1) {
		c = CIRC_SPACE_TO_END(circ->head, circ->tail, UART_XMIT_SIZE);
		if (count < c)
			c = count;
		if (c <= 0)
			break;
		memcpy(circ->buf + circ->head, buf, c);
		circ->head = (circ->head + c) & (UART_XMIT_SIZE - 1);
		buf += c;
		count -= c;
		ret += c;
	}
	spin_unlock_irqrestore(&port->lock, flags);

	uart_start(tty);
	return ret;
}
```
## kfifo
kfifo的实现同时沿用了用circ_buf中位操作来加速的思路（所以也需要缓冲区长度为2的幂次方），但相较于circ_buf实现上更复杂一些，操作接口更抽象。kfiko可以通过`kfifo_alloc`自行创建缓冲区（推荐自行创建，会自动完善缓冲区长度），并通过`mask`记录缓冲区大小。
kfifo通过在使用内存屏障指令使得单生产者单消费者的场景下无须加锁，既无锁队列。
kfifo作为无锁队列使用时，有一下前提
1. 单生产者/单消费者无需使用锁进行同步
2. 未使用kfifo_reset()
3. 只有在消费者端使用了kfifo_reset_out()
```C
// file: include/linux/kfifo.h
struct __kfifo {
	unsigned int	in;		// 生产者插入更新in
	unsigned int	out;	// 消费者消费更新out
	unsigned int	mask;
	unsigned int	esize;
	void		*data;
};
```
以下是kfifo的入队和出队接口，
```C
/**
 * kfifo_put - put data into the fifo
 * @fifo: address of the fifo to be used
 * @val: the data to be added
 *
 * This macro copies the given value into the fifo.
 * It returns 0 if the fifo was full. Otherwise it returns the number
 * processed elements.
 *
 * Note that with only one concurrent reader and one concurrent
 * writer, you don't need extra locking to use these macro.
 */
#define	kfifo_put(fifo, val) \
({ \
	typeof((fifo) + 1) __tmp = (fifo); \
	typeof((val) + 1) __val = (val); \
	unsigned int __ret; \
	const size_t __recsize = sizeof(*__tmp->rectype); \
	struct __kfifo *__kfifo = &__tmp->kfifo; \
	if (0) { \
		typeof(__tmp->ptr_const) __dummy __attribute__ ((unused)); \
		__dummy = (typeof(__val))NULL; \
	} \
	if (__recsize) \
		__ret = __kfifo_in_r(__kfifo, __val, sizeof(*__val), \
			__recsize); \
	else { \
		__ret = !kfifo_is_full(__tmp); \
		if (__ret) { \
			(__is_kfifo_ptr(__tmp) ? \
			((typeof(__tmp->type))__kfifo->data) : \
			(__tmp->buf) \
			)[__kfifo->in & __tmp->kfifo.mask] = \
				*(typeof(__tmp->type))__val; \
			smp_wmb(); \
			__kfifo->in++; \
		} \
	} \
	__ret; \
})

/**
 * kfifo_get - get data from the fifo
 * @fifo: address of the fifo to be used
 * @val: the var where to store the data to be added
 *
 * This macro reads the data from the fifo.
 * It returns 0 if the fifo was empty. Otherwise it returns the number
 * processed elements.
 *
 * Note that with only one concurrent reader and one concurrent
 * writer, you don't need extra locking to use these macro.
 */
#define	kfifo_get(fifo, val) \
__kfifo_uint_must_check_helper( \
({ \
	typeof((fifo) + 1) __tmp = (fifo); \
	typeof((val) + 1) __val = (val); \
	unsigned int __ret; \
	const size_t __recsize = sizeof(*__tmp->rectype); \
	struct __kfifo *__kfifo = &__tmp->kfifo; \
	if (0) \
		__val = (typeof(__tmp->ptr))0; \
	if (__recsize) \
		__ret = __kfifo_out_r(__kfifo, __val, sizeof(*__val), \
			__recsize); \
	else { \
		__ret = !kfifo_is_empty(__tmp); \
		if (__ret) { \
			*(typeof(__tmp->type))__val = \
				(__is_kfifo_ptr(__tmp) ? \
				((typeof(__tmp->type))__kfifo->data) : \
				(__tmp->buf) \
				)[__kfifo->out & __tmp->kfifo.mask]; \
			smp_wmb(); \
			__kfifo->out++; \
		} \
	} \
	__ret; \
}) \
)
```
todo 分析多/单生产者单/多消费者模式下的加锁同步情况

## 参考
1. [环形缓冲区](https://www.kernel.org/doc/html/latest/translations/zh_CN/core-api/circular-buffers.html)
2. [知乎-Linux kernel中有哪些奇技淫巧？](https://www.zhihu.com/question/471637144/answer/2143542730)