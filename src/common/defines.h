/*
 * 模块名称  : 公共——宏定义与编译选项
 * 编写人    : 组员A（架构负责人）
 * 功能描述  : 全局宏定义、调试开关、系统常量
 */

#ifndef DEFINES_H
#define DEFINES_H

/* ========================== 调试开关 ========================== */

// #define DEBUG_MODE          /* 开启后输出详细调试信息 */

#ifdef DEBUG_MODE
#define DEBUG_PRINT(fmt, ...) printf("[DEBUG] " fmt "\n", ##__VA_ARGS__)
#else
#define DEBUG_PRINT(fmt, ...) ((void)0)
#endif

/* ========================== 安全宏 ========================== */

/** @brief 内存分配安全检查 */
#define safe_malloc(ptr, size)                                              \
    do                                                                      \
    {                                                                       \
        (ptr) = malloc(size);                                               \
        if ((ptr) == NULL)                                                  \
        {                                                                   \
            printf("[错误] 内存分配失败: %s, 行 %d\n", __FILE__, __LINE__); \
            exit(EXIT_FAILURE);                                             \
        }                                                                   \
    } while (0)

/** @brief 安全释放并置空 */
#define safe_free(ptr) \
    do                 \
    {                  \
        free(ptr);     \
        (ptr) = NULL;  \
    } while (0)

/* ========================== 工具宏 ========================== */

/** @brief 取两数较小值 */
#define min_val(a, b) (((a) < (b)) ? (a) : (b))

/** @brief 取两数较大值 */
#define max_val(a, b) (((a) > (b)) ? (a) : (b))

/** @brief 交换两个变量 */
#define swap_val(type, a, b) \
    do                   \
    {                    \
        type _tmp = (a); \
        (a) = (b);       \
        (b) = _tmp;      \
    } while (0)

/** @brief 清空输入缓冲区 */
#define clear_stdin()                                 \
    do                                                \
    {                                                 \
        int _c;                                       \
        while ((_c = getchar()) != '\n' && _c != EOF) \
            ;                                         \
    } while (0)

/** @brief 按任意键继续 */
#define pause_console()                         \
    do                                  \
    {                                   \
        printf("\n按 Enter 键继续..."); \
        clear_stdin();                  \
    } while (0)

#endif /* DEFINES_H */
