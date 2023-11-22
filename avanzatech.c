#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/errno.h>
#include <linux/string.h>
#include <linux/unistd.h>
#include <linux/linkage.h>

#define INT_16 2                // 2 bytes int
#define INT_32 4                // 4 bytes int
#define MAX_NUM_ALLOWED_16 31   // Max computable cubic for 2 bytes int
#define MAX_NUM_ALLOWED_32 1290 // Max computable cubic for 4 bytes int
#define MAX_NAME_LENGTH 64      // Max name's length allowed

asmlinkage long avanzatech(int number, char __user *buffer, size_t length, char __user *dest_buffer, size_t dest_len)
{

    // Validate pointers to read and write
    if (access_ok(VERIFY_READ, buffer, length) == 0)
    {
        printk(KERN_ERR "Buffer's origin could not be read\n");
        return -EFAULT;
    }
    if (access_ok(VERIFY_WRITE, dest_buffer, dest_len) == 0)
    {
        printk(KERN_ERR "Buffer's destination cannot be modified\n");
        return -EFAULT;
    }

    // Validate buffer sizes
    if (length <= 0 || length > MAX_NAME_LENGTH || length >= dest_len || dest_len <= 0)
    {
        printk(KERN_ERR "Buffer's length invalid\n");
        return -EINVAL;
    }
    // Allocate memory for user
    char *kernel_buffer = (char *)kmalloc(length, GFP_KERNEL);
    if (kernel_buffer == NULL)
    {
        printk(KERN_ERR "Error allocating memory in kernell\n");
        return -ENOMEM;
    }

    // Copy name from user
    if (copy_from_user(kernel_buffer, buffer, length) != 0)
    {
        printk(KERN_ERR "Buffer from user couldn't be copied properly\n");
        kfree(kernel_buffer);
        return -EFAULT;
    }

    // Validate Number Input and compute the number's cube
    int int_length = sizeof(int) == INT_16 ? INT_16 : INT_32;
    int MAX_NUMBER = int_length == INT_16 ? MAX_NUM_ALLOWED_16 : MAX_NUM_ALLOWED_32;
    int MIN_NUMBER = -MAX_NUMBER;
    if (number > MAX_NUMBER || number < MIN_NUMBER)
    {
        printk(KERN_ERR "The number %d is out of valid boundaries\n", number);
        kfree(kernel_buffer);
        return -EINVAL;
    }
    int cube_number = number * number * number;

    // Calculate total length of response
    const char *FORMAT = "Hi %s, the cube of %d is %d\n";
    size_t total_size = snprintf(NULL, 0, FORMAT, kernel_buffer, number, cube_number);
    if (total_size < 0)
    {
        printf(KERN_ERR "Error calculating total size of response\n");
        kfree(kernel_buffer);
        return -EFAULT;
    }

    // If dest_len is too small
    if (total_size > dest_len)
    {
        printk(KERN_ERR "The destination buffer hasn't enough space\n");
        kfree(kernel_buffer);
        return -EFAULT;
    }
    // Allocate memory in kernel for response
    char *kernel_response = (char *)kmalloc(total_size, GFP_KERNEL);
    if (kernel_response == NULL)
    {
        printk(KERN_ERR "Error allocating memory in kernell response\n");
        kfree(kernel_buffer);
        return -ENOMEM;
    }
    // Construct Response Message
    if (snprintf(kernel_response, total_size, FORMAT, kernel_buffer, number, cube_number) < 0)
    {
        printk(KERN_ERR "Error concatenting response in kernell\n");
        kfree(kernel_buffer);
        kfree(kernel_response);
        return -EFAULT;
    }

    // Clean destination buffer
    memset(dest_buffer + total_size, 0, dest_len - total_size);
    // Copy response to user space
    if (copy_to_user(dest_buffer, kernel_response, dest_len) != 0)
    {
        printk(KERN_ERR "Buffer to user couldn't be copied properly\n");
        kfree(kernel_buffer);
        kfree(kernel_response);
        return -EFAULT;
    }
    // Success print statement
    printk(KERN_DEBUG "Message %s copied succesfully in user space\n", kernel_response);
    // free memory allocated
    kfree(kernel_response);
    kfree(kernel_buffer);
    return 0;
}
