
#pragma once

extern int benchmark( int begin_index, int thread_count,
        int uthread_per_thread, int msg_per_user,
        int begin_qps_per_uthread, int max_qps_per_uthread, int qps_time_interval_per_uthread,
        int qps_increment_per_uthread  );
