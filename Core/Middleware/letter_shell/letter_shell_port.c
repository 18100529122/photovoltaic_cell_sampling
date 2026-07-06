#include "letter_shell_port.h"
#include "usart.h"
#include "cmsis_os.h"

/* Shell 对象 */
Shell shell;

/* Shell 缓冲区 */
char shellBuffer[512];

/**
 * @brief Shell 写函数
 */
signed short Shell_Write(char *data, unsigned short len)
{
    /* 通过 UART4 发送数据 */
    HAL_UART_Transmit(&huart4, (uint8_t *)data, len, 100);
    return len;
}

/**
 * @brief Shell 读函数
 */
signed short Shell_Read(char *data, unsigned short len)
{
    /* 这里可以实现中断方式接收，或者使用查询方式 */
    /* 为简化，这里暂时返回0，在中断中调用 shellHandler */
    return 0;
}

#if SHELL_USING_LOCK == 1
/**
 * @brief Shell 锁
 */
int Shell_Lock(Shell *shell)
{
    (void)shell;
    /* 使用 FreeRTOS 临界区 */
    taskENTER_CRITICAL();
    return 0;
}

/**
 * @brief Shell 解锁
 */
int Shell_Unlock(Shell *shell)
{
    (void)shell;
    taskEXIT_CRITICAL();
    return 0;
}
#endif

/**
 * @brief Shell 初始化
 */
void LetterShell_Init(void)
{
    /* 配置 shell 读写函数 */
    shell.read = Shell_Read;
    shell.write = Shell_Write;
#if SHELL_USING_LOCK == 1
    shell.lock = Shell_Lock;
    shell.unlock = Shell_Unlock;
#endif
    
    /* 初始化 shell */
    shellInit(&shell, shellBuffer, 512);
}

/**
 * @brief Shell 任务
 */
void LetterShell_Task(void *argument)
{
    (void)argument;
    while (1) {
        if (shell.read) {
            char data;
            if (shell.read(&data, 1) == 1) {
                shellHandler(&shell, data);
            }
        }
        osDelay(10); /* 避免空转占满 CPU */
    }
}

/**
 * @brief UART 接收回调
 */
void LetterShell_UART_RxCallback(uint8_t ch)
{
    shellHandler(&shell, ch);
}
