/**
 * @file     middleware.c
 * @brief    中间件层整合实现
 */

/*========================= 头文件包含 (Includes) ==========================*/
#include "middleware.h"
#include "letter_shell_port.h"
#include "cmsis_os.h"

/*========================= 宏定义 (Macros) ================================*/

/*========================= 类型定义 (Typedefs) ============================*/

/*========================= 全局变量 (Global Variables) ====================*/

/*========================= 静态变量 (Static Variables) ====================*/

/*========================= 静态函数声明 (Static Function Declarations) ====*/

/*========================= 函数实现 (Function Definitions) ================*/
/**
 * @brief  中间件层初始化
 */
void Middleware_Init(void)
{
    /* 初始化 Letter Shell */
    LetterShell_Init();
    
    /* 创建 Shell 任务 */
    BaseType_t ret = xTaskCreate(LetterShell_Task, 
                                  "shell_task", 
                                  1024, 
                                  NULL, 
                                  tskIDLE_PRIORITY + 1, 
                                  NULL);
    if (ret != pdPASS) {
        printf("Failed to create shell_task! Error: %ld\r\n", ret);
    } else {
        printf("shell_task created successfully!\r\n");
    }

    printf("Middleware Init Complete\r\n");
}

