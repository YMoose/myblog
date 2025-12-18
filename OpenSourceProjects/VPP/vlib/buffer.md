# vlib buffer
vlib_buffer_t是vpp的基本包数据结构(等同于dpdk的ret_mbuf)
```C
// file: src/vlib/buffer.h
typedef union
{
  struct
  {
    CLIB_CACHE_LINE_ALIGN_MARK (cacheline0);

    /** signed offset in data[], pre_data[] that we are currently
      * processing. If negative current header points into predata area. packet开始的index位置，如果是负的则指向predata区域 */
    i16 current_data;

    /** Nbytes between current data and the end of this buffer.  */
    u16 current_length;

    /** buffer flags:
	<br> VLIB_BUFFER_FREE_LIST_INDEX_MASK: bits used to store free list index,
	<br> VLIB_BUFFER_IS_TRACED: trace this buffer.
	<br> VLIB_BUFFER_NEXT_PRESENT: this is a multi-chunk buffer.
	<br> VLIB_BUFFER_TOTAL_LENGTH_VALID: as it says
	<br> VLIB_BUFFER_EXT_HDR_VALID: buffer contains valid external buffer manager header,
	set to avoid adding it to a flow report
	<br> VLIB_BUFFER_FLAG_USER(n): user-defined bit N
     */
    u32 flags;

    /** Generic flow identifier */
    u32 flow_id;

    /** Reference count for this buffer. */
    volatile u8 ref_count;

    /** index of buffer pool this buffer belongs. */
    u8 buffer_pool_index;

    /** Error code for buffers to be enqueued to error handler.  */
    vlib_error_t error;

    /** Next buffer for this linked-list of buffers. Only valid if
      * VLIB_BUFFER_NEXT_PRESENT flag is set. */
    u32 next_buffer;

    /** The following fields can be in a union because once a packet enters
     * the punt path, it is no longer on a feature arc */
    union
    {
      /** Used by feature subgraph arcs to visit enabled feature nodes */
      u32 current_config_index;
      /* the reason the packet once punted */
      u32 punt_reason;
    };

    /** Opaque data used by sub-graphs for their own purposes. (vnet_buffer_opaque_t(src/vnet/buffer.h)使用了) */
    u32 opaque[10];

    /** part of buffer metadata which is initialized on alloc ends here. */
      STRUCT_MARK (template_end);

    /** start of 2nd half (2nd cacheline on systems where cacheline size is 64) */
      CLIB_ALIGN_MARK (second_half, 64);

    /** Specifies trace buffer handle if VLIB_PACKET_IS_TRACED flag is
      * set. */
    u32 trace_handle;

    /** Only valid for first buffer in chain. Current length plus total length
      * given here give total number of bytes in buffer chain. */
    u32 total_length_not_including_first_buffer;

    /**< More opaque data, see ../vnet/vnet/buffer.h (我们可以添加自己数据结构的地方)*/
    u32 opaque2[14];

    /** start of buffer headroom */
      CLIB_ALIGN_MARK (headroom, 64);

    /** Space for inserting data before buffer start.  Packet rewrite string
      * will be rewritten backwards and may extend back before
      * buffer->data[0].  Must come directly before packet data. 数据包其他信息存放的地方，我们很少用到*/
    u8 pre_data[VLIB_BUFFER_PRE_DATA_SIZE];

    /** Packet data */
    u8 data[];
  };
#ifdef CLIB_HAVE_VEC128
  u8x16 as_u8x16[4];
#endif
#ifdef CLIB_HAVE_VEC256
  u8x32 as_u8x32[2];
#endif
#ifdef CLIB_HAVE_VEC512
  u8x64 as_u8x64[1];
#endif
} vlib_buffer_t;
```