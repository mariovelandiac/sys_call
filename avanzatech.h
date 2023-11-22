#ifndef __LINUX_AVANZATECH_H
#define __LINUX_AVANZATECH_H
	
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/errno.h>
#include <linux/string.h>
#include <linux/unistd.h>
#include <linux/linkage.h>

_syscall5(long, avanzatech, int, number, char*, buffer, size_t, length, char*, dest_buffer, size_t, dest_len);

#endif