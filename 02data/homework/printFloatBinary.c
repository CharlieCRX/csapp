#include <stdio.h>
//from chat-gpt
// 函数将 float 转换为二进制字符串
void printFloatBinary(float f) {
    // 使用指针将 float 地址转换为 unsigned int 地址
    unsigned int* ptr = (unsigned int*)&f;

    // 获取 float 的二进制表示
    unsigned int floatAsInt = *ptr;

    // 打印二进制表示
    for (int i = sizeof(float) * 8 - 1; i >= 0; i--) {
        unsigned int mask = 1 << i;
        putchar((floatAsInt & mask) ? '1' : '0');

        // 在输出中添加空格，以改善可读性
        if (i % 4 == 0)
            putchar(' ');
    }

    putchar('\n');
}

int main() {
    float floatValue = 255.0;

    printf("Float value: %f\n", floatValue);
    printf("Binary representation: ");
    printFloatBinary(floatValue);

    return 0;
}

