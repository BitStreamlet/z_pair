# z_pair

A lightweight and efficient inter-thread notification library based on Unix domain sockets for Linux systems.

## ⚡ Introduction

**This is a simple Linux inter-thread notification library that provides a reliable mechanism for thread synchronization using Unix domain sockets.** 😊

## 💻 How It Works
The library uses Unix domain socket pairs to implement a non-blocking notification mechanism between threads. When a signal is sent, it writes a byte to the socket, and waiting threads can efficiently detect this signal using select().

## 🚀 Getting Started

### **Build**

```bash
make        # Build the library and test program
make clean  # Clean build files
```

### **Test**
Run the test program to see the notification mechanism in action:
```bash
./build/test
```

The test program demonstrates:
- Creating a z_pair instance
- Multiple threads waiting for signals with different timeouts
- Signaling between threads
- Proper cleanup of resources

## 📚 Usage

### **Basic Example**
```c
#include "z_pair.h"
#include <pthread.h>

void* worker_thread(void* arg) {
    struct z_pair* pair = (struct z_pair*)arg;
    
    // Wait for signal with 1000ms timeout
    int result = z_pair_wait(pair, 1000);
    if (result == 1) {
        printf("Signal received!\n");
    } else if (result == 0) {
        printf("Timeout occurred\n");
    } else {
        printf("Error occurred\n");
    }
    return NULL;
}

int main() {
    // Create a z_pair instance
    struct z_pair* pair = z_pair_create();
    if (!pair) {
        printf("Failed to create z_pair\n");
        return -1;
    }

    // Create a worker thread
    pthread_t thread;
    pthread_create(&thread, NULL, worker_thread, pair);

    // Signal the worker thread after some time
    usleep(500 * 1000);  // 500ms delay
    z_pair_signal(pair);

    // Wait for thread to complete
    pthread_join(thread, NULL);

    // Cleanup
    z_pair_destroy(pair);
    return 0;
}
```

## 🛠️ API Reference

### z_pair_create
```c
struct z_pair* z_pair_create(void);
```
Creates a new z_pair instance for thread notification.
- Returns: Pointer to z_pair structure, or NULL on failure

### z_pair_signal
```c
void z_pair_signal(struct z_pair* pair);
```
Sends a signal to wake up any threads waiting on this z_pair instance.
- Thread-safe: Yes
- Multiple signals are coalesced into one

### z_pair_wait
```c
int z_pair_wait(struct z_pair* pair, int timeout_ms);
```
Waits for a signal on the z_pair instance.
- `timeout_ms`: Timeout in milliseconds (-1 for infinite wait)
- Returns:
  - 1: Signal received
  - 0: Timeout
  - -1: Error occurred

### z_pair_destroy
```c
void z_pair_destroy(struct z_pair* pair);
```
Cleans up and frees all resources associated with the z_pair instance.

## 🔍 Features
- Non-blocking notification mechanism
- Support for timeouts
- Thread-safe operations
- Efficient signal coalescing
- Clean resource management
- Minimal system resource usage

## 📋 Implementation Details
The library uses:
- Unix domain sockets for efficient inter-thread communication
- Non-blocking I/O for better performance
- Mutex protection for thread safety
- Select() for timeout support
- Signal coalescing to prevent overwhelming

## 🤝 Contributing
Contributions are welcome! Feel free to submit issues and pull requests.

## 📄 License
This project is open source and available under the MIT License.