#include "z_tool.h"
#include "z_debug.h"
#include "z_pair.h"
#include <pthread.h>

struct thread_arg {
    struct z_pair* pair;
    int timeout_ms;
};

// 线程函数，用于测试z_pair_wait
void* test_z_pair_wait(void* arg) {
    struct thread_arg* t_arg = (struct thread_arg*)arg;
    struct z_pair* pair = t_arg->pair;
    int timeout_ms = t_arg->timeout_ms;

    Z_INFO("Thread: Waiting for the z_pair instance with %dms timeout", timeout_ms);
    int wait_result = z_pair_wait(pair, timeout_ms);
    if (wait_result == 1) {
        Z_INFO("Thread: z_pair_wait succeeded");
    } else if (wait_result == 0) {
        Z_INFO("Thread: z_pair_wait timed out");
    } else {
        Z_ERROR("Thread: z_pair_wait failed");
    }

    return NULL;
}

int main(int argc, char *argv[]) {
    // 创建一个z_pair实例
    struct z_pair *pair = z_pair_create();
    if (pair == NULL) {
        Z_ERROR("Failed to create z_pair instance");
        return -1;
    }

    // 创建一个线程来测试z_pair_wait，设置超时时间为500ms
    pthread_t thread1;
    struct thread_arg arg1 = {pair, 500};
    if (pthread_create(&thread1, NULL, test_z_pair_wait, &arg1) != 0) {
        Z_ERROR("Failed to create thread");
        z_pair_destroy(pair);
        return -1;
    }

    // 创建另一个线程来测试z_pair_wait，设置超时时间为1500ms
    pthread_t thread2;
    struct thread_arg arg2 = {pair, 1500};
    if (pthread_create(&thread2, NULL, test_z_pair_wait, &arg2) != 0) {
        Z_ERROR("Failed to create thread");
        z_pair_destroy(pair);
        return -1;
    }

    // 主线程中触发信号
    Z_INFO("Main thread: Signaling the z_pair instance after 1000ms");
    usleep(1000 * 1000); // 等待1秒后触发信号
    z_pair_signal(pair);

    // 等待子线程完成
    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);

    // 释放z_pair实例
    Z_INFO("Main thread: Destroying the z_pair instance");
    z_pair_destroy(pair);
    z_tool_malloc_info();
    return 0;
}