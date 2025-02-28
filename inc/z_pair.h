#ifndef _Z_PAIR_H_
#define _Z_PAIR_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
struct z_pair {
    int read_fd;       
    int write_fd;         
    pthread_mutex_t mtx;   
    volatile int signaled; 
};

struct z_pair* z_pair_create(void);
void z_pair_signal(struct z_pair* zp);
int z_pair_wait(struct z_pair* zp, int timeout_ms);
void z_pair_destroy(struct z_pair* zp);
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _Z_TOOL_H_ */
