#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/errno.h>
#include <linux/string.h>
#include <linux/unistd.h>
#include <linux/linkage.h>

asmlinkage long __sys_avanzatech(int number, char __user *name, size_t name_length, char __user *dest_buffer, size_t dest_len)
{
	int cube_number;
	char *kernel_buffer, *kernel_response, *FORMAT;
	size_t total_size;
	
	// Allocate memory for user
	kernel_buffer = (char *)kmalloc(name_length, GFP_KERNEL);
	if (kernel_buffer == NULL)
	{
		printk(KERN_ERR "[avanzatech]: Error allocating memory in kernell\n");
		return -ENOMEM;
	}
	
	printk(KERN_INFO "[avanzatech]: Memory for name allocated");

	// Copy name from user
	if (copy_from_user(kernel_buffer, name, name_length) != 0)
	{
		printk(KERN_ERR "[avanzatech]: Buffer from user couldn't be copied properly\n");
		kfree(kernel_buffer);
		return -EFAULT;
	}
	
	printk(KERN_INFO "[avanzatech]: Copied name to kernel space\n");

	// Compute the number's cube
	cube_number = number * number * number;
	
	printk(KERN_INFO "[avanzatech]: Cube of number %d calculated: %d\n", number, cube_number);

	// Calculate total length of response
	FORMAT = "Hi %s, the cube of %d is %d\n";
	total_size = snprintf(NULL, 0, FORMAT, kernel_buffer, number, cube_number);
	if (total_size < 0)
	{
		printk(KERN_ERR "[avanzatech]: Error calculating total size of response\n");
		kfree(kernel_buffer);
		return -EFAULT;
	}
	
	printk(KERN_INFO "[avanzatech]: total size calculated is %zu", total_size);

	// If dest_len is too small
	if (total_size > dest_len)
	{
		printk(KERN_ERR "[avanzatech]: The destination buffer hasn't enough space\n");
		kfree(kernel_buffer);
		return -EFAULT;
	}
	
	printk(KERN_INFO "[avanzatech]: dest_len has enough space\n");
	
	// Allocate memory in kernel for response
	kernel_response = (char *)kmalloc(dest_len, GFP_KERNEL);
	if (kernel_response == NULL)
	{
		printk(KERN_ERR "[avanzatech]: Error allocating memory in kernell response\n");
		kfree(kernel_buffer);
		return -ENOMEM;
	}
	
	printk(KERN_INFO "[avanzatech]: kernel_response buffer allocated");
	
	
	// Construct Response Message
	if (snprintf(kernel_response, total_size, FORMAT, kernel_buffer, number, cube_number) < 0)
	{
		printk(KERN_ERR "[avanzatech]: Error concatenting response in kernell\n");
		kfree(kernel_buffer);
		kfree(kernel_response);
		return -EFAULT;
	}
	
	printk(KERN_INFO "[avanzatech]: concatening name, number and response success\n");

	// Clean destination buffer
	// memset(dest_buffer + total_size, 0, dest_len - total_size);
	// Copy response to user space
	if (copy_to_user(dest_buffer, kernel_response, dest_len) != 0)
	{
		printk(KERN_ERR "[avanzatech]: Buffer to user couldn't be copied properly\n");
		kfree(kernel_buffer);
		kfree(kernel_response);
		return -EFAULT;
	}
	
	// Success print statement
	printk(KERN_DEBUG "[avanzatech]: Message %s copied succesfully in user space\n", kernel_response);
	// free memory allocated
	kfree(kernel_response);
	kfree(kernel_buffer);
	return 0;
}

SYSCALL_DEFINE5(avanzatech, int, number, char __user *, name, size_t, name_length, char __user *, dest_buffer, size_t, dest_len) {
	int INT_16, INT_32, MAX_NUM_ALLOWED_16, MAX_NUM_ALLOWED_32, MAX_NAME_LENGTH, MAX_NUMBER, MIN_NUMBER;
	int int_length;
	
	printk(KERN_INFO "[avanzatech]: Starting syscall\n");
	
	printk(KERN_INFO "[avanzatech]: number is %d\n", number);
	printk(KERN_INFO "[avanzatech]: name_length is %zu\n", name_length);
	printk(KERN_INFO "[avanzatech]: dest_len is %zu\n", dest_len);	
	
	INT_16 = 2;             // bits in 2 bytes integer
	INT_32 = 4;             // bits in 4 bytes integer
	MAX_NUM_ALLOWED_16 = 31;        // max num argument for cube function with 16 bit int
	MAX_NUM_ALLOWED_32 = 1290;      // max num argument for cube function with 32 bit int
	MAX_NAME_LENGTH = 64;   // max name length allowed

	// Validate buffer sizes
	if (name_length <= 0 || name_length > MAX_NAME_LENGTH || name_length >= dest_len || dest_len <= 0)
	{
		printk(KERN_ERR "[avanzatech]: Buffer's length invalid\n");
		return -EINVAL;
	}
	
	printk(KERN_INFO "[avanzatech]: Buffers size Validation OK\n");
	
	// Validate Number Input
	int_length = sizeof(int) == INT_16 ? INT_16 : INT_32;
	MAX_NUMBER = int_length == INT_16 ? MAX_NUM_ALLOWED_16 : MAX_NUM_ALLOWED_32;
	MIN_NUMBER = -MAX_NUMBER;
	if (number > MAX_NUMBER || number < MIN_NUMBER)
	{
		printk(KERN_ERR "[avanzatech]: The number %d is out of valid boundaries\n", number);
		return -EINVAL;
	}
	
	printk(KERN_INFO "[avanzatech]: Number Validation OK\n");	
	printk(KERN_INFO "[avanzatech]: Calling __sys_avanzatech\n");	
	
	return __sys_avanzatech(number, name, name_length, dest_buffer, dest_len);
}

/*
// This works 
SYSCALL_DEFINE5(avanzatech, int, number, char __user *, name, size_t, name_length, char __user *, dest_buffer, size_t, dest_len) {
	int INT_16, INT_32, MAX_NUM_ALLOWED_16, MAX_NUM_ALLOWED_32, MAX_NAME_LENGTH, MAX_NUMBER, MIN_NUMBER;
	int int_length, cube_number;
	char *kernel_buffer, *kernel_response, *FORMAT;
	size_t total_size;

	printk(KERN_INFO "[avanzatech]: Starting syscall\n");
	printk(KERN_INFO "[avanzatech]: number is %d\n", number);
	printk(KERN_INFO "[avanzatech]: name_length is %zu\n", name_length);
	printk(KERN_INFO "[avanzatech]: dest_len is %zu\n", dest_len);	

	INT_16 = 2;             // bits in 2 bytes integer
	INT_32 = 4;             // bits in 4 bytes integer
	MAX_NUM_ALLOWED_16 = 31;        // max num argument for cube function with 16 bit int
	MAX_NUM_ALLOWED_32 = 1290;      // max num argument for cube function with 32 bit int
	MAX_NAME_LENGTH = 64;   // max name length allowed

	// Validate buffer sizes
	if (name_length <= 0 || name_length > MAX_NAME_LENGTH || name_length >= dest_len || dest_len <= 0)
	{
		printk(KERN_ERR "Buffer's length invalid\n");
		return -EINVAL;
	}
	
	printk(KERN_INFO "[avanzatech]: Number Validation OK");
    
	// Allocate memory for user
	kernel_buffer = (char *)kmalloc(name_length, GFP_KERNEL);
	if (kernel_buffer == NULL)
	{
		printk(KERN_ERR "Error allocating memory in kernell\n");
		return -ENOMEM;
	}
	
	printk(KERN_INFO "[avanzatech]: Memory Allocated");

	// Copy name from user
	if (copy_from_user(kernel_buffer, name, name_length) != 0)
	{
		printk(KERN_ERR "Buffer from user couldn't be copied properly\n");
		kfree(kernel_buffer);
		return -EFAULT;
	}
	
	printk(KERN_INFO "[avanzatech]: Copied name to kernel space\n");

	// Validate Number Input and compute the number's cube

	int_length = sizeof(int) == INT_16 ? INT_16 : INT_32;
	MAX_NUMBER = int_length == INT_16 ? MAX_NUM_ALLOWED_16 : MAX_NUM_ALLOWED_32;
	MIN_NUMBER = -MAX_NUMBER;
	if (number > MAX_NUMBER || number < MIN_NUMBER)
	{
		printk(KERN_ERR "The number %d is out of valid boundaries\n", number);
		kfree(kernel_buffer);
		return -EINVAL;
		}
	cube_number = number * number * number;
	
	printk(KERN_INFO "[avanzatech]: Cube of number %d calculated: %d\n", number, cube_number);

	// Calculate total length of response

	FORMAT = "Hi %s, the cube of %d is %d\n";
	total_size = snprintf(NULL, 0, FORMAT, kernel_buffer, number, cube_number);
	if (total_size < 0)
	{
		printk(KERN_ERR "Error calculating total size of response\n");
		kfree(kernel_buffer);
		return -EFAULT;
	}
	
	printk(KERN_INFO "[avanzatech]: total size calculated is %zu", total_size);

	// If dest_len is too small
	if (total_size > dest_len)
	{
		printk(KERN_ERR "The destination buffer hasn't enough space\n");
		kfree(kernel_buffer);
		return -EFAULT;
	}
	
	printk(KERN_INFO "[avanzatech]: dest_len has enough space\n");
	
	// Allocate memory in kernel for response
	kernel_response = (char *)kmalloc(dest_len, GFP_KERNEL);
	if (kernel_response == NULL)
	{
		printk(KERN_ERR "Error allocating memory in kernell response\n");
		kfree(kernel_buffer);
		return -ENOMEM;
	}
	
	printk(KERN_INFO "[avanzatech]: kernel_response buffer allocated");
	
	
	// Construct Response Message
	if (snprintf(kernel_response, total_size, FORMAT, kernel_buffer, number, cube_number) < 0)
	{
		printk(KERN_ERR "Error concatenting response in kernell\n");
		kfree(kernel_buffer);
		kfree(kernel_response);
		return -EFAULT;
	}
	
	printk(KERN_INFO "[avanzatech]: concatening name, number and response success\n");

	// Clean destination buffer
	// memset(dest_buffer + total_size, 0, dest_len - total_size);
	// Copy response to user space
	if (copy_to_user(dest_buffer, kernel_response, dest_len) != 0)
	{
		printk(KERN_ERR "Buffer to user couldn't be copied properly\n");
		kfree(kernel_buffer);
		kfree(kernel_response);
		return -EFAULT;
	}
	
	// Success print statement
	printk(KERN_INFO "[avanzatech]: Message %s copied succesfully in user space\n", kernel_response);
	// free memory allocated
	kfree(kernel_response);
	kfree(kernel_buffer);
	return 0;
}

*/
