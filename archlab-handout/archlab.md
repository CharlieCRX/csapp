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
2. 所有暂时的定义变量都在一个代码块中的历史C代码，可以用`-fcommon`来编译

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

求和函数原型

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

:three: