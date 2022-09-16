# vector
头文件目录 /src/vppinfra/vec.h

## 简介
vector是可以动态改变大小的数组，它包含一个user header，许多CLIB 数据结构（如 hash heap pool）都是基于vector，它们的user header有所不同。

内存布局
~~~~~~~~
                    user header (aligned to uword boundary)
                    vector length: number of elements
   user's pointer-> vector element #0
                    vector element #1
                    ...
~~~~~~~~


- 用户指针指向了第一个元素 #0，vec不需要特地初始化，null指针是有效的vector， 表示长度为0的vector，可以直接使用vec_add这些宏直接添加元素，在添加时宏会自动完成初始化过程

- 可以使用vec_reset_length(v) 将数组长度设置为0

- 用户可以指定元素的对齐方式，使用vec_*_aligned宏。

- vector中的元素可以是任何C类型， int double struct等。

- 许多宏定义都支持内存对齐。也可以在结构体中加入CLIB_CACHE_LINE_ALIGN_MARK() 对齐。

## 数据结构及方法
vecotr的数据结构如下：
```
typedef struct
{
  u32 len; /**数组长度，注意不是分配的内存大小**/
  u8 numa_id; /**< NUMA id */
  u8 vpad[3]; /**< pad to 8 bytes */
  u8 vector_data[0];  /**< Vector data . */
} vec_header_t;
```

| 序号 | 宏或者函数 | 说明 | 备注 |
| - |----------------|------------------|---------------------|
| 1 | vec_validate(V, I) | 确保V的长度足够访问I,如果不够resize V。用于预分配Vecotr大小| 无 |
| 2 | vec_len(V) | 计算V的长度 | 当V=null，返回为0 |
| 3 | vec_free(V) | free V占用的存储空间 | 无 |
| 4 | vec_end(V) | 返回data的结尾地址 | 无 |
| 5 | vec_bytes(V) | 返回data占用的字节数目 | 无 |
| 6 | vec_is_member(V, ptr) | 判断ptr是否是data中的一个元素 | 无 |
| 7 | vec_foreach(var,vec) | 遍历V中元素 | 无 |
| 8 | vec_resize(V,N) | 在V中新增N个元素的存储空间 | 无 |
| 9 | vec_add2(V,P,N) | 在V中新增N个元素的存储空间，P为新增元素的起始位置 | 无 |
| 10 | vec_add1(V,E) | 在V中末尾新增E元素 | 无 |
| 11 | vec_add(V,E,N) | 在V中末尾新增N个元素 | 无 |
| 12 | vec_insert(V,N,M)  | 在V中的M位置插入N个元素的空间，并初始化为0 | 无 |
| 13 | vec_insert_elts(V,E,N,M) | 往V中插入N个元素 | 无 |
| 14 | vec_delete(V,N,M) | 在V中，从M开始删除N个元素 | 不会改变V占用存储空间的大小 |
| 15 | vec_dup(V) | 复制 V | 无 |
| 16 | vec_zero(V) | V中所有元素设置成0 | 无 |
| 18 | vec_is_equal(v1,v2) | 比较v1 和V2 是否相同，1相等，0不想等| 无 |
| 19 | vec_set(v,val) | 把v中所有元素都设置成元素val | 无 |
| 20 | vec_append(v1,v2) | 把v2合并到v1中结尾 | 无 |
| 21 | vec_prepend(v1, v2) | 把v2 合并到v1的前面 | 无 |
| 22 | vec_clone(v1, v2) | 克隆v2 到 v1，但是v1和v2的元素类型可能不一样 | 无 |
| 23 | vec_new(T,N) | 创建一个动态数组，T为类型，N为长度 | 无 |
| 24 | vec_alloc(V,N) | 给V多分配N个元素空间 | 无 |
| 25 | vec_copy(DST,SRC) | 数组元素拷贝 | 无 |
| 26 | vec_pop(V)| 移除数组中最后一个元素 | 无 |
| 27 | vec_cmp(v1,v2) | 比较两个数组大小 | 无 |
| 28 | vec_search(v,E) | 从v中搜索 E元素 | 无 |
| 29 | vec_reset_length(v, l) | 改变v的长度 | 无 |
| 30 | _vec_find(V) | 找到vector header的起始地址 | 无 |
| 31 | vec_header(void *v, uword header_bytes) | 找到user header的起始地址 | 无 |
| 32 | vec_header_end (void *v, uword header_bytes) | 找到user header的结尾地址 | 无 |

_h 表示 user header 大小
_a 表示元素对齐 大小
_n 表示numa id