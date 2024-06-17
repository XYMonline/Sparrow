## CA
如果ca文件夹下没有证书和私钥，会使用`server_vertificate.hpp`中硬编码的自签名证书和私钥。
在conf.json中填写证书和密钥路径，加载证书。

## config字段说明
| 字段 | 说明 |
| --- | --- |
| host | 服务器监听的域名 |
| auth_client_port_range | 认证服务器监听客户端连接的端口范围 |
| route_auth_port_range | 路由服务器监听认证服务器连接的端口范围 |
| route_route_port_range | 路由服务器监听其他路由节点连接的端口范围 |
| route_business_port_range | 路由服务器监听业务服务器连接的端口范围 |
| route_supervisor_port_range | 路由服务器监听集群管理程序连接的端口范围 |
| business_client_port_range | 业务服务器监听客户端连接的端口范围 |
| auth_update_interval | 认证服务器发送负载信息的间隔(ms) |
| route_update_interval | 路由服务器发送负载信息的间隔(ms) |
| business_update_interval | 业务服务器发送负载信息的间隔(ms) |
| work_thread_num | 服务器程序工作线程数 |
| doc_root | 网站主页路径？暂时没用 |
| preload_user_info_limit | 预载的用户信息条数 |
| cert_path | tls证书路径 |
| private_key_path | 密钥路径 |
| dh_path | dh路径 |
| db_host | 数据库地址 |
| db_port | 数据库端口 |
| db_name | 数据库名字 |
| cache_host | 缓存服务器(redis)地址 |
| cache_port | 缓存服务器端口 |
| log_type | 日志类型 |
| log_level | 日志级别 |
| log_path | 日志保存路径 |
| log_file_size | 日志文件大小 |
| log_file_num | 日志文件数量 |