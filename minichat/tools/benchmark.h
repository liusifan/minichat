
#pragma once

#include <vector>

typedef struct _Op {
    int duration_sec;
    double begin_qps;
    double end_qps;
    double qps_variation;
    int qps_var_interval_sec;
    int qps_var_direction;
} Op;

typedef struct tagBMArgs {
    int begin_index;
    int thread_count;
    int uthread_per_thread;
    int user_per_uthread;
    int msg_per_user;

    int begin_qps_per_uthread;
    int max_qps_per_uthread;
    int qps_time_interval_per_uthread;
    int qps_increment_per_uthread;

    bool auth_use_rsa;

    std::vector<Op> op_vec;
} BMArgs_t;

extern int benchmark( const BMArgs_t & args );

