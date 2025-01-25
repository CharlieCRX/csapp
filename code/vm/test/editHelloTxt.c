#include "csapp.h"
void editHelloTxt() {
    // 以读写模式打开文件
    int fd = Open("/home/crx/study/csapp/code/vm/test/exe/hello.txt", O_RDWR, 0);
    struct stat stat;

    // 获取文件状态
    fstat(fd, &stat);
    
    // 使用 mmap 映射文件到内存
    char *bufp = Mmap(NULL, stat.st_size, PROT_WRITE|PROT_READ, MAP_SHARED, fd, 0);

    // 修改前先读取数据
    Write(STDOUT_FILENO, bufp, stat.st_size);

    // 修改数据
    bufp[0] = 'J';

    // 修改后读取数据
    Write(STDOUT_FILENO, bufp, stat.st_size);

    // 解除映射
    Munmap(bufp, stat.st_size);

    // 关闭文件
    Close(fd);
    return;
}

int main() {
    editHelloTxt();
    return 0;
}
