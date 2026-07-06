/**
 * @file shell_cmd_list.c
 * @author Letter (NevermindZZT@gmail.com)
 * @brief shell cmd list
 * @version 3.0.0
 * @date 2020-01-17
 * 
 * @copyright (c) 2020 Letter
 * 
 */

#include "shell.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "data_structure.h"
#include "bsp_can.h"
#include "bsp_flash.h"

#if SHELL_USING_CMD_EXPORT != 1

extern int shellSetVar(char *name, int value);
extern void shellUp(Shell *shell);
extern void shellDown(Shell *shell);
extern void shellRight(Shell *shell);
extern void shellLeft(Shell *shell);
extern void shellTab(Shell *shell);
extern void shellBackspace(Shell *shell);
extern void shellDelete(Shell *shell);
extern void shellEnter(Shell *shell);
extern void shellHelp(int argc, char *argv[]);
extern void shellUsers(void);
extern void shellCmds(void);
extern void shellVars(void);
extern void shellKeys(void);
extern void shellClear(void);
#if SHELL_EXEC_UNDEF_FUNC == 1
extern int shellExecute(int argc, char *argv[]);
#endif

SHELL_AGENCY_FUNC(shellRun, shellGetCurrent(), (const char *)p1);

/**
 * @brief 测试命令
 */
static int Test_Cmd(int argc, char *argv[])
{
    shellWriteString(shellGetCurrent(), "Hello from letter shell!\r\n");
    return 0;
}

/**
 * @brief 系统信息命令
 */
static int SysInfo_Cmd(int argc, char *argv[])
{
    Shell *shell = shellGetCurrent();
    shellWriteString(shell, "=== System Info ===\r\n");
    shellWriteString(shell, "MCU: STM32G474VETx\r\n");
    shellWriteString(shell, "Clock: 170MHz\r\n");
    shellWriteString(shell, "CAN Node: 30\r\n");
    return 0;
}

/**
 * @brief CAN 测试命令
 */
static int CANTest_Cmd(int argc, char *argv[])
{
    Shell *shell = shellGetCurrent();
    ResultData_t testData;
    int i;
    
    shellWriteString(shell, "Starting CAN test...\r\n");
    
    /* 填充测试数据 */
    for (i = 0; i < 12; i++) {
        testData.voltage[i] = 1.23f + i * 0.1f;
        testData.current[i] = 0.56f + i * 0.05f;
    }
    testData.temperature[0] = 25.5f;
    testData.temperature[1] = 30.2f;
    
    /* 发送数据 */
    if (BSP_CAN_SendData(&testData) == CAN_OK) {
        shellWriteString(shell, "CAN data sent successfully!\r\n");
    } else {
        shellWriteString(shell, "CAN send failed!\r\n");
    }
    
    return 0;
}

/**
 * @brief Flash擦除命令
 * Usage: flash_erase <page> 或 flash_erase all
 */
static int FlashErase_Cmd(int argc, char *argv[])
{
    Shell *shell = shellGetCurrent();
    HAL_StatusTypeDef status;
    char buffer[128];
    uint32_t actual_page;

    if (argc < 2) {
        shellWriteString(shell, "Usage: flash_erase <page>|all\r\n");
        return -1;
    }

    snprintf(buffer, sizeof(buffer), "Flash info: DATA_START=0x%08lX, PAGE_SIZE=%lu, PAGE_COUNT=%lu\r\n", 
             FLASH_DATA_START_ADDR, FLASH_PAGE_SIZE, FLASH_DATA_PAGE_COUNT);
    shellWriteString(shell, buffer);

    if (strcmp(argv[1], "all") == 0) {
        actual_page = (FLASH_DATA_START_ADDR - FLASH_BASE) / FLASH_PAGE_SIZE;
        snprintf(buffer, sizeof(buffer), "Erasing all %lu pages starting from page %lu...\r\n", 
                 FLASH_DATA_PAGE_COUNT, actual_page);
        shellWriteString(shell, buffer);
        status = BSP_FLASH_EraseAllData();
    } else {
        int page = atoi(argv[1]);
        if (page < 0 || page >= (int)FLASH_DATA_PAGE_COUNT) {
            snprintf(buffer, sizeof(buffer), "Invalid page! (0-%lu)\r\n", FLASH_DATA_PAGE_COUNT - 1);
            shellWriteString(shell, buffer);
            return -1;
        }
        actual_page = (FLASH_DATA_START_ADDR - FLASH_BASE) / FLASH_PAGE_SIZE + page;
        snprintf(buffer, sizeof(buffer), "Erasing logical page %d (physical page %lu)...\r\n", page, actual_page);
        shellWriteString(shell, buffer);
        status = BSP_FLASH_ErasePage(page);
    }

    if (status == HAL_OK) {
        shellWriteString(shell, "Erase done!\r\n");
        return 0;
    } else {
        snprintf(buffer, sizeof(buffer), "Erase failed! (status=%d)\r\n", status);
        shellWriteString(shell, buffer);
        return -1;
    }
}

/**
 * @brief Flash写入命令
 * Usage: flash_write <addr> <data1> <data2> ...
 */
static int FlashWrite_Cmd(int argc, char *argv[])
{
    Shell *shell = shellGetCurrent();
    HAL_StatusTypeDef status;
    uint32_t addr;
    uint8_t data[128];
    int i;
    char buffer[64];
    
    if (argc < 3) {
        shellWriteString(shell, "Usage: flash_write <addr> <data1> <data2> ...\r\n");
        return -1;
    }
    
    addr = strtoul(argv[1], NULL, 0);
    if (addr < FLASH_DATA_START_ADDR || addr >= FLASH_DATA_END_ADDR) {
        snprintf(buffer, sizeof(buffer), "Invalid addr! (0x%08lX-0x%08lX)\r\n", FLASH_DATA_START_ADDR, FLASH_DATA_END_ADDR - 1);
        shellWriteString(shell, buffer);
        return -1;
    }
    
    int data_len = argc - 2;
    if (data_len > 128) data_len = 128;
    
    for (i = 0; i < data_len; i++) {
        data[i] = (uint8_t)strtoul(argv[2 + i], NULL, 0);
    }
    
    snprintf(buffer, sizeof(buffer), "Writing %d bytes to 0x%08lX...\r\n", data_len, addr);
    shellWriteString(shell, buffer);
    
    status = BSP_FLASH_Write(addr, data, data_len);
    
    if (status == HAL_OK) {
        shellWriteString(shell, "Write done!\r\n");
        return 0;
    } else {
        snprintf(buffer, sizeof(buffer), "Write failed! (status=%d)\r\n", status);
        shellWriteString(shell, buffer);
        return -1;
    }
}

/**
 * @brief Flash读取命令
 * Usage: flash_read <addr> <len>
 */
static int FlashRead_Cmd(int argc, char *argv[])
{
    Shell *shell = shellGetCurrent();
    uint32_t addr;
    uint32_t len;
    uint8_t data[128];
    int i;
    char buffer[128];
    
    if (argc < 3) {
        shellWriteString(shell, "Usage: flash_read <addr> <len>\r\n");
        return -1;
    }
    
    addr = strtoul(argv[1], NULL, 0);
    len = strtoul(argv[2], NULL, 0);
    
    if (addr < FLASH_DATA_START_ADDR || (addr + len) > FLASH_DATA_END_ADDR) {
        snprintf(buffer, sizeof(buffer), "Invalid addr/len! (0x%08lX-0x%08lX)\r\n", FLASH_DATA_START_ADDR, FLASH_DATA_END_ADDR - 1);
        shellWriteString(shell, buffer);
        return -1;
    }
    
    if (len > 128) len = 128;
    
    BSP_FLASH_Read(addr, data, len);
    
    snprintf(buffer, sizeof(buffer), "Read %lu bytes from 0x%08lX:\r\n", len, addr);
    shellWriteString(shell, buffer);
    
    for (i = 0; i < (int)len; i++) {
        if (i % 16 == 0) {
            if (i > 0) shellWriteString(shell, "\r\n");
            snprintf(buffer, sizeof(buffer), "  %04X: ", (unsigned int)(addr + i - FLASH_DATA_START_ADDR));
            shellWriteString(shell, buffer);
        }
        snprintf(buffer, sizeof(buffer), "%02X ", data[i]);
        shellWriteString(shell, buffer);
    }
    shellWriteString(shell, "\r\n");
    
    return 0;
}

/**
 * @brief shell命令表
 * 
 */
const ShellCommand shellCommandList[] = 
{
    {.attr.value=SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_USER),
    .data.user.name = SHELL_DEFAULT_USER,
    .data.user.password = SHELL_DEFAULT_USER_PASSWORD,
    .data.user.desc = "default user"},
    SHELL_CMD_ITEM(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC),
                   setVar, shellSetVar, set var),
    SHELL_KEY_ITEM(SHELL_CMD_PERMISSION(0), 0x1B5B4100, shellUp, up),
    SHELL_KEY_ITEM(SHELL_CMD_PERMISSION(0), 0x1B5B4200, shellDown, down),
    SHELL_KEY_ITEM(SHELL_CMD_PERMISSION(0)|SHELL_CMD_ENABLE_UNCHECKED,
                   0x1B5B4300, shellRight, right),
    SHELL_KEY_ITEM(SHELL_CMD_PERMISSION(0)|SHELL_CMD_ENABLE_UNCHECKED,
                   0x1B5B4400, shellLeft, left),
    SHELL_KEY_ITEM(SHELL_CMD_PERMISSION(0), 0x09000000, shellTab, tab),
    SHELL_KEY_ITEM(SHELL_CMD_PERMISSION(0)|SHELL_CMD_ENABLE_UNCHECKED,
                   0x08000000, shellBackspace, backspace),
    SHELL_KEY_ITEM(SHELL_CMD_PERMISSION(0)|SHELL_CMD_ENABLE_UNCHECKED,
                   0x7F000000, shellDelete, delete),
    SHELL_KEY_ITEM(SHELL_CMD_PERMISSION(0)|SHELL_CMD_ENABLE_UNCHECKED,
                   0x1B5B337E, shellDelete, delete),
#if SHELL_ENTER_LF == 1
    SHELL_KEY_ITEM(SHELL_CMD_PERMISSION(0)|SHELL_CMD_ENABLE_UNCHECKED,
                   0x0A000000, shellEnter, enter),
#endif
#if SHELL_ENTER_CR == 1
    SHELL_KEY_ITEM(SHELL_CMD_PERMISSION(0)|SHELL_CMD_ENABLE_UNCHECKED,
                   0x0D000000, shellEnter, enter),
#endif
#if SHELL_ENTER_CRLF == 1
    SHELL_KEY_ITEM(SHELL_CMD_PERMISSION(0)|SHELL_CMD_ENABLE_UNCHECKED,
                   0x0D0A0000, shellEnter, enter),
#endif
    SHELL_CMD_ITEM(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN)|SHELL_CMD_DISABLE_RETURN,
                   help, shellHelp, show command info\r\nhelp [cmd]),
    SHELL_CMD_ITEM(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC)|SHELL_CMD_DISABLE_RETURN,
                   users, shellUsers, list all user),
    SHELL_CMD_ITEM(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC)|SHELL_CMD_DISABLE_RETURN,
                   cmds, shellCmds, list all cmd),
    SHELL_CMD_ITEM(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC)|SHELL_CMD_DISABLE_RETURN,
                   vars, shellVars, list all var),
    SHELL_CMD_ITEM(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC)|SHELL_CMD_DISABLE_RETURN,
                   keys, shellKeys, list all key),
    SHELL_CMD_ITEM(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC)|SHELL_CMD_DISABLE_RETURN,
                   clear, shellClear, clear console),
    SHELL_CMD_ITEM(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC)|SHELL_CMD_DISABLE_RETURN,
                   sh, SHELL_AGENCY_FUNC_NAME(shellRun), run command directly),
#if SHELL_EXEC_UNDEF_FUNC == 1
    SHELL_CMD_ITEM(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN)|SHELL_CMD_DISABLE_RETURN,
                   exec, shellExecute, execute function undefined),
#endif
    SHELL_CMD_ITEM(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN),
                   test, Test_Cmd, test command),
    SHELL_CMD_ITEM(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN),
                   sysinfo, SysInfo_Cmd, show system info),
    SHELL_CMD_ITEM(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN),
                   cantest, CANTest_Cmd, test CAN send),
    SHELL_CMD_ITEM(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN),
                   flash_erase, FlashErase_Cmd, erase flash: flash_erase <page>|all),
    SHELL_CMD_ITEM(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN),
                   flash_write, FlashWrite_Cmd, write flash: flash_write <addr> <data1> <data2> ...),
    SHELL_CMD_ITEM(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN),
                   flash_read, FlashRead_Cmd, read flash: flash_read <addr> <len>),
};


/**
 * @brief shell命令表大小
 * 
 */
const unsigned short shellCommandCount 
    = sizeof(shellCommandList) / sizeof(ShellCommand);

#endif
