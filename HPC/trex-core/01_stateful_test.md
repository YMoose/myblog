# stateful test
# 配置文件
```
- duration : 10.0
  generator :                           # 流量生成配置
          distribution : "seq"          
          clients_start : "16.0.0.1"    # 模拟客户端起始ip
          clients_end   : "16.0.1.255"  # 模拟客户端结束ip
          servers_start : "48.0.0.1"    # 模拟服务端起始ip
          servers_end   : "48.0.0.255"  # 模拟服务端结束ip
          clients_per_gb : 201
          min_clients    : 101
          dual_port_mask : "1.0.0.0" 
          tcp_aging      : 1
          udp_aging      : 1
  #cap_ipg  : true                      # 采用pcap文件中的包间间隔
  #cap_ipg_min  : 30                    # 最小包间间隔，如果pcap文件中的包间间隔比这个小则采用这个值
  #cap_override_ipg :200                # 包间间隔强制采用这个值
  cap_info : 
     - name: cap2/dns.pcap              # pcap 文件路径
       cps : 1.0                        # connections per second
       ipg : 10000                      # inter-packet gap(microseconds). 10,000 = 10msec
       #server_addr      : "48.0.0.7"   # 采用这个包的所有流的server_addr 都采用这个值
       #one_app_server   : true         # 要用上面那个配置这里就要用true
       rtt : 10000                      # 要和igp数值相同
       w   : 1                          # 每对c/s的ip发几条流
     - name: avl/delay_10_http_browsing_0.pcap  # 混合流量的另一条流配置
       cps : 2.0
       ipg : 10000
       rtt : 10000
       w: 1
```