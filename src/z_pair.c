#include "z_tool.h"
#include "z_debug.h"
#include "z_pair.h"

/**
 * @brief Create a new z_pair instance
 * @return Pointer to the new z_pair instance       
 */
struct z_pair* z_pair_create(void) {
    struct z_pair* pair = z_tool_malloc(sizeof(struct z_pair));
    if (!pair) return NULL;

    int fds[2];
    if (socketpair(AF_UNIX, SOCK_STREAM, 0, fds) == -1) {
        z_tool_free(pair);
        return NULL;
    }

    // Set to non-blocking mode
    fcntl(fds[0], F_SETFL, O_NONBLOCK);
    fcntl(fds[1], F_SETFL, O_NONBLOCK);

    pair->read_fd = fds[0];
    pair->write_fd = fds[1];
    pair->signaled = 0;

    pthread_mutex_init(&pair->mtx, NULL);
    return pair;
}

/**
 * @brief Trigger wakeup for the z_pair instance
 * @param pair Pointer to the z_pair instance
 */
void z_pair_signal(struct z_pair* pair) {
    pthread_mutex_lock(&pair->mtx);
    
    if (!pair->signaled) {
        char byte = 0;
        // Write trigger byte (ignore failure)
        if (write(pair->write_fd, &byte, 1) == 1) {
            pair->signaled = 1;
        }
    }
    
    pthread_mutex_unlock(&pair->mtx);
}

/**
 * @brief Wait for an event on the z_pair instance
 * @param pair Pointer to the z_pair instance
 * @param timeout_ms Timeout in milliseconds
 * @return 1 if an event occurred, 0 on timeout, -1 on error
 */
int z_pair_wait(struct z_pair* pair, int timeout_ms) {
    struct timeval tv = {
        .tv_sec = timeout_ms / 1000,
        .tv_usec = (timeout_ms % 1000) * 1000
    };

    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(pair->read_fd, &fds);

    // Socket event detection
    int ret = select(pair->read_fd + 1, &fds, NULL, NULL, 
                   (timeout_ms >= 0) ? &tv : NULL);
    if (ret <= 0) return ret;

    // Read data and reset state
    pthread_mutex_lock(&pair->mtx);
    char buf[16];
    while (read(pair->read_fd, buf, sizeof(buf)) > 0);
    pair->signaled = 0;
    pthread_mutex_unlock(&pair->mtx);

    return 1;
}

/**
 * @brief Destroy the z_pair instance and free resources
 * @param pair Pointer to the z_pair instance
 */
void z_pair_destroy(struct z_pair* pair) {
    if (!pair) return;

    close(pair->read_fd);
    close(pair->write_fd);
    
    pthread_mutex_destroy(&pair->mtx);
    z_tool_free(pair);
}
