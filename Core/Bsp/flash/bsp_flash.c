/**
 * @file     bsp_flash.c
 * @brief    flash实现
 */

/*========================= 头文件包含 (Includes) ==========================*/
#include "bsp_flash.h"

/*========================= 宏定义 (Macros) ================================*/
#define FLASH_BANK1_END_ADDR  (0x08040000UL)

/*========================= 类型定义 (Typedefs) ============================*/

/*========================= 全局变量 (Global Variables) ====================*/

/*========================= 静态变量 (Static Variables) ====================*/

/*========================= 静态函数声明 (Static Function Declarations) ====*/
static uint32_t BSP_FLASH_GetBank(uint32_t addr);

/*========================= 函数实现 (Function Definitions) ================*/

static uint32_t BSP_FLASH_GetBank(uint32_t addr)
{
    if (addr < FLASH_BANK1_END_ADDR) {
        return FLASH_BANK_1;
    } else {
        return FLASH_BANK_2;
    }
}

void BSP_FLASH_Init(void)
{
}

HAL_StatusTypeDef BSP_FLASH_ErasePage(uint32_t page)
{
    HAL_StatusTypeDef status;
    FLASH_EraseInitTypeDef erase_init;
    uint32_t page_error;
    uint32_t actual_addr;
    uint32_t actual_page;
    uint32_t bank;

    if (page >= FLASH_DATA_PAGE_COUNT) {
        return HAL_ERROR;
    }

    actual_addr = FLASH_DATA_START_ADDR + page * FLASH_PAGE_SIZE;
    bank = BSP_FLASH_GetBank(actual_addr);
    
    if (bank == FLASH_BANK_1) {
        actual_page = (actual_addr - FLASH_BASE) / FLASH_PAGE_SIZE;
    } else {
        actual_page = (actual_addr - FLASH_BANK1_END_ADDR) / FLASH_PAGE_SIZE;
    }

    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_PROGERR | FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR | FLASH_FLAG_SIZERR | FLASH_FLAG_PGSERR | FLASH_FLAG_MISERR | FLASH_FLAG_FASTERR);

    status = HAL_FLASH_Unlock();
    if (status != HAL_OK) {
        return status;
    }

    erase_init.TypeErase = FLASH_TYPEERASE_PAGES;
    erase_init.Banks = bank;
    erase_init.Page = actual_page;
    erase_init.NbPages = 1;

    status = HAL_FLASHEx_Erase(&erase_init, &page_error);

    HAL_FLASH_Lock();

    return status;
}

HAL_StatusTypeDef BSP_FLASH_EraseAllData(void)
{
    HAL_StatusTypeDef status;
    FLASH_EraseInitTypeDef erase_init;
    uint32_t page_error;
    uint32_t actual_page;
    uint32_t bank;

    bank = BSP_FLASH_GetBank(FLASH_DATA_START_ADDR);
    
    if (bank == FLASH_BANK_1) {
        actual_page = (FLASH_DATA_START_ADDR - FLASH_BASE) / FLASH_PAGE_SIZE;
    } else {
        actual_page = (FLASH_DATA_START_ADDR - FLASH_BANK1_END_ADDR) / FLASH_PAGE_SIZE;
    }

    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_PROGERR | FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR | FLASH_FLAG_SIZERR | FLASH_FLAG_PGSERR | FLASH_FLAG_MISERR | FLASH_FLAG_FASTERR);

    status = HAL_FLASH_Unlock();
    if (status != HAL_OK) {
        return status;
    }

    erase_init.TypeErase = FLASH_TYPEERASE_PAGES;
    erase_init.Banks = bank;
    erase_init.Page = actual_page;
    erase_init.NbPages = FLASH_DATA_PAGE_COUNT;

    status = HAL_FLASHEx_Erase(&erase_init, &page_error);

    HAL_FLASH_Lock();

    return status;
}

HAL_StatusTypeDef BSP_FLASH_Write(uint32_t addr, const uint8_t *data, uint32_t len)
{
    HAL_StatusTypeDef status;
    uint32_t i;
    uint64_t data64;
    uint32_t aligned_addr;
    uint8_t write_buffer[8];

    if (addr < FLASH_DATA_START_ADDR || (addr + len) > FLASH_DATA_END_ADDR) {
        return HAL_ERROR;
    }

    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_PROGERR | FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR | FLASH_FLAG_SIZERR | FLASH_FLAG_PGSERR | FLASH_FLAG_MISERR | FLASH_FLAG_FASTERR);

    status = HAL_FLASH_Unlock();
    if (status != HAL_OK) {
        return status;
    }

    for (i = 0; i < len; ) {
        aligned_addr = (addr + i) & ~0x07;
        uint32_t offset_in_dword = (addr + i) - aligned_addr;
        uint32_t bytes_remaining = len - i;
        uint32_t bytes_to_write = 8 - offset_in_dword;

        if (bytes_to_write > bytes_remaining) {
            bytes_to_write = bytes_remaining;
        }

        memset(write_buffer, 0xFF, 8);
        memcpy(&write_buffer[offset_in_dword], &data[i], bytes_to_write);

        data64 = 0;
        for (uint32_t j = 0; j < 8; j++) {
            data64 |= (uint64_t)write_buffer[j] << (j * 8);
        }

        status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, aligned_addr, data64);
        if (status != HAL_OK) {
            break;
        }

        i += bytes_to_write;
    }

    HAL_FLASH_Lock();

    return status;
}

void BSP_FLASH_Read(uint32_t addr, uint8_t *data, uint32_t len)
{
    uint32_t i;

    if (addr < FLASH_DATA_START_ADDR || (addr + len) > FLASH_DATA_END_ADDR) {
        return;
    }

    for (i = 0; i < len; i++) {
        data[i] = *(volatile uint8_t *)(addr + i);
    }
}

