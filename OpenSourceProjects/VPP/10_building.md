# 编译
## 在ubuntu20.04上编译1810版本
首先，编译dpdk的时候回遇到vhost.h版本的问题而找不到某些宏，用一下diff可以解决
```diff
diff --git a/lib/librte_vhost/vhost.h b/lib/librte_vhost/vhost.h
index b4abad30c..b569992d7 100644
--- a/lib/librte_vhost/vhost.h
+++ b/lib/librte_vhost/vhost.h
@@ -243,6 +243,18 @@ struct vring_packed_desc_event {
 };
 #endif
 
+/* Temporary to allow compilation */
+#ifndef VRING_DESC_F_AVAIL
+#define VRING_DESC_F_AVAIL     (1ULL << 7)
+#define VRING_DESC_F_USED      (1ULL << 15)
+#endif
+
+#ifndef VRING_EVENT_F_ENABLE
+#define VRING_EVENT_F_ENABLE 0x0
+#define VRING_EVENT_F_DISABLE 0x1
+#define VRING_EVENT_F_DESC 0x2
+#endif
+
 /*
  * Available and used descs are in same order
  */
```
其次，编译有些东西需要安装python2 以及 
```bash
apt isntall python2
apt install libnuma-dev
apt install python-ply
```