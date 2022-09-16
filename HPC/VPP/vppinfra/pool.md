# pool 
头文件目录 src/vppinfra/pool.h 

## 简介
Pool是使用Vector 和 bitmap结合而成的结构，用来快速alloc和free 固定大小且独立的数据结构，pool非常适合用来分配per-session的数据结构。

内存布局
~~~~~~~~
                    pool_header_t
                    vector length: number of elements
   user's pointer-> pool element #0
                    pool element #1
                    Not alloc.
                    pool element #3
                    ...
~~~~~~~~

- 用户可以指定元素的对齐方式，使用pool_*_aligned宏。

- 用户可以使用pool_foreach_*_index来在迭代器函数参数中加入body

## 数据结构及方法
pool的数据结构如下
```
typedef struct
{
  /** Bitmap of indices of free objects. 跟踪记录pool中空位的bitmap */
  uword *free_bitmap;

  /** Vector of free indices.  One element for each set bit in bitmap. 空位的vec地址，主要用于快速re-allocation */
  u32 *free_indices;

  /* The following fields are set for fixed-size, preallocated pools */

  /** Maximum size of the pool, in elements pool的容量*/
  u32 max_elts;

  /** mmap segment info: base + length mmap段信息：基址 + 长度 */
  u8 *mmap_base;
  u64 mmap_size;

} pool_header_t;
```

| 序号 | 宏或者函数 | 说明 | 备注 |
| - |----------------|------------------|---------------------|
| 1 | pool_header(void *v) | 找到pool_header_t的起始地址 | |
| 2 | pool_init_fixed(pool,max_elts) | pool为pool内存储的元素指针,初始化一个包含max_elts个元素的pool| |
| 3 | pool_validate(void *v) | 确保pool v的内存全部可用，可以是一个空指针，也可以清空一个已有的pool | |
| 4 | pool_validate_index(V, I) | 确保V的长度足够访问I,如果不够resize V。用于预分配Vecotr大小| |
| 5 | pool_elts(void *v) | 返回pool里激活的元素个数| |
| 6 | pool_len(p) | 返回pool的长度(所有激活未激活的元素个数) | |
| 7 | pool_header_bytes(void *v) | 返回pool header的字节数 | |
| 8 | pool_bytes(P) | 返回pool的字节数| |
| 9 | pool_free_elts(void *v) | 返回pool内空位数量| | 
| 11 | pool_get(P,E) | 从pool中申请(sizeof(*E)大小)的空间给指针E以供使用 | 空间申请并不会初始化为0，要用宏\*_zero才会 |
| 12 | pool_is_free(P,E) | 用pool_head_t的bitmap来查看指针E位置是否可用 | |
| 13 | pool_is_free_index(P,I) | 用pool_head_t的bitmap来查看index I位置是否可用 | |
| 14 | pool_put(P,E) | 将元素指针E上的空间free(返还给pool)| |
| 15 | pool_put_index(p,i) | 将index i上的空间free(返还给pool) | |
| 16 | pool_alloc(P,N) | 给pool添加N个空位(resize) | |
| 17 | pool_dup(P) | 返回pool P的一个深拷贝 | |
| 18 | pool_free(p) | 释放pool p | |
| 19 | pool_foreach_region(LO,HI,POOL,BODY) | 用BODY迭代POOL中从LO到HI的元素| 这里假设BODY足够智慧可以处理pool中不同的大小的元素数组 | 
| 20 | pool_foreach(VAR,POOL) | BODY写在POOL后面，用body遍历POOL中的元素，VAR作为迭代器的变量 | 用法: <br> `proc_t *procs;   // a pool of processes.`<br>`proc_t *proc;    // pointer to one process; used as the iterator.`<br>`pool_foreach (proc, procs, ({`<br>`if (proc->state != PROC_STATE_RUNNING)`<br>`    continue;`<br>`// check a running proc in some way`<br>`...`<br>`}));` |
| 21 | pool_elt_at_index(p,i) | 返回在Pool p中index i位置的指针 | |
| 21 | pool_next_index(P,I) | 返回Pool P中index I的下一个被占用的index | |
| 22 | pool_foreach_index(i,v) |  BODY写在POOL后面，用body遍历v中的元素，index i作为迭代器的变量 | 类似20 |
| 23 | pool_flush(VAR, POOL, BODY) | 用BODY遍历POOL中的每个元素，在对其进行释放，VAR是迭代变量 | 类似20 |