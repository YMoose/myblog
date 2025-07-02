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
### 定义及初始化
```C
// linux-6.15.3
// file: include/linux/kfifo.h
struct __kfifo {
	unsigned int	in;		// 生产者插入更新in
	unsigned int	out;	// 消费者消费更新out
	unsigned int	mask;
	unsigned int	esize;
	void		*data;
};

#define __STRUCT_KFIFO_COMMON(datatype, recsize, ptrtype) \
	union { \
		struct __kfifo	kfifo; \
		datatype	*type; \
		const datatype	*const_type; \
		char		(*rectype)[recsize]; \
		ptrtype		*ptr; \
		ptrtype const	*ptr_const; \
	}

#define __STRUCT_KFIFO(type, size, recsize, ptrtype) \
{ \
	__STRUCT_KFIFO_COMMON(type, recsize, ptrtype); \
	type		buf[((size < 2) || (size & (size - 1))) ? -1 : size]; \
}

#define STRUCT_KFIFO(type, size) \
	struct __STRUCT_KFIFO(type, size, 0, type)

#define __STRUCT_KFIFO_PTR(type, recsize, ptrtype) \
{ \
	__STRUCT_KFIFO_COMMON(type, recsize, ptrtype); \
	type		buf[0]; \
}

#define STRUCT_KFIFO_PTR(type) \
	struct __STRUCT_KFIFO_PTR(type, 0, type)

/*
 * define compatibility "struct kfifo" for dynamic allocated fifos
 */
struct kfifo __STRUCT_KFIFO_PTR(unsigned char, 0, void);

···

/**
 * DECLARE_KFIFO_PTR - macro to declare a fifo pointer object
 * @fifo: name of the declared fifo
 * @type: type of the fifo elements
 */
#define DECLARE_KFIFO_PTR(fifo, type)	STRUCT_KFIFO_PTR(type) fifo

/**
 * DECLARE_KFIFO - macro to declare a fifo object
 * @fifo: name of the declared fifo
 * @type: type of the fifo elements
 * @size: the number of elements in the fifo, this must be a power of 2
 */
#define DECLARE_KFIFO(fifo, type, size)	STRUCT_KFIFO(type, size) fifo

/**
 * INIT_KFIFO - Initialize a fifo declared by DECLARE_KFIFO
 * @fifo: name of the declared fifo datatype
 */
#define INIT_KFIFO(fifo) \
(void)({ \
	typeof(&(fifo)) __tmp = &(fifo); \
	struct __kfifo *__kfifo = &__tmp->kfifo; \
	__kfifo->in = 0; \
	__kfifo->out = 0; \
	__kfifo->mask = __is_kfifo_ptr(__tmp) ? 0 : ARRAY_SIZE(__tmp->buf) - 1;\
	__kfifo->esize = sizeof(*__tmp->buf); \
	__kfifo->data = __is_kfifo_ptr(__tmp) ?  NULL : __tmp->buf; \
})

/**
 * kfifo_init - initialize a fifo using a preallocated buffer
 * @fifo: the fifo to assign the buffer
 * @buffer: the preallocated buffer to be used
 * @size: the size of the internal buffer, this have to be a power of 2
 *
 * This macro initializes a fifo using a preallocated buffer.
 *
 * The number of elements will be rounded-up to a power of 2.
 * Return 0 if no error, otherwise an error code.
 */
#define kfifo_init(fifo, buffer, size) \
({ \
	typeof((fifo) + 1) __tmp = (fifo); \
	struct __kfifo *__kfifo = &__tmp->kfifo; \
	__is_kfifo_ptr(__tmp) ? \
	__kfifo_init(__kfifo, buffer, size, sizeof(*__tmp->type)) : \
	-EINVAL; \
})
```
kfifo 通过`struct __kfifo`数据结构来管理一段缓冲区（缓冲区可以使用者自行管理，也可以和kfifo一起创建），从而可以使缓冲区可以使用kfifo的接口。
内核的使用kfifo通常会使用`DECLARE_KFIFO`和`DECLARE_KFIFO_PTR`宏来声明fifo的数据结构，然后使用`INIT_KFIFO`进行初始化。如果是`DECLARE_KFIFO_PTR`声明的，还需要通过`kfifo_alloc`创建缓冲区。
todo `recsize`这个不知道是干嘛用的，代码中用的也很少。通常情况为0。
```C
struct counter_device {
···
	DECLARE_KFIFO_PTR(events, struct counter_event);
···
};

// file: mm/memory-failure.c
struct memory_failure_cpu {
	DECLARE_KFIFO(fifo, struct memory_failure_entry,
		      MEMORY_FAILURE_FIFO_SIZE);
	raw_spinlock_t lock;
	struct work_struct work;
};

static int __init memory_failure_init(void)
{
	struct memory_failure_cpu *mf_cpu;
	int cpu;

	for_each_possible_cpu(cpu) {
		mf_cpu = &per_cpu(memory_failure_cpu, cpu);
		raw_spin_lock_init(&mf_cpu->lock);
		INIT_KFIFO(mf_cpu->fifo);
		INIT_WORK(&mf_cpu->work, memory_failure_work_func);
	}

	register_sysctl_init("vm", memory_failure_table);

	return 0;
}
```

### 数据入队出队接口
kfifo通过在使用内存屏障指令使得单生产者单消费者的场景下无须加锁，既无锁队列。
kfifo作为无锁队列使用时，有以下前提
1. 单生产者单消费者场景：单生产者和单消费者保证了只有一个指令流对共享资源有写操作，生产者操作in，消费者操作out。
2. 未使用`kfifo_reset()`：`kfifo_reset`对两个共享资源都有写操作，可能导致另一个指令流对共享资源写操作的冲突。
3. 只有在消费者端使用了`kfifo_reset_out()`：在消费者端使用`kfifo_reset_out`和消费者消费相同，只对out进行了写操作。

先看入队接口，有如下两个接口
- `kfifo_put`：将单个元素val加入队列尾（in）
- `kfifo_in`：将buf中的n个元素搬运到队列尾（in）

本质上就是三部分
1. 查看队列剩余空间
2. `kfifo_copy_in`
3. 增加in。

> 注意：增加in之前，需要确保入队的数据已经写到内存上，且可被其他核（特别指消费者队列，查看队列元素长度时）看到（内存一致性）。这里使用了`smp_wmb`来保证。如果没有`smp_wmb`可能发生内存操作乱序，生产者还没有将入队数据填入到队列中，in已经变为了in+1，同时消费者正好消费in位置的错误数据（out=in），导致数据错误。

```C
// file: include/linux/kfifo.h
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
	typeof((fifo) + 1) __tmp = (fifo); \ // typeof((fifo) + 1)可指定tmp为fifo的类型，+1主要是为了类型安全，可以使得数组类型转换为指针类型，且可以在参数是复杂表达式的时候生效
	typeof((val) + 1) __val = (val); \
	unsigned int __ret; \
	const size_t __recsize = sizeof(*__tmp->rectype); \
	struct __kfifo *__kfifo = &__tmp->kfifo; \
	if (0) { \  // 编译时，检查类型是否一致，类型断言
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
 * kfifo_in - put data into the fifo
 * @fifo: address of the fifo to be used
 * @buf: the data to be added
 * @n: number of elements to be added
 *
 * This macro copies the given buffer into the fifo and returns the
 * number of copied elements.
 *
 * Note that with only one concurrent reader and one concurrent
 * writer, you don't need extra locking to use these macro.
 */
#define	kfifo_in(fifo, buf, n) \
({ \
	typeof((fifo) + 1) __tmp = (fifo); \
	typeof(__tmp->ptr_const) __buf = (buf); \
	unsigned long __n = (n); \
	const size_t __recsize = sizeof(*__tmp->rectype); \
	struct __kfifo *__kfifo = &__tmp->kfifo; \
	(__recsize) ?\
	__kfifo_in_r(__kfifo, __buf, __n, __recsize) : \
	__kfifo_in(__kfifo, __buf, __n); \
})
```

```C
// file: lib/kfifo.c
static void kfifo_copy_in(struct __kfifo *fifo, const void *src,
		unsigned int len, unsigned int off)
{
	unsigned int size = fifo->mask + 1;
	unsigned int esize = fifo->esize;
	unsigned int l;

	off &= fifo->mask;
	if (esize != 1) {
		off *= esize;
		size *= esize;
		len *= esize;
	}
	l = min(len, size - off);

	memcpy(fifo->data + off, src, l);
	memcpy(fifo->data, src + l, len - l);
	/*
	 * make sure that the data in the fifo is up to date before
	 * incrementing the fifo->in index counter
	 */
	smp_wmb();
}

unsigned int __kfifo_in(struct __kfifo *fifo,
		const void *buf, unsigned int len)
{
	unsigned int l;

	l = kfifo_unused(fifo);
	if (len > l)
		len = l;

	kfifo_copy_in(fifo, buf, len, fifo->in);
	fifo->in += len;
	return len;
}
EXPORT_SYMBOL(__kfifo_in);

unsigned int __kfifo_in_r(struct __kfifo *fifo, const void *buf,
		unsigned int len, size_t recsize)
{
	if (len + recsize > kfifo_unused(fifo))
		return 0;

	__kfifo_poke_n(fifo, len, recsize);

	kfifo_copy_in(fifo, buf, len, fifo->in + recsize);
	fifo->in += len + recsize;
	return len;
}
EXPORT_SYMBOL(__kfifo_in_r);
```

出队接口与入队接口类似，有如下两个接口
- `kfifo_get`：将队列头（out）的元素搬运到val中
- `kfifo_in`：将队列头（out）的n个元素搬运到buf中
  
也一样有三个步骤
1. 查看队列剩余元素长度
2. `kfifo_copy_out`
3. 增加out

> 注意：同样的，增加out之前，需要确保出队的数据已经写到内存上，且可被其他核（特别指生产者队列，查看队列剩余长度时）看到（内存一致性）。如果没有`smp_wmb`可能发生内存操作乱序，消费者者还没有将出队数据搬运到buf中，out已经变为了out+1，同时生产者者正好将新入队的数据搬运out位置，搬运到buf的数据为新入队的数据，导致数据错误。

``` C
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

/**
 * kfifo_out - get data from the fifo
 * @fifo: address of the fifo to be used
 * @buf: pointer to the storage buffer
 * @n: max. number of elements to get
 *
 * This macro gets some data from the fifo and returns the numbers of elements
 * copied.
 *
 * Note that with only one concurrent reader and one concurrent
 * writer, you don't need extra locking to use these macro.
 */
#define	kfifo_out(fifo, buf, n) \
__kfifo_uint_must_check_helper( \
({ \
	typeof((fifo) + 1) __tmp = (fifo); \
	typeof(__tmp->ptr) __buf = (buf); \
	unsigned long __n = (n); \
	const size_t __recsize = sizeof(*__tmp->rectype); \
	struct __kfifo *__kfifo = &__tmp->kfifo; \
	(__recsize) ?\
	__kfifo_out_r(__kfifo, __buf, __n, __recsize) : \
	__kfifo_out(__kfifo, __buf, __n); \
}) \
)
```

todo 分析多/单生产者单/多消费者模式下的加锁同步情况
### 多线程场景
kfifo还有用spinlock提供了一些多线程场景下的接口。
```C
/**
 * kfifo_in_spinlocked - put data into the fifo using a spinlock for locking
 * @fifo: address of the fifo to be used
 * @buf: the data to be added
 * @n: number of elements to be added
 * @lock: pointer to the spinlock to use for locking
 *
 * This macro copies the given values buffer into the fifo and returns the
 * number of copied elements.
 */
#define	kfifo_in_spinlocked(fifo, buf, n, lock) \
({ \
	unsigned long __flags; \
	unsigned int __ret; \
	spin_lock_irqsave(lock, __flags); \
	__ret = kfifo_in(fifo, buf, n); \
	spin_unlock_irqrestore(lock, __flags); \
	__ret; \
})

/**
 * kfifo_in_spinlocked_noirqsave - put data into fifo using a spinlock for
 * locking, don't disable interrupts
 * @fifo: address of the fifo to be used
 * @buf: the data to be added
 * @n: number of elements to be added
 * @lock: pointer to the spinlock to use for locking
 *
 * This is a variant of kfifo_in_spinlocked() but uses spin_lock/unlock()
 * for locking and doesn't disable interrupts.
 */
#define kfifo_in_spinlocked_noirqsave(fifo, buf, n, lock) \
({ \
	unsigned int __ret; \
	spin_lock(lock); \
	__ret = kfifo_in(fifo, buf, n); \
	spin_unlock(lock); \
	__ret; \
})
```

### 使用
可以查看 lib/tests/kfifo_kunit.c 和 samples/kfifo/dma-example.c
## 参考
1. [环形缓冲区](https://www.kernel.org/doc/html/latest/translations/zh_CN/core-api/circular-buffers.html)
2. [知乎-Linux kernel中有哪些奇技淫巧？](https://www.zhihu.com/question/471637144/answer/2143542730)