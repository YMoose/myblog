# Wireshark 协议解析源码探索
从wireshark官方文档中的添加自定义协议解析部分学习wireshark协议解析代码的基本流程
## setting up the dissector 设置解析器
首先，此解析器是作为插件解析器加入到wireshark中(后续一点修改就可以转化为内置解析器)
### 初始化
``` C
#include "config.h"
#include <epan/packet.h>

#define FOO_PORT 1234

/* store protocol handle and is initialised to -1,
will be set when dissector is registered within the main program */
static int proto_foo = -1;

/* 协议字段 foo.type */
static int hf_foo_pdu_type = -1;

/* 协议subtree */
static gint ett_foo = -1;

/* 注册函数，将协议注册到wireshark主程序 */
void
proto_register_foo(void)
{
    /* 存储协议解析字段的数组，会由后面的函数proto_register_field_array进行注册 */
    static hf_register_info hf[] = {
        { &hf_foo_pdu_type,             /* 字段 index */ 
            { "FOO PDU Type",           /* 字段标签 */
            "foo.type",                 /* 字段简写(用于filter中) */
            FT_UINT8,                   /* 字段类型 */
            BASE_DEC,                   /* 此字段类型的展示方式(BASE_DEC(decimal)) */
            NULL,                       /* 
            0x0,
            NULL, 
            HFILL }
        }
    };

    /* Setup protocol subtree array */
    static gint *ett[] = {
        &ett_foo
    };

    /* 调用proto_register_protocol，返回protocol handle值 */
    proto_foo = proto_register_protocol (
        "FOO Protocol", /* name        */
        "FOO",          /* short name  */
        "foo"           /* filter_name */
        );

    proto_register_field_array(proto_foo, hf, array_length(hf));
    proto_register_subtree_array(ett, array_length(ett));
}

/* 注册函数，将协议解析函数与protocol_handle 绑定 */
void
proto_reg_handoff_foo(void)
{
    static dissector_handle_t foo_handle;
    /* Step1: 创建一个解析器handle， 用于绑定解析函数和协议 */
    foo_handle = create_dissector_handle(dissect_foo, proto_foo);
    /* Step2: 将这个解析器handle和对应的流量绑定 */
    dissector_add_uint("udp.port", FOO_PORT, foo_handle);
}
```

### Dissection 解析函数
``` C
/* 
    tvb: 存放数据包数据
    pinfo: 存放和协议有关的数据，可以在此对其更新
    tree _U_: 当有更深层的解析时使用(_U_: 告诉编译器可能不使用，不报warning) 
    data _U_: 当有更深层的解析时使用(_U_: 告诉编译器可能不使用，不报warning)
    */
static int
dissect_foo(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree _U_, void *data _U_)
{
    /* 将wireshark 显示时的协议这一栏改为此协议 */
    col_set_str(pinfo->cinfo, COL_PROTOCOL, "FOO");
    /* Clear the info column */
    col_clear(pinfo->cinfo,COL_INFO);

    /* 添加协议元素（subtree），用于更具体的解析，在界面上会高亮显示对应的字节 */
    /* 
        tree: 当前tree，也就是gui里的解析字段里的下拉字段
        proto_foo: 协议
        tvb:
        0: tvb起始偏移
        -1: tvb结束偏移
        ENC_NA: encoding方式。ENC_NA("not applicable")，其他还有ENC_BIG_ENDIAN(big endian)、ENC_LITTLE_ENDIAN(little endian)
        此函数用于gui里解析字段和字节对应
    */
    proto_item *ti = proto_tree_add_item(tree, proto_foo, tvb, 0, -1, ENC_NA);
    gint offset = 0;
    /* 为协议元素的解析添加subtree */
    proto_tree *foo_tree = proto_item_add_subtree(ti, ett_foo);
    proto_tree_add_item(foo_tree, hf_foo_pdu_type, tvb, offset, 1, ENC_BIG_ENDIAN);
    offset += 1;
    /* 可以用相同的方法, 对更多字段进行解析 */
    return tvb_captured_length(tvb);
}
```

### 解析中要用的数据结构
```C
#define FOO_START_FLAG      0x01
#define FOO_END_FLAG        0x02
#define FOO_PRIORITY_FLAG   0x04

static int hf_foo_startflag = -1;
static int hf_foo_endflag = -1;
static int hf_foo_priorityflag = -1;

static int hf_foo_pdu_type = -1;
static gint ett_foo = -1;

/* ... */

static const value_string packettypenames[] = {
    { 1, "Initialise" },
    { 2, "Terminate" },
    { 3, "Data" },
    { 0, NULL }
};

static int
dissect_foo(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, void *data _U_)
{
    ...
        ...
        static int* const bits[] = {
            &hf_foo_startflag,
            &hf_foo_endflag,
            &hf_foo_priorityflag,
            NULL
        };
        /* 添加bit位解析 */
        proto_tree_add_bitmask(foo_tree, tvb, offset, hf_foo_flags, ett_foo, bits, ENC_BIG_ENDIAN);
        offset += 1;
        ...
    ...
    return tvb_captured_length(tvb);
}

void
proto_register_foo(void)
{
    /* 存储协议解析字段的数组，会由后面的函数proto_register_field_array进行注册 */
    static hf_register_info hf[] = {
        { &hf_foo_pdu_type,             /* 字段 index */ 
            { "FOO PDU Type",           /* 字段标签 */
            "foo.type",                 /* 字段简写(用于filter中) */
            FT_UINT8,                   /* 字段类型 */
            BASE_DEC,                   /* 此字段类型的展示方式(BASE_DEC(decimal)) */
            VALS(packettypenames),      /* 可以根据解析值做映射 */
            0x0,                        /* 对bit解析 */
            NULL, 
            HFILL }
        },
        { &hf_foo_flags,
            { "FOO PDU Flags", "foo.flags",
            FT_UINT8, BASE_HEX,
            NULL, 0x0,
            NULL, HFILL }
        },
        { &hf_foo_startflag,
            { "FOO PDU Start Flags", "foo.flags.start",
            FT_BOOLEAN, 8,
            NULL, FOO_START_FLAG,
            NULL, HFILL }
        },
        { &hf_foo_endflag,
            { "FOO PDU End Flags", "foo.flags.end",
            FT_BOOLEAN, 8,
            NULL, FOO_END_FLAG,
            NULL, HFILL }
        },
        { &hf_foo_priorityflag,
            { "FOO PDU Priority Flags", "foo.flags.priority",
            FT_BOOLEAN, 8,
            NULL, FOO_PRIORITY_FLAG,
            NULL, HFILL }
        },
    };

    /* Setup protocol subtree array */
    static gint *ett[] = {
        &ett_foo
    };

    proto_foo = proto_register_protocol (
        "FOO Protocol", /* name       */
        "FOO",          /* short_name */
        "foo"           /* filter_name*/
        );

    proto_register_field_array(proto_foo, hf, array_length(hf));
    proto_register_subtree_array(ett, array_length(ett));
}
```

## 常用数据结构及函数
### tvbuff_t
可以看作是数据包的数据buff
### pinfo
用于存放数据包解析后的一些信息与元数据
#### 数据结构
``` C
typedef struct _packet_info {
  const char *current_proto;        /**< name of protocol currently being dissected */
  struct epan_column_info *cinfo;   /**< Column formatting information */
  guint32 presence_flags;           /**< Presence flags for some items */
  guint32 num;                      /**< Frame number */
  nstime_t abs_ts;                  /**< Packet absolute time stamp */
  nstime_t rel_ts;                  /**< Relative timestamp (yes, it can be negative) */
  frame_data *fd;
  union wtap_pseudo_header *pseudo_header;
  wtap_rec *rec;                    /**< Record metadata */
  GSList *data_src;                 /**< Frame data sources */
  address dl_src;                   /**< link-layer source address */
  address dl_dst;                   /**< link-layer destination address */
  address net_src;                  /**< network-layer source address */
  address net_dst;                  /**< network-layer destination address */
  address src;                      /**< source address (net if present, DL otherwise )*/
  address dst;                      /**< destination address (net if present, DL otherwise )*/
  guint32 vlan_id;                  /**< First encountered VLAN Id if present otherwise 0 */
  const char *noreassembly_reason;  /**< reason why reassembly wasn't done, if any */
  gboolean fragmented;              /**< TRUE if the protocol is only a fragment */
  struct {
    guint32 in_error_pkt:1;         /**< TRUE if we're inside an {ICMP,CLNP,...} error packet */
    guint32 in_gre_pkt:1;           /**< TRUE if we're encapsulated inside a GRE packet */
  } flags;
  port_type ptype;                  /**< type of the following two port numbers */
  guint32 srcport;                  /**< source port */
  guint32 destport;                 /**< destination port */
  guint32 match_uint;               /**< matched uint for calling subdissector from table */
  const char *match_string;         /**< matched string for calling subdissector from table */
  gboolean use_endpoint;            /**< TRUE if endpoint member should be used for conversations */
  struct endpoint* conv_endpoint;   /**< Data that can be used for conversations */
  guint16 can_desegment;            /**< >0 if this segment could be desegmented.
                                         A dissector that can offer this API (e.g.
                                         TCP) sets can_desegment=2, then
                                         can_desegment is decremented by 1 each time
                                         we pass to the next subdissector. Thus only
                                         the dissector immediately above the
                                         protocol which sets the flag can use it*/
  guint16 saved_can_desegment;      /**< Value of can_desegment before current
                                         dissector was called.  Supplied so that
                                         dissectors for proxy protocols such as
                                         SOCKS can restore it, allowing the
                                         dissectors that they call to use the
                                         TCP dissector's desegmentation (SOCKS
                                         just retransmits TCP segments once it's
                                         finished setting things up, so the TCP
                                         desegmentor can desegment its payload). */
  int desegment_offset;             /**< offset to stuff needing desegmentation */
#define DESEGMENT_ONE_MORE_SEGMENT 0x0fffffff
#define DESEGMENT_UNTIL_FIN        0x0ffffffe
  guint32 desegment_len;            /**< requested desegmentation additional length
                                       or
                                       DESEGMENT_ONE_MORE_SEGMENT:
                                         Desegment one more full segment
                                         (warning! only partially implemented)
                                       DESEGMENT_UNTIL_FIN:
                                         Desgment all data for this tcp session
                                         until the FIN segment.
                                    */
  guint16 want_pdu_tracking;    /**< >0 if the subdissector has specified
                                   a value in 'bytes_until_next_pdu'.
                                   When a dissector detects that the next PDU
                                   will start beyond the start of the next
                                   segment, it can set this value to 2
                                   and 'bytes_until_next_pdu' to the number of
                                   bytes beyond the next segment where the
                                   next PDU starts.

                                   If the protocol dissector below this
                                   one is capable of PDU tracking it can
                                   use this hint to detect PDUs that starts
                                   unaligned to the segment boundaries.
                                   The TCP dissector is using this hint from
                                   (some) protocols to detect when a new PDU
                                   starts in the middle of a tcp segment.

                                   There is intelligence in the glue between
                                   dissector layers to make sure that this
                                   request is only passed down to the protocol
                                   immediately below the current one and not
                                   any further.
                                */
  guint32 bytes_until_next_pdu;

  int     p2p_dir;              /**< Packet was captured as an
                                       outbound (P2P_DIR_SENT)
                                       inbound (P2P_DIR_RECV)
                                       unknown (P2P_DIR_UNKNOWN) */

  GHashTable *private_table;    /**< a hash table passed from one dissector to another */

  wmem_list_t *layers;      /**< layers of each protocol */
  guint8 curr_layer_num;       /**< The current "depth" or layer number in the current frame */
  guint16 link_number;

  guint16 clnp_srcref;          /**< clnp/cotp source reference (can't use srcport, this would confuse tpkt) */
  guint16 clnp_dstref;          /**< clnp/cotp destination reference (can't use dstport, this would confuse tpkt) */

  int link_dir;                 /**< 3GPP messages are sometime different UP link(UL) or Downlink(DL) */

  gint16 src_win_scale;        /**< Rcv.Wind.Shift src applies when sending segments; -1 unknown; -2 disabled */
  gint16 dst_win_scale;        /**< Rcv.Wind.Shift dst applies when sending segments; -1 unknown; -2 disabled */

  GSList* proto_data;          /**< Per packet proto data */

  GSList* dependent_frames;     /**< A list of frames which this one depends on */

  GSList* frame_end_routines;

  wmem_allocator_t *pool;      /**< Memory pool scoped to the pinfo struct */
  struct epan_session *epan;
  const gchar *heur_list_name;    /**< name of heur list if this packet is being heuristically dissected */
} packet_info;
```
### conversation_t
用于存放会话上下文
#### 数据结构
``` C 
/**
 * Data structure representing a conversation.
 */
struct conversation_key;
typedef struct conversation_key* conversation_key_t;

typedef struct conversation {
	struct conversation *next;	/** pointer to next conversation on hash chain */
	struct conversation *last;	/** pointer to the last conversation on hash chain */
	struct conversation *latest_found; /** pointer to the last conversation on hash chain */
	guint32	conv_index;		/** unique ID for conversation */
	guint32 setup_frame;		/** frame number that setup this conversation */
					/* Assume that setup_frame is also the lowest frame number for now. */
	guint32 last_frame;		/** highest frame number in this conversation */
	wmem_tree_t *data_list;		/** list of data associated with conversation */
	wmem_tree_t *dissector_tree;	/** tree containing protocol dissector client associated with conversation */
	guint	options;		/** wildcard flags */
	conversation_key_t key_ptr;	/** pointer to the key for this conversation */
} conversation_t;
```
#### 常用函数
``` C
    /* 根据pinfo找到这条会话的上下文，如果没有找到则创建一个新的会话 */
    conversation = find_or_create_conversation(pinfo);
    /* 将协议解析所需要的上下文暂时存放在conversation会话上下文中 */
    conversation_add_proto_data(conversation, proto_foo, foo_info);
```