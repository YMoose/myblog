# wireshark 协议解析源代码框架
wireshark的协议解析源代码主要在src/epan目录下，其采用了协议树的方式进行组织，由新协议可通过注册自己的特征进入协议树。
## 注册
```
#include "config.h"
#include <epan/packet.h>

#define FOO_PORT 1234                       // 设定协议常用端口

static int proto_foo = -1;                  // protocol handle，当注册到wireshark主程序中时会被赋值(如下)

static const value_string packettypenames[] = {
    { 1, "Initialise" },
    { 2, "Terminate" },
    { 3, "Data" },
    { 0, NULL }
};

void
proto_register_foo(void)                    // 注册函数，wireshark启动时会调用
{
    static hf_register_info hf[] = {        // 定义协议细节所需要的字段, hf(head field)
        { &hf_foo_pdu_type,                 // 字段索引
            { "FOO PDU Type", "foo.type",   // {字段全程， 字段的简称（用于筛选），
            FT_UINT8, BASE_DEC,             // 字段类型， 打印格式 ， 
            VALS(packettypenames), 0x0,     // 值与字符串对应的map（用VALS宏）， flag与字符串对应用的(此处不介绍)
            NULL, HFILL }
        }
    };

    /* Setup protocol subtree array */
    static gint *ett[] = {                  // 子节点，用于记录细节字段解析的状态，以供查看是否需要解析
        &ett_foo
    };

    proto_foo = proto_register_protocol (
        "FOO Protocol", /* name        */
        "FOO",          /* short name  */
        "foo"           /* filter_name */
        );

    proto_register_field_array(proto_foo, hf, array_length(hf));        // 注册hf
    proto_register_subtree_array(ett, array_length(ett));               // 注册ett
}
```
```
void
proto_reg_handoff_foo(void)
{
    static dissector_handle_t foo_handle;
    // step1: 创建一个与协议handle"proto_foo"和解析函数"dissect_foo"绑定的协议解析器handle
    foo_handle = create_dissector_handle(dissect_foo, proto_foo);
    // step2: 向协议识别引擎注册这个解析器handle
    dissector_add_uint("udp.port", FOO_PORT, foo_handle);
}
```
PS. 按照惯例，以上两个函数都放在文件最末尾
## 解析
解析函数在注册时作为函数指针传入，模板如下
```
static int
dissect_foo(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree _U_, void *data _U_)
{
    col_set_str(pinfo->cinfo, COL_PROTOCOL, "FOO");
    /* Clear the info column */
    col_clear(pinfo->cinfo,COL_INFO);

    return tvb_captured_length(tvb);
}
```
参数
- tvbuff_t *tvb: 包数据
- packet_info *pinfo: 可以用于存取一些有关协议的一般信息
- proto_tree *tree: 细节解析需要使用的参数
- _U_: 向编译器表示此参数可能不用

### 细节解析
要对协议payload进行进一步解析，要通过`proto_tree_add_item()`构建一个subtree去解析。
```
static int
dissect_foo(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, void *data _U_)
{
    col_set_str(pinfo->cinfo, COL_PROTOCOL, "FOO");
    /* Clear out stuff in the info column */
    col_clear(pinfo->cinfo,COL_INFO);

    proto_item *ti = proto_tree_add_item(tree, proto_foo, tvb, 0, -1, ENC_NA);  // 添加subtree
    proto_tree *foo_tree = proto_item_add_subtree(ti, ett_foo);                 // subtree下添加item
    proto_tree_add_item(foo_tree, hf_foo_pdu_type, tvb, 0, 1, ENC_BIG_ENDIAN);  // 给item添加subtree解析

    return tvb_captured_length(tvb);
}
```
```
proto_item *
proto_tree_add_item(proto_tree *tree, int hfindex, tvbuff_t *tvb,
		    const gint start, gint length, const guint encoding)
{
	register header_field_info *hfinfo;

	PROTO_REGISTRAR_GET_NTH(hfindex, hfinfo);
	return proto_tree_add_item_new(tree, hfinfo, tvb, start, length, encoding);
}
```
参数
- proto_tree *tree: 上级协议解析树
- int hfindex: 需要解析的协议/字段索引
- tvbuff_t *tvb: 包数据
- const gint start: subtree解析起始位置
- gint length: subtree解析长度，-1代表到tvb结尾
- const guint encoding: 编码方法(`ENC_NA`(not applicable)/`ENC_BIG_ENDIAN`(big endian)/`ENC_LITTLE_ENDIAN`(little endian))

