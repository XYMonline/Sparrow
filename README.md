# Sparrow

## 1. 介绍
Sparrow是一个轻量级的分布式服务器框架，基于Boost.Asio和C++20协程。

## 2.构建项目

### 2.1. 安装Vcpkg
```shell
git clone https://github.com/microsoft/vcpkg
cd vcpkg
```
`Windows`
```shell
.\bootstrap-vcpkg.bat
```
`Linux`
```shell
sudo apt-get install git cmake curl zip unzip tar pkg-config
```
```shell
./bootstrap-vcpkg.sh
```

### 2.2. 安装依赖
`Windows`
```shell
./vcpkg install openssl:x64-windows boost-uuid:x64-windows boost-asio:x64-windows nlohmann-json:x64-windows boost-mysql:x64-windows protobuf:x64-windows redis-plus-plus:x64-windows spdlog:x64-windows boost-beast:x64-windows parallel-hashmap:x64-windows
```
`Linux`
```bash
./vcpkg install openssl:x64-linux boost-uuid:x64-linux boost-asio:x64-linux nlohmann-json:x64-linux boost-mysql:x64-linux protobuf:x64-linux redis-plus-plus:x64-linux spdlog:x64-linuxlinux boost-beast:x64-linux parallel-hashmap:x64-linux
```

### 2.3. 构建运行

`Windows`
```
在Visual Studio 2022中打开项目 -> 生成 -> 全部生成
```

## 3. 在框架的基础上开发

在main分支中，只实现了登录验证和业务服务器的连接，将在新的分支中实现更多的功能，比如即时通讯、游戏服务器等。

### 3.1. 创建新的会话类型

所有的会话类型都以CRTP的方式继承自`websocket_session`和`std::enable_shared_from_this`，使用`std::make_shared`构造，并实现以下接口：

```cpp
//example
class session : public websocket_session<session>, public std::enable_shared_from_this<session>
{
public:
        // 进行消息处理的协程，self为保存自己的智能指针，用于维持生命周期
        net::awaitable<void> handle_messages(auto self) {...}

        // 返回服务器的名字，用于区分不同的服务器
        static std::string server_name() {...}
}
```

### 3.2. 创建新的服务器类型

### 3.3. 创建新的消息类型

### 3.4. 客户端连接服务器
