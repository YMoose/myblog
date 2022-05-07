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

/* 注册函数，将协议解析函数与protocol handle 绑定 */
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
    tree _U_: 当有更深层的解析时使用(_U_: 告诉编译器可能不适用，不报warning) 
    data _U_: 当有更深层的解析时使用(_U_: 告诉编译器可能不适用，不报warning)
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
        tree: 当前tree
        proto_foo:
        tvb:
        0: tvb起始偏移
        -1: tvb结束偏移
        ENC_NA: encoding方式。ENC_NA("not applicable")，其他还有ENC_BIG_ENDIAN(big endian)、ENC_LITTLE_ENDIAN(little endian)
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