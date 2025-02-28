# z_pair

一个基于Unix域套接字的轻量级高效线程通知库，专为Linux系统设计。

## ⚡ 简介

**这是一个简单的Linux线程通知库，使用Unix域套接字提供可靠的线程同步机制。** 😊

## 💻 工作原理
该库使用Unix域套接字对来实现线程间的非阻塞通知机制。当发送信号时，会向套接字写入一个字节，等待的线程可以通过select()高效地检测这个信号。

## 🚀 快速开始

### **构建**

```bash
make        # 构建库和测试程序
make clean  # 清理构建文件
```

### **测试**
运行测试程序来查看通知机制的运行情况：
```bash
./build/test
```

测试程序演示了：
- 创建z_pair实例
- 多个线程以不同超时时间等待信号
- 线程间的信号传递
- 资源的正确清理

## 📚 使用方法

### **基本示例**
```c
#include "z_pair.h"
#include <pthread.h>

void* worker_thread(void* arg) {
    struct z_pair* pair = (struct z_pair*)arg;
    
    // 等待信号，超时时间为1000毫秒
    int result = z_pair_wait(pair, 1000);
    if (result == 1) {
        printf("收到信号！\n");
    } else if (result == 0) {
        printf("等待超时\n");
    } else {
        printf("发生错误\n");
    }
    return NULL;
}

int main() {
    // 创建z_pair实例
    struct z_pair* pair = z_pair_create();
    if (!pair) {
        printf("创建z_pair失败\n");
        return -1;
    }

    // 创建工作线程
    pthread_t thread;
    pthread_create(&thread, NULL, worker_thread, pair);

    // 延时一段时间后发送信号
    usleep(500 * 1000);  // 延时500毫秒
    z_pair_signal(pair);

    // 等待线程完成
    pthread_join(thread, NULL);

    // 清理资源
    z_pair_destroy(pair);
    return 0;
}
```

## 🛠️ API参考

### z_pair_create
```c
struct z_pair* z_pair_create(void);
```
创建新的z_pair实例用于线程通知。
- 返回值：指向z_pair结构的指针，失败时返回NULL

### z_pair_signal
```c
void z_pair_signal(struct z_pair* pair);
```
发送信号以唤醒等待在此z_pair实例上的线程。
- 线程安全：是
- 多个信号会合并为一个

### z_pair_wait
```c
int z_pair_wait(struct z_pair* pair, int timeout_ms);
```
在z_pair实例上等待信号。
- `timeout_ms`：超时时间（毫秒），-1表示无限等待
- 返回值：
  - 1：收到信号
  - 0：超时
  - -1：发生错误

### z_pair_destroy
```c
void z_pair_destroy(struct z_pair* pair);
```
清理并释放与z_pair实例关联的所有资源。

## 🔍 特性
- 非阻塞通知机制
- 支持超时设置
- 线程安全操作
- 高效的信号合并
- 清晰的资源管理
- 最小化系统资源使用

## 📋 实现细节
该库使用：
- Unix域套接字实现高效的线程间通信
- 非阻塞I/O提升性能
- 互斥锁保证线程安全
- Select()支持超时机制
- 信号合并防止过载

## 🤝 贡献
欢迎贡献！随时提交问题和拉取请求。

## 📄 许可证
本项目采用MIT许可证开源。
