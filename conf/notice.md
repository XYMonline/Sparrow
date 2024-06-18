## CA
默认使用`server_vertificate.hpp`中硬编码的自签名证书和私钥。
在conf.json中填写证书和密钥路径，加载证书。

## config字段说明
| 字段 | 值类型 | 可选值 |  说明 |
| --- | --- | --- | --- |
| host | string | 合法的ip | 服务器监听的域名 | 
| auth_client_port_range | array | [a, b] 合法的端口, a <= b | 认证服务器监听客户端连接的端口范围 |
| route_auth_port_range | array | [a, b] 合法的端口, a <= b | 路由服务器监听认证服务器连接的端口范围 |
| route_route_port_range | array | [a, b] 合法的端口, a <= b | 路由服务器监听其他路由节点连接的端口范围 |
| route_business_port_range | array | [a, b] 合法的端口, a <= b | 路由服务器监听业务服务器连接的端口范围 |
| route_supervisor_port_range | array | [a, b] 合法的端口, a <= b | 路由服务器监听集群管理程序连接的端口范围 |
| business_client_port_range | array | [a, b] 合法的端口, a <= b | 业务服务器监听客户端连接的端口范围 |
| auth_update_interval | int | a >= 0 | 认证服务器发送负载信息的间隔(ms) |
| route_update_interval | int | a >= 0 | 路由服务器发送负载信息的间隔(ms) |
| business_update_interval | int | a >= 0 | 业务服务器发送负载信息的间隔(ms) |
| work_thread_num | int | 2 <= a <= 物理线程数 | 服务器程序工作线程数 |
| doc_root | string | 合法路径 | 网站主页路径？暂时没用 |
| preload_user_info_limit | int | a >= 0 | 预载的用户信息条数 |
| cert_path | string | 合法路径/空 | 证书路径 |
| key_path | string | 合法路径/空 | 私钥路径 |
| dh_path | string | 合法路径/空 | dh路径 |
| db_host | string | 合法的ip | 数据库地址 |
| db_port | int | 合法的端口 | 数据库端口 |
| db_name | string | 合法的数据库名 | 数据库名字 |
| db_user | string | 数据库用户名 | 数据库用户名 |
| db_password | string | - | 数据库用户对应的密码 |
| cache_host | string | 合法的ip | 缓存服务器(redis)地址 |
| cache_port | int | 合法的端口 | 缓存服务器端口 |
| cache_user | string | 合法的账户名或空 | 缓存服务器的用户名 |
| cache_password | string | - | 缓存服务器用户对应的密码 |
| log_type | string | console basic rotating daily | 日志类型 |
| log_level | string | debug info error warn | 日志级别 |
| log_path | string | 合法的路径 | 日志保存路径 |
| log_file_size | int | a > 0 | 日志文件大小 |
| log_file_num | int | a > 0 | 日志文件数量 |
