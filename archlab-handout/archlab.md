# archlab

## 环境准备

按照教程解压`sim.tar`文件，进入`sim`文件夹，执行命令

```bash
cd sim
make clean; make
```

多重定义报错：

```bash
multiple definition of `lineno'; yas-grammar.o:(.bss+0x0): first defined here
```

按照[问题描述](https://stackoverflow.com/questions/63152352/fail-to-compile-the-y86-simulatur-csapp)，这个问题是

> I had the same problem, it is about gcc. gcc-10 changed default from "-fcommon" to "-fno-common". You need to add "-fcommon" flag to Makefiles.
>
> Old misc/Makefile:
>
> ```c
> CFLAGS=-Wall -O1 -g
> LCFLAGS=-O1
> ```
>
> New misc/Makefile:
>
> ```c
> CFLAGS=-Wall -O1 -g -fcommon
> LCFLAGS=-O1 -fcommon
> ```

具体的报错原因在[链接](https://gcc.gnu.org/gcc-10/porting_to.html)，总结就是：

在头文件中省略关键字`extern`来声明一个全局变量，这样会导致多个文件使用这个全局变量的时候，会产生“多重定义”的错误。之前的`GCC`版本这个错误会被忽视，然后现在`GCC10`默认用`-fno-common`。而这个`-fno-common`命令的加入，使得之前版本忽视的“多重定义”的链接错误会被展示出来。

解决方案有两个：

1. 在头文件中用关键字`extern`来声明全局变量，并且保证这个声明仅在一个头文件中
2. 所有暂时定义的变量都在一个代码块中的历史C代码，可以用`-fcommon`来编译

然后去找`/misc`下的`Makefile`，修改编译选项，返回上一层`sim`，执行`make`，继续报错：

```bash
gcc -Wall -O2  -isystem /usr/include/tcl8.5 -I../misc  -o psim psim.c pipe-std.c \
        ../misc/isa.c -L/usr/lib -ltk -ltcl -lm
/usr/bin/ld: /tmp/ccAsZkLA.o:(.bss+0x0): multiple definition of `mem_wb_state'; /tmp/cclcvzbD.o:(.bss+0x120): first defined here
/usr/bin/ld: /tmp/ccAsZkLA.o:(.bss+0x8): multiple definition of `ex_mem_state'; /tmp/cclcvzbD.o:(.bss+0x128): first defined here
/usr/bin/ld: /tmp/ccAsZkLA.o:(.bss+0x10): multiple definition of `id_ex_state'; /tmp/cclcvzbD.o:(.bss+0x130): first defined here
/usr/bin/ld: /tmp/ccAsZkLA.o:(.bss+0x18): multiple definition of `if_id_state'; /tmp/cclcvzbD.o:(.bss+0x138): first defined here
/usr/bin/ld: /tmp/ccAsZkLA.o:(.bss+0x20): multiple definition of `pc_state'; /tmp/cclcvzbD.o:(.bss+0x140): first defined here
/usr/bin/ld: cannot find -ltk: No such file or directory
/usr/bin/ld: cannot find -ltcl: No such file or directory
collect2: error: ld returned 1 exit status
make[1]: *** [Makefile:44: psim] Error 1
make[1]: Leaving directory '/home/crx/study/csapp/archlab-handout/sim/pipe'
make: *** [Makefile:28: all] Error 2
```

报错分为两部分：变量多重定义错误和库文件缺失错误。

**库文件缺失错误**

```bash
/usr/bin/ld: cannot find -ltk: No such file or directory
/usr/bin/ld: cannot find -ltcl: No such file or directory
```

这些错误表示链接器找不到 `tk` 和 `tcl` 库，可以使用以下命令安装这些库：

```bash
sudo apt-get update
sudo apt-get install tk-dev tcl-dev
```

**多重定义报错**

```bash
/usr/bin/ld: /tmp/ccAsZkLA.o:(.bss+0x0): multiple definition of `mem_wb_state'; /tmp/cclcvzbD.o:(.bss+0x120): first defined here
/usr/bin/ld: /tmp/ccAsZkLA.o:(.bss+0x8): multiple definition of `ex_mem_state'; /tmp/cclcvzbD.o:(.bss+0x128): first defined here
/usr/bin/ld: /tmp/ccAsZkLA.o:(.bss+0x10): multiple definition of `id_ex_state'; /tmp/cclcvzbD.o:(.bss+0x130): first defined here
/usr/bin/ld: /tmp/ccAsZkLA.o:(.bss+0x18): multiple definition of `if_id_state'; /tmp/cclcvzbD.o:(.bss+0x138): first defined here
/usr/bin/ld: /tmp/ccAsZkLA.o:(.bss+0x20): multiple definition of `pc_state'; /tmp/cclcvzbD.o:(.bss+0x140): first defined here
```

这个问题与第一个问题雷同，这里不再赘述。

解决完毕后，即可编译成功。

:red_circle: 这里总结下环境布置时候遇到的问题和解决方案：

:one: **库文件缺失错误**

解决方案：

```bash
sudo apt-get update
sudo apt-get install tk-dev tcl-dev
```

:two: **多重定义报错**

解决方案：将`misc`和`pipe`下的`Makefile`中的

```bash
CFLAGS=-Wall -O1 -g
LCFLAGS=-O1
```

改为：

```bash
CFLAGS=-Wall -O1 -g -fcommon
LCFLAGS=-O1 -fcommon
```

## part A

即将`examples.c`的C函数转换为Y86-64的汇编语言，语法格式同X86-64。这里粘贴下`examples.c`的代码：

```C
/* 
 * Architecture Lab: Part A 
 * 
 * High level specs for the functions that the students will rewrite
 * in Y86-64 assembly language
 */

/* $begin examples */
/* linked list element */
typedef struct ELE {
    long val;
    struct ELE *next;
} *list_ptr;

/* sum_list - Sum the elements of a linked list */
long sum_list(list_ptr ls)
{
    long val = 0;
    while (ls) {
	val += ls->val;
	ls = ls->next;
    }
    return val;
}

/* rsum_list - Recursive version of sum_list */
long rsum_list(list_ptr ls)
{
    if (!ls)
	return 0;
    else {
	long val = ls->val;
	long rest = rsum_list(ls->next);
	return val + rest;
    }
}

/* copy_block - Copy src to dest and return xor checksum of src */
long copy_block(long *src, long *dest, long len)
{
    long result = 0;
    while (len > 0) {
	long val = *src++;
	*dest++ = val;
	result ^= val;
	len--;
    }
    return result;
}
/* $end examples */
```

:one:**实现链表元素的求和**

目的：将函数`sum_list`转换为y86格式的汇编代码，并且进行测试。

链表元素求和函数`sum_list`原型：

```C
/* sum_list - Sum the elements of a linked list */
long sum_list(list_ptr ls)
{
    long val = 0;
    while (ls) {
	val += ls->val;
	ls = ls->next;
    }
    return val;
}
```

测试的链表数据：

```assembly
# Sample linked list
	.align 8
ele1:
	.quad 0x00a
	.quad ele2
ele2:
	.quad 0x0b0
	.quad ele3
ele3:
	.quad 0xc00
	.quad 0
```

既然让按照X86-64格式仿写Y86-64代码，那就先将代码编译为X86形式的汇编代码，转换命令：

```bash
gcc -S examples.c
```

查看`sum_list`X86格式的汇编代码：

```assembly
	.file	"examples.c"
	.text
	.globl	sum_list
	.type	sum_list, @function
sum_list:
.LFB0:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	movq	%rdi, -24(%rbp)
	movq	$0, -8(%rbp)
	jmp	.L2
.L3:
	movq	-24(%rbp), %rax
	movq	(%rax), %rax
	addq	%rax, -8(%rbp)
	movq	-24(%rbp), %rax
	movq	8(%rax), %rax
	movq	%rax, -24(%rbp)
.L2:
	cmpq	$0, -24(%rbp)
	jne	.L3
	movq	-8(%rbp), %rax
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE0:
	.size	sum_list, .-sum_list
	.globl	rsum_list
	.type	rsum_list, @function
```

仿照`/y86-code/asum.ys`的原型写`sum.ys`：

```assembly
# Execution begins at address 0 
	.pos 0
	irmovq stack, %rsp  	# Set up stack pointer
	call main		# Execute main program
	halt			# Terminate program 

# Sample linked list
	.align 8
ele1:
	.quad 0x00a
	.quad ele2
ele2:
	.quad 0x0b0
	.quad ele3
ele3:
	.quad 0xc00
	.quad 0

main:	
	irmovq ele1,%rdi
	call sum_list		# sum_list(ele1)
	ret

# long sum_list(list_ptr ls)
# start ls %rdi
sum_list:
	irmovq $8, %r8       # Constant 8
	xorq %rax, %rax		 # val = 0
	andq %rdi, %rdi		 # ls null? Set CC
	jmp     test         # Goto test
loop:
	mrmovq	(%rdi), %r9	 # Get ls->val
	addq %r9, %rax       # Add to sum
	mrmovq 8(%rdi), %rdi # Get ls->next
	andq %rdi, %rdi		 # ls null? Set CC
test:
	jne    loop          # Stop when ls is null
	ret
	
# Stack starts here and grows to lower addresses
	.pos 0x100
stack:
```

这里记录下遇到的问题：

> **指针的指针困惑**
>
> 在`loop`处，获取`ls->val`的方法现在用的是指令：
>
> ```assembly
> mrmovq 8(%rdi), %rdi # Get ls->next
> ```
>
> 此时`%rdi`中保存的是链表第一个元素的地址。
>
> 这个指令是在内存地址`%rdi + 8`处保存的值赋值给`%rdi`。即链表的指针域`8(%rdi)`指的地址赋给`%rdi`，这样`%rdi`获取到了链表下一个元素地址。
>
> 但是一开始我写的是
>
> ```assembly
> iaddq $8,%rdi        # Get ls->next
> ```
>
> 本意也是想获取链表下一个元素的地址。但是这样做`%rdi`会获得下一个元素的地址吗？
>
> 这样是不会的。
>
> 原因：
>
> - `mrmovq 8(%rdi), %rdi`：将内存地址`%rdi + 8`处保存的**下一个元素的地址**赋值给`%rdi`
> - `iaddq $8,%rdi`：相当于将`%rdi`保存的内存地址`+ 8`赋值给`%rdi`。在`%rdi`保存的是链表首元素地址的场景下，指令执行后相当于`%rdi` 指向当前链表节点 `ele1` 的 `next` 字段地址，而不是下一个节点 `ele2`的地址。
>
> 重新理解指针:peach:

:two:**递归求链表元素和**

递归求和函数原型

```C
/* rsum_list - Recursive version of sum_list */
long rsum_list(list_ptr ls)
{
    if (!ls)
	return 0;
    else {
	long val = ls->val;
	long rest = rsum_list(ls->next);
	return val + rest;
    }
}
```

对应的X86汇编代码为

```assembly
rsum_list:
.LFB1:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$32, %rsp
	movq	%rdi, -24(%rbp)
	cmpq	$0, -24(%rbp)
	jne	.L6
	movl	$0, %eax
	jmp	.L7
.L6:
	movq	-24(%rbp), %rax
	movq	(%rax), %rax
	movq	%rax, -16(%rbp)
	movq	-24(%rbp), %rax
	movq	8(%rax), %rax
	movq	%rax, %rdi
	call	rsum_list
	movq	%rax, -8(%rbp)
	movq	-16(%rbp), %rdx
	movq	-8(%rbp), %rax
	addq	%rdx, %rax
.L7:
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE1:
	.size	rsum_list, .-rsum_list
	.globl	copy_block
	.type	copy_block, @function
```

仿照`/y86-code/asum.ys`的原型写`rsum.ys`：

```assembly
# Execution begins at address 0 
	.pos 0
	irmovq stack, %rsp  	# Set up stack pointer
	call main		# Execute main program
	halt			# Terminate program 

# Sample linked list
	.align 8
ele1:
	.quad 0x00a
	.quad ele2
ele2:
	.quad 0x0b0
	.quad ele3
ele3:
	.quad 0xc00
	.quad 0

main:	
	irmovq ele1,%rdi
	call rsum_list		 # rsum_list(ele1)
	ret

/* $begin rsum_list-ys */
# Recursive version of sum_list
# long rsum_list(list_ptr ls)
# ls in %rdi
rsum_list:
	irmovq $8, %r8       # Constant 8
	xorq %rax, %rax		 # Set val = 0
	andq %rdi, %rdi		 # If ls is null, Set CC
	je return			 # If (!ls), return 0
	pushq %rbx			 # Save callee-saved register
	mrmovq (%rdi),%rbx	 # Get *ls
	irmovq $8,%r10
	mrmovq 8(%rdi), %rdi # Get ls->next
	call rsum_list		 # Evaluate rest = rsum_list(ls->next)
	addq %rbx, %rax		 # Add *ls to sum
	popq %rbx 			 # Restore callee-saved register
	
return:
	ret	
/* $end rsum-ys */	

# Stack starts here and grows to lower addresses
	.pos 0x200
stack:

```

> **调用者和被调用者寄存器**
>
> 函数P（caller）在调用函数Q（callee）的时候，Q函数会在运算之前，将P函数可能会用到的寄存器存放到栈中（`%rbx`），然后再去使用这个寄存器进行计算。这个Q函数为了P函数保存的寄存器，就是被调用者寄存器。

:three:**将源链表数据复制到新链表中，并且返回异或源链表所有元素的结果**

```C
/* copy_block - Copy src to dest and return xor checksum of src */
long copy_block(long *src, long *dest, long len)
{
    long result = 0;
    while (len > 0) {
	long val = *src++;
	*dest++ = val;
	result ^= val;
	len--;
    }
    return result;
}
/* $end examples */
```

对应的X86汇编代码为：

```assembly
copy_block:
.LFB2:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	movq	%rdi, -24(%rbp)
	movq	%rsi, -32(%rbp)
	movq	%rdx, -40(%rbp)
	movq	$0, -16(%rbp)
	jmp	.L9
.L10:
	movq	-24(%rbp), %rax
	leaq	8(%rax), %rdx
	movq	%rdx, -24(%rbp)
	movq	(%rax), %rax
	movq	%rax, -8(%rbp)
	movq	-32(%rbp), %rax
	leaq	8(%rax), %rdx
	movq	%rdx, -32(%rbp)
	movq	-8(%rbp), %rdx
	movq	%rdx, (%rax)
	movq	-8(%rbp), %rax
	xorq	%rax, -16(%rbp)
	subq	$1, -40(%rbp)
.L9:
	cmpq	$0, -40(%rbp)
	jg	.L10
	movq	-16(%rbp), %rax
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
```

这里注意，被复制的块和目的块均为**数组**。

对应的Y86格式代码`copy.ys`为:

```assembly
# Execution begins at address 0 
	.pos 0 
	irmovq stack, %rsp  	# Set up stack pointer  
	call main		# Execute main program
	halt			# Terminate program 



# Source block
.align 8
src:
	.quad 0x00a
	.quad 0x0b0
	.quad 0xc00
# Destination block
dest:
	.quad 0x111
	.quad 0x222
	.quad 0x333

main:	irmovq src,%rdi	
	irmovq dest,%rsi
	irmovq $3, %rdx
	call copy_block			# copy_block(src, dest, 3)
	ret	
	
copy_block:	
	xorq %rax, %rax			# Set return value to 0
	andq %rdx, %rdx			# Set Condition codes
	irmovq $8, %r8			# Constant 8
	irmovq $1, %r9          # Constant 1
	jmp test				# Goto test

loop:
	mrmovq (%rdi), %r10		# Set val to *src
	addq %r8,%rdi			# src++
	rmmovq %r10, (%rsi)		# Set *dest = val
	addq %r8, %rsi			# dest++
	xorq %r10, %rax			# result ^= val
	subq %r9, %rdx			# len--

test:
	jg loop				# Stop when len <= 0
	ret


	
# The stack starts here and grows to lower addresses
	.pos 0x300		
stack:	 
```

