#ifndef __MIDDLEWARE_H__
#define __MIDDLEWARE_H__
/**
 * @file     middleware.h
 * @brief    中间件层整合头文件
 */

#ifdef __cplusplus
extern "C" {
#endif

/*========================= 头文件包含 (Includes) ==========================*/
#include "main.h"

/*========================= 宏定义 (Macros) ================================*/

/*========================= 类型定义 (Typedefs) ============================*/

/*========================= 全局变量 (Global Variables) ====================*/

/*========================= 函数声明 (Function Declarations) ===============*/
/**
 * @brief  应用层初始化
 */
void Middleware_Init(void);

#ifdef __cplusplus
}
#endif

#endif /* __MIDDLEWARE_H__ */
