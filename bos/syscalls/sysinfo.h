#ifndef SYSNAMES_H
#define SYSNAMES_H

#include <sys/syscall.h>
#include <unistd.h>

const char *get_syscall_name(int sys_num);
int is_ptr_syscall(int sys_num);
int is_err_syscall(int sys_num, long long ret);

#endif
