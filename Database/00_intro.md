# 数据库
根据数据的保存格式来进行分类
1. 层次数据库(Hierarchical Database, HDB): 数据通过树形结构表现
2. 关系数据库(Relational Database, RDB): 采用由行和列组成的二维表来管理数据，同时使用专门的SQL对数据进行操作
3. 面对对象数据库(Object Oriented Database，OODB): 把数据以及对数据的操作集合起来以对象为单位进行管理
4. XML数据库(XML Database，XMLDB): 对xml形式的大量数据进行高速处理
5. 键值对存储系统(Key-Value Store，KVS): 单纯用来保存查询所使用的主键(Key)和值(Value)的组合的数据库。可以把它想象成关联数组或者散列(hash)
## 数据库的结构
最常见的就是客户端/服务器类型(C/S类型)
## 表的结构
RDBMS中由行和列组成的二维表来管理数据。表存储再由RDBMS管理的数据库中。  
根据SQL语句的内容返回的数据同样必须是二维表的形式(这也是关系型数据库的特征之一)
## SQL
- SQL是为操作数据库而开发的语言。
- SQL用关键字、表名、列名等组合为一条sql语句。
- 根据对RDBMS赋予的指令种类不同，SQL语句可以分为一下三类
  1. DDL(Data Definition Language，数据定义语言): 用来创建或者删除存储数据用的数据库以及数据库中的表等对象。DDL包含以下几种指令:
     1. CREATE：创建数据库和表等对象
     2. DROP：删除数据库和表等对象
     3. ALTER：修改数据库和表等对象的结构
  2. DML: DML(Data Manipulation Language，数据操纵语言): 用来查询或者变更表中的记录，也是使用最多的。DML 包含以下几种指令:
     1. SELECT：查询表中的数据
     2. INSERT：向表中插入新数据
     3. UPDATE：更新表中的数据
     4. DELETE：删除表中的数据
  3. DCL(Data Control Language，数据控制语言): 用来确认或者取消对数据库中的数据进行的变更。除此之外，还可以对 RDBMS 的用户是否有权限操作数据库中的对象（数据库表等）进行设定。DCL 包含以下几种指令。
     1. COMMIT：确认对数据库中的数据进行的变更
     2. ROLLBACK： 取消对数据库中的数据进行的变更
     3. GRANT：赋予用户操作权限
     4. REVOKE：取消用户的操作权限
### SQL基本书写规则
- 以`;`结尾
- 关键字不区分大小写，数据区分
- 常数的书写方式是固定的(字符串和日期用`'`括起来，数字直接写)
- 命名数据库、表、列时，只能用英文字母、数字、`_`，且必须以半角英文字母开头。
- 同一数据库不可以由两个相同名称的表
- 主键时可以特定一行数据的列。
## 注释的书写
- 书写在“--”之后，只能写在同一行
- 书写在“/*”和“*/”之间，可以跨多行