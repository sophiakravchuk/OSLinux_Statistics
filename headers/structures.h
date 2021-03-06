#ifndef OS_STATISTIC_STRUCTURES_H
#define OS_STATISTIC_STRUCTURES_H

#include <string>
#include <vector>

struct sorting_info {
    int name;
    int pid;
    int state;
    int cpu_u;
    int virt;
    int pss;
    int mem_p;
};


struct active_window {
    int processes;
    int cpu;
    int memory;
    int about_us;
    int graph_proc;
    int help;
};


struct task_manager_file_info {
    int pid;
    std::string process_name;
    double cpu_usage;
    long mem_percentage;
    long pss;
    long vm_size;
    std::string proc_state;
};

struct constant_info {
    std::string cpu_name;
    int cpu_cores;
};

#endif //OS_STATISTIC_STRUCTURES_H
