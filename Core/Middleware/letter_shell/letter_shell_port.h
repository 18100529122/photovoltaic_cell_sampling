#ifndef __LETTER_SHELL_PORT_H
#define __LETTER_SHELL_PORT_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include "shell.h"

void LetterShell_Init(void);
void LetterShell_Task(void *argument);
void LetterShell_UART_RxCallback(uint8_t ch);

#ifdef __cplusplus
}
#endif

#endif /* __LETTER_SHELL_PORT_H */
