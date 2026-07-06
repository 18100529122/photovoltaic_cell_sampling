#ifndef __BSP_FLASH_H__
#define __BSP_FLASH_H__
/**
 * @file     bsp_flash.h
 * @brief    flash头文件
 */
#ifdef __cplusplus
extern "C" {
#endif

/*========================= 头文件包含 (Includes) ==========================*/
#include "main.h"

/*========================= 宏定义 (Macros) ================================*/
#define FLASH_DATA_START_ADDR    0x0807C000UL
#define FLASH_DATA_SIZE          (16 * 1024UL)
#define FLASH_DATA_END_ADDR      (FLASH_DATA_START_ADDR + FLASH_DATA_SIZE)
#define FLASH_PAGE_SIZE          (2 * 1024UL)
#define FLASH_DATA_PAGE_COUNT    (FLASH_DATA_SIZE / FLASH_PAGE_SIZE)

/*========================= 类型定义 (Typedefs) ============================*/

/*========================= 全局变量 (Global Variables) ====================*/

/*========================= 函数声明 (Function Declarations) ===============*/
void BSP_FLASH_Init(void);
HAL_StatusTypeDef BSP_FLASH_ErasePage(uint32_t page);
HAL_StatusTypeDef BSP_FLASH_EraseAllData(void);
HAL_StatusTypeDef BSP_FLASH_Write(uint32_t addr, const uint8_t *data, uint32_t len);
void BSP_FLASH_Read(uint32_t addr, uint8_t *data, uint32_t len);

#ifdef __cplusplus
}
#endif
#endif /* __BSP_FLASH_H__ */
