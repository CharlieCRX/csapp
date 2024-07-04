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

## part B

这次我们要开始对SEQ（sequential Processor）进行下手了。

partB的主要任务就是要在`sim/seq/seq-full.hcl`进行修改指令`IIADDQ`，使得处理器支持`iaddq`指令。

这个指令的具体描述在课本的课后问题4.51和4.52中，现在看下具体功能是什么：

> 4.51
>
> Practice Problem 4.3 introduced the `iaddq` instruction to add immediate data to a
> register. Describe the computations performed to implement this instruction. Use
> the computations for `irmovq` and `OPq` (Figure 4.18) as a guide.
>
> 4.52
>
> The file `seq-full.hcl` contains the HCL description for SEQ, along with the
> declaration of a constant `IIADDQ` having hexadecimal value C, the instruction code
> for `iaddq`. Modify the HCL descriptions of the control logic blocks to implement
> the iaddq instruction, as described in Practice Problem 4.3 and Problem 4.51. See
> the lab material for directions on how to generate a simulator for your solution
> and how to test it.

这里告诉我们的信息：

- `iaddq`：the instruction to add immediate data to a
  register
- `seq-full.hcl` ：contains the HCL description for SEQ
- lab material for directions： how to generate a simulator for your solution
  and how to test it.

首先写出这个`iaddq`指令在SEQ下具体每个阶段的计算过程（参考Figure 4.18）

| Stage      | iadd V, rB                                                   |
| ---------- | ------------------------------------------------------------ |
| Fetch      | icode:ifun ← M₁[PC]<br/>rA:rB ← M₁[PC + 1]<br/>valC ← M₈[PC + 2]<br />valP ← PC + 10 |
| Decode     | valB ← R[rB]                                                 |
| Execute    | valE ← valB + valC<br/>Set CC                                |
| Memory     |                                                              |
| Write back | R[rB] ← valE                                                 |
| PC update  | PC ← valP                                                    |

这样我们就可以修改`seq-full.hcl`中对应阶段的内容了：

1. 取指阶段：指令有效，需要寄存器，需要常数值

   ```c
   bool instr_valid = icode in
       { INOP, IHALT, IRRMOVQ, IIRMOVQ, IRMMOVQ, IMRMOVQ,
              IOPQ, IJXX, ICALL, IRET, IPUSHQ, IPOPQ, IIADDQ };
   ```

   ```C
   bool need_regids =
       icode in { IRRMOVQ, IOPQ, IPUSHQ, IPOPQ,
                IIRMOVQ, IRMMOVQ, IMRMOVQ, IIADDQ };
   ```

   ```c
   bool need_valC =
       icode in { IIRMOVQ, IRMMOVQ, IMRMOVQ, IJXX, ICALL, IIADDQ };
   ```

2. 译码和写回：从rB读取valB，并且将计算值valE写回到rB

   ```c
   word srcB = [
       icode in { IOPQ, IRMMOVQ, IMRMOVQ, IIADDQ } : rB;
       icode in { IPUSHQ, IPOPQ, ICALL, IRET } : RRSP;
       1 : RNONE;  # Don't need register
   ];
   ```

   ```C
   word dstE = [
       icode in { IRRMOVQ } && Cnd : rB;
       icode in { IIRMOVQ, IOPQ, IIADDQ} : rB;
       icode in { IPUSHQ, IPOPQ, ICALL, IRET } : RRSP;
       1 : RNONE;  # Don't write any register
   ];
   ```

   

3. 执行：计算valB和valC的和，并且设置CC

   ```c
   word aluA = [
       icode in { IRRMOVQ, IOPQ } : valA;
       icode in { IIRMOVQ, IRMMOVQ, IMRMOVQ, IIADDQ } : valC;
       icode in { ICALL, IPUSHQ } : -8;
       icode in { IRET, IPOPQ } : 8;
       # Other instructions don't need ALU
   ];
   
   ## Select input B to ALU
   word aluB = [
       icode in { IRMMOVQ, IMRMOVQ, IOPQ, ICALL,
                 IPUSHQ, IRET, IPOPQ, IIADDQ } : valB;
       icode in { IRRMOVQ, IIRMOVQ } : 0;
       # Other instructions don't need ALU
   ];
   ```

   ```c
   ## Should the condition codes be updated?
   bool set_cc = icode in { IOPQ, IIADDQ };
   ```

修改`seq-full.hcl`文件完毕后，就需要用SEQ模拟器来测试下结果了。以下是测试步骤：

- 创建模拟器：

  ```bash
  make VERSION=full
  ```

  创建时候报错：

  ```bash
  crx@ubuntu:seq$ make VERSION=full
  # Building the seq-full.hcl version of SEQ
  ../misc/hcl2c -n seq-full.hcl <seq-full.hcl >seq-full.c
  gcc -Wall -O2 -isystem /usr/include/tcl8.5 -I../misc -DHAS_GUI -o ssim \
          seq-full.c ssim.c ../misc/isa.c -L/usr/lib -ltk -ltcl -lm
  ssim.c:20:10: fatal error: tk.h: No such file or directory
     20 | #include <tk.h>
        |          ^~~~~~
  compilation terminated.
  make: *** [Makefile:44: ssim] Error 1
  ```

  看了[前辈帖子](https://lincx-911.github.io/2021/12/architecture_lab/)的回答，解决方案如下

  ```bash
  sed -i "s/tcl8.5/tcl8.6/g" Makefile                     #将 Makefile 文件中所有出现的 tcl8.5 替换为 tcl8.6。
  sed -i "s/CFLAGS=/CFLAGS=-DUSE_INTERP_RESULT /g" Makefile 
  ```

  然后继续make，报错：

  ```bash
  /usr/bin/ld: /tmp/ccD1lDwT.o:(.data.rel+0x0): undefined reference to `matherr'
  collect2: error: ld returned 1 exit status
  make: *** [Makefile:44: ssim] Error 1
  ```

  解决undefined reference to `matherr`：我们可以在`ssim.c`找到`matherr`,然后注释掉那两行，因为没有用到。

  重新编译，结果ok。

- 用写好的模拟程序来执行含有`iaddq`指令的程序：现在在`y86-code/`下，有个文件`asumi.ys`，其功能是计算数组的和，并且用的求和指令为`iaddq`，最终结果是`%rax = 0xabcdabcdabcd`，`asumi.ys`源码如下

  ```assembly
  # Execution begins at address 0 
  	.pos 0 
  	irmovq stack, %rsp  	# Set up stack pointer  
  	call main		# Execute main program
  	halt			# Terminate program 
  
  # Array of 4 elements
  	.align 8 	
  array:	.quad 0x000d000d000d
  	.quad 0x00c000c000c0
  	.quad 0x0b000b000b00
  	.quad 0xa000a000a000
  
  main:	irmovq array,%rdi	
  	irmovq $4,%rsi
  	call sum		# sum(array, 4)
  	ret 
  
  /* $begin sumi-ys */
  # long sum(long *start, long count)
  # start in %rdi, count in %rsi
  sum:
  	xorq %rax,%rax		# sum = 0
  	andq %rsi,%rsi		# Set condition codes
  	jmp    test
  loop:
  	mrmovq (%rdi),%r10	# Get *start
  	addq %r10,%rax          # Add to sum
  	iaddq $8,%rdi           # start++
  	iaddq $-1,%rsi          # count--
  test:
  	jne    loop             # Stop when 0
  	ret
  /* $end sumi-ys */
  
  # The stack starts here and grows to lower addresses
  	.pos 0x100		
  stack:	 
  ```

  这样用对应`asumi.ys`编译成`asumi.yo`后，用SEQ模拟器执行查看结果：

  ```bash
  ./ssim -t ../y86-code/asumi.yo
  ```

  执行结果：

  ```bash
  
  32 instructions executed
  Status = HLT
  Condition Codes: Z=1 S=0 O=0
  Changed Register State:
  %rax:   0x0000000000000000      0x0000abcdabcdabcd
  %rsp:   0x0000000000000000      0x0000000000000100
  %rdi:   0x0000000000000000      0x0000000000000038
  %r10:   0x0000000000000000      0x0000a000a000a000
  Changed Memory State:
  0x00f0: 0x0000000000000000      0x0000000000000055
  0x00f8: 0x0000000000000000      0x0000000000000013
  ISA Check Succeeds
  ```

  可以看到`%rax`为预测的`0x0000abcdabcdabcd`，并且输出结果为`ISA Check Succeeds`，所以可以进行下一步。

- 基准程序测试

  因为上面的测试程序已经验证结果没问题，现在就要进行基准程序的测试了：

  ```bash
  unix> (cd ../y86-code; make testssim)
  ```

  结果：

  ```bash
  crx@ubuntu:seq$ (cd ../y86-code; make testssim)
  Makefile:42: warning: ignoring prerequisites on suffix rule definition
  Makefile:45: warning: ignoring prerequisites on suffix rule definition
  Makefile:48: warning: ignoring prerequisites on suffix rule definition
  Makefile:51: warning: ignoring prerequisites on suffix rule definition
  ../seq/ssim -t asum.yo > asum.seq
  ../seq/ssim -t asumr.yo > asumr.seq
  ../seq/ssim -t cjr.yo > cjr.seq
  ../seq/ssim -t j-cc.yo > j-cc.seq
  ../seq/ssim -t poptest.yo > poptest.seq
  ../seq/ssim -t pushquestion.yo > pushquestion.seq
  ../seq/ssim -t pushtest.yo > pushtest.seq
  ../seq/ssim -t prog1.yo > prog1.seq
  ../seq/ssim -t prog2.yo > prog2.seq
  ../seq/ssim -t prog3.yo > prog3.seq
  ../seq/ssim -t prog4.yo > prog4.seq
  ../seq/ssim -t prog5.yo > prog5.seq
  ../seq/ssim -t prog6.yo > prog6.seq
  ../seq/ssim -t prog7.yo > prog7.seq
  ../seq/ssim -t prog8.yo > prog8.seq
  ../seq/ssim -t ret-hazard.yo > ret-hazard.seq
  grep "ISA Check" *.seq
  asum.seq:ISA Check Succeeds
  asumr.seq:ISA Check Succeeds
  cjr.seq:ISA Check Succeeds
  j-cc.seq:ISA Check Succeeds
  poptest.seq:ISA Check Succeeds
  prog1.seq:ISA Check Succeeds
  prog2.seq:ISA Check Succeeds
  prog3.seq:ISA Check Succeeds
  prog4.seq:ISA Check Succeeds
  prog5.seq:ISA Check Succeeds
  prog6.seq:ISA Check Succeeds
  prog7.seq:ISA Check Succeeds
  prog8.seq:ISA Check Succeeds
  pushquestion.seq:ISA Check Succeeds
  pushtest.seq:ISA Check Succeeds
  ret-hazard.seq:ISA Check Succeeds
  rm asum.seq asumr.seq cjr.seq j-cc.seq poptest.seq pushquestion.seq pushtest.seq prog1.seq prog2.seq prog3.seq prog4.seq prog5.seq prog6.seq prog7.seq prog8.seq ret-hazard.seq
  ```

  可以看出所有结果均测试通过，可以进行下一步了。

- 回归测试

  上面步骤没问题了，可以先测下除了`iaddq`和`leave`的指令：

  ```bash
  unix> (cd ../ptest; make SIM=../seq/ssim)
  ```

  结果：

  ```bash
  crx@ubuntu:seq$ (cd ../ptest; make SIM=../seq/ssim)
  ./optest.pl -s ../seq/ssim
  Simulating with ../seq/ssim
    All 49 ISA Checks Succeed
  ./jtest.pl -s ../seq/ssim
  Simulating with ../seq/ssim
    All 64 ISA Checks Succeed
  ./ctest.pl -s ../seq/ssim
  Simulating with ../seq/ssim
    All 22 ISA Checks Succeed
  ./htest.pl -s ../seq/ssim
  Simulating with ../seq/ssim
    All 600 ISA Checks Succeed
  ```

  然后再加入`iaddq`指令的测试：

  ```bash
  (cd ../ptest; make SIM=../seq/ssim TFLAGS=-i)
  ```

  结果：

  ```c
  crx@ubuntu:seq$ (cd ../ptest; make SIM=../seq/ssim TFLAGS=-i)
  ./optest.pl -s ../seq/ssim -i
  Simulating with ../seq/ssim
    All 58 ISA Checks Succeed
  ./jtest.pl -s ../seq/ssim -i
  Simulating with ../seq/ssim
    All 96 ISA Checks Succeed
  ./ctest.pl -s ../seq/ssim -i
  Simulating with ../seq/ssim
    All 22 ISA Checks Succeed
  ./htest.pl -s ../seq/ssim -i
  Simulating with ../seq/ssim
    All 756 ISA Checks Succeed
  ```

  测试成功，开始partC（partB总计用时：3.5h）

## part C

在partC中，我们的工作目录切换至`sim/pipe`。

任务是修改`ncopy.ys`和`pipe-full.hcl`，从而让`ncopy.ys`跑的更快。

就是贴合PIPE的实现思路，看看如何高效利用流水线来实现代码的实现速度。

:one:**问题**：

1. 是什么拖慢了实现速度？

   答：循环和落后的相加指令。

2. 如何加速执行速度？

   答：一方面，csapp的5.8节对于循环问题导致的速度慢有效果；另一个对于落后的相加指令，可以让`iaddq V，rB`来直接代替常数运算。

3. 如何直观查看运行速度？

   答：运行脚本`benchmark.pl`

**解决思路**

**PIPE模拟器`psim`**

修改`pipe-full.hcl`，，使得支持`iaddq`，从而简化常数加寄存器的性能损失。这里修改过程与上述partB部分一致，修改完毕后，生成模拟器`psim`。

```bash
make psim VERSION=full			#报错解决方案与partB一致
```

- 检验`psim`的正确性：

  :one: 运行模拟程序`asumi.ys`

  ```bash
  ./psim -t ../y86-code/asumi.yo
  ```

  结果运行无误：

  ```bash
  48 instructions executed
  Status = HLT
  Condition Codes: Z=1 S=0 O=0
  Changed Register State:
  %rax:   0x0000000000000000      0x0000abcdabcdabcd
  %rsp:   0x0000000000000000      0x0000000000000100
  %rdi:   0x0000000000000000      0x0000000000000038
  %r10:   0x0000000000000000      0x0000a000a000a000
  Changed Memory State:
  0x00f0: 0x0000000000000000      0x0000000000000055
  0x00f8: 0x0000000000000000      0x0000000000000013
  ISA Check Succeeds
  CPI: 44 cycles/32 instructions = 1.38
  ```

  可以看出`pipe-full.hcl`修改成功。

  :two: 基准程序测试

  ```bash
  (cd ../y86-code; make testpsim)
  ```

  基准测试成功

  ```bash
  crx@ubuntu:pipe$ (cd ../y86-code; make testpsim)
  Makefile:42: warning: ignoring prerequisites on suffix rule definition
  Makefile:45: warning: ignoring prerequisites on suffix rule definition
  Makefile:48: warning: ignoring prerequisites on suffix rule definition
  Makefile:51: warning: ignoring prerequisites on suffix rule definition
  ../pipe/psim -t asum.yo > asum.pipe
  ../pipe/psim -t asumr.yo > asumr.pipe
  ../pipe/psim -t cjr.yo > cjr.pipe
  ../pipe/psim -t j-cc.yo > j-cc.pipe
  ../pipe/psim -t poptest.yo > poptest.pipe
  ../pipe/psim -t pushquestion.yo > pushquestion.pipe
  ../pipe/psim -t pushtest.yo > pushtest.pipe
  ../pipe/psim -t prog1.yo > prog1.pipe
  ../pipe/psim -t prog2.yo > prog2.pipe
  ../pipe/psim -t prog3.yo > prog3.pipe
  ../pipe/psim -t prog4.yo > prog4.pipe
  ../pipe/psim -t prog5.yo > prog5.pipe
  ../pipe/psim -t prog6.yo > prog6.pipe
  ../pipe/psim -t prog7.yo > prog7.pipe
  ../pipe/psim -t prog8.yo > prog8.pipe
  ../pipe/psim -t ret-hazard.yo > ret-hazard.pipe
  grep "ISA Check" *.pipe
  asum.pipe:ISA Check Succeeds
  asumr.pipe:ISA Check Succeeds
  cjr.pipe:ISA Check Succeeds
  j-cc.pipe:ISA Check Succeeds
  poptest.pipe:ISA Check Succeeds
  prog1.pipe:ISA Check Succeeds
  prog2.pipe:ISA Check Succeeds
  prog3.pipe:ISA Check Succeeds
  prog4.pipe:ISA Check Succeeds
  prog5.pipe:ISA Check Succeeds
  prog6.pipe:ISA Check Succeeds
  prog7.pipe:ISA Check Succeeds
  prog8.pipe:ISA Check Succeeds
  pushquestion.pipe:ISA Check Succeeds
  pushtest.pipe:ISA Check Succeeds
  ret-hazard.pipe:ISA Check Succeeds
  rm asum.pipe asumr.pipe cjr.pipe j-cc.pipe poptest.pipe pushquestion.pipe pushtest.pipe prog1.pipe prog2.pipe prog3.pipe prog4.pipe prog5.pipe prog6.pipe prog7.pipe prog8.pipe ret-hazard.pipe
  ```

  :three:回归测试

  ```bash
  (cd ../ptest; make SIM=../pipe/psim TFLAGS=-i)
  ```

  回归测试成功

  ```bash
  Simulating with ../pipe/psim
    All 756 ISA Checks Succeed
  ```

  支持`iaddq`的模拟器ok了，下面开始优化代码。

- **优化`ncopy.ys`**

  优化思路：

  - 将循环体中的一个计算表达式换多个表达式
  - 多个表达式之间填充气泡
  
  1. [将循环体中的一个计算表达式换多个表达式](https://github.com/kcxain/CSAPP-Lab/blob/master/Notes/Lab04-Architecture_Lab.md#%E5%BE%AA%E7%8E%AF%E5%B1%95%E5%BC%80)
  
     ```assembly
     	# Loop header
     	andq %rdx,%rdx		# len <= 0?
     	jmp test
     Loop:
     	mrmovq (%rdi),%r8
     	rmmovq %r8,(%rsi)
     	andq %r8,%r8
     	jle Loop1
     	iaddq $1,%rax
     Loop1:
     	mrmovq 8(%rdi),%r8
     	rmmovq %r8,8(%rsi)
     	andq %r8,%r8
     	jle Loop2
     	iaddq $1,%rax
     Loop2:
     	mrmovq 16(%rdi),%r8
     	rmmovq %r8,16(%rsi)
     	andq %r8,%r8
     	jle Loop3
     	iaddq $1,%rax
     Loop3:
     	mrmovq 24(%rdi),%r8
     	rmmovq %r8,24(%rsi)
     	andq %r8,%r8
     	jle Loop4
     	iaddq $1,%rax
     Loop4:
     	mrmovq 32(%rdi),%r8
     	rmmovq %r8,32(%rsi)
     	andq %r8,%r8
     	jle Loop5
     	iaddq $1,%rax
     Loop5:
     	mrmovq 40(%rdi),%r8
     	rmmovq %r8,40(%rsi)
     	iaddq $48,%rdi
     	iaddq $48,%rsi
     	andq %r8,%r8
     	jle test
     	iaddq $1,%rax	
     test:
     	iaddq $-6, %rdx			# 先减，判断够不够6个
     	jge Loop				# 6路展开
     	iaddq $-8,%rdi
     	iaddq $-8,%rsi
     	iaddq $6, %rdx
     	jmp test2				#剩下的
     Lore:
     	mrmovq (%rdi),%r8
     	rmmovq %r8,(%rsi)
     	andq %r8,%r8
     	jle test2
     	iaddq $1,%rax
     test2:
     	iaddq $8,%rdi
     	iaddq $8,%rsi
     	iaddq $-1, %rdx
     	jge Lore
     ```
  
     每次循环都对6个数进行复制，每次复制就设置一个条件语句判断返回时是否加1，对于剩下的数据每次循环只对1个数进行复制。
  
     注意，程序多次使用了下面的操作：
  
     ```assembly
     mrmovq (%rdi), %r8
     rmmovq %r8, (%rsi)
     ```
  
     `Y86-64`处理器的流水线有 F(取指)、D(译码)、E(执行)、M(访存)、W(写回) 五个阶段，D 阶段才读取寄存器，M 阶段才读取对应内存值，
  
     即使使用转发来避免数据冒险，这其中也至少会有一个气泡。像这样
  
     ```assembly
     mrmovq (%rdi), %r8
     bubble
     rmmovq %r8, (%rsi)
     ```
  
     一个优化办法是，多取一个寄存器，连续进行两次数据复制。
  
     ```assembly
     mrmovq (%rdi), %r8
     mrmovq 8(%rdi), %r9
     rmmovq %r8, (%rsi)
     rmmovq %r9, 8(%rsi)
     ```
  
     像这样，对`%r8`和`%r9`进行读入和读出的操作之间都隔着一条其他指令，就不会有气泡产生了。代码如下：
  
     ```assembly
     	# Loop header
     	andq %rdx,%rdx		# len <= 0?
     	jmp test
     Loop:
     	mrmovq (%rdi),%r8
     	mrmovq 8(%rdi),%r9
     	andq %r8,%r8
     	rmmovq %r8,(%rsi)
     	rmmovq %r9,8(%rsi)
     	jle Loop1
     	iaddq $1,%rax
     Loop1:	
     	andq %r9,%r9
     	jle Loop2
     	iaddq $1,%rax
     Loop2:
     	mrmovq 16(%rdi),%r8
     	mrmovq 24(%rdi),%r9
     	andq %r8,%r8
     	rmmovq %r8,16(%rsi)
     	rmmovq %r9,24(%rsi)
     	jle Loop3
     	iaddq $1,%rax
     Loop3:	
     	andq %r9,%r9
     	jle Loop4
     	iaddq $1,%rax
     Loop4:
     	mrmovq 32(%rdi),%r8
     	mrmovq 40(%rdi),%r9
     	andq %r8,%r8
     	rmmovq %r8,32(%rsi)
     	rmmovq %r9,40(%rsi)
     	jle Loop5
     	iaddq $1,%rax
     Loop5:
     	iaddq $48,%rdi
     	iaddq $48,%rsi		
     	andq %r9,%r9
     	jle test
     	iaddq $1,%rax
     test:
     	iaddq $-6, %rdx			# 先减，判断够不够6个
     	jge Loop				# 6路展开
     	iaddq $6, %rdx
     	jmp test2				#剩下的
     
     L:
     	mrmovq (%rdi),%r8
     	andq %r8,%r8
     	rmmovq %r8,(%rsi)
     	jle L1
     	iaddq $1,%rax
     L1:
     	mrmovq 8(%rdi),%r8
     	andq %r8,%r8
     	rmmovq %r8,8(%rsi)
     	jle L2
     	iaddq $1,%rax
     L2:
     	mrmovq 16(%rdi),%r8
     	iaddq $24,%rdi
     	rmmovq %r8,16(%rsi)
     	iaddq $24,%rsi
     	andq %r8,%r8
     	jle test2
     	iaddq $1,%rax
     test2:
     	iaddq $-3, %rdx			# 先减，判断够不够3个
     	jge L
     	iaddq $2, %rdx			# -1则不剩了，直接Done,0 剩一个, 1剩2个
         je R0
         jl Done
     	mrmovq (%rdi),%r8
     	mrmovq 8(%rdi),%r9
     	rmmovq %r8,(%rsi)
     	rmmovq %r9,8(%rsi)
     	andq %r8,%r8
     	jle R2
     	iaddq $1,%rax
     R2:
     	andq %r9,%r9
     	jle Done
     	iaddq $1,%rax
     	jmp Done
     R0:
     	mrmovq (%rdi),%r8
     	andq %r8,%r8
     	rmmovq %r8,(%rsi)
     	jle Done
     	iaddq $1,%rax
     ```
  
     结果：
  
     ```bash
     54      360     6.67
     55      362     6.58
     56      374     6.68
     57      381     6.68
     58      386     6.66
     59      394     6.68
     60      397     6.62
     61      399     6.54
     62      410     6.61
     63      418     6.63
     64      423     6.61
     Average CPE     8.17
     Score   46.7/60.0
     ```
  
     最终得分46.7，完结archlab，总共花费时间20h
