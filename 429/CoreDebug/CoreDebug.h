#ifndef __CORE_DEBUG_H
#define __CORE_DEBUG_H
#ifdef __cplusplus
 extern "C" {
#endif
#include "main.h"

 typedef struct
 {
   __IOM uint32_t DHCSR;                  /*!< Offset: 0x000 (R/W)  Debug Halting Control and Status Register */
   __OM  uint32_t DCRSR;                  /*!< Offset: 0x004 ( /W)  Debug Core Register Selector Register */
   __IOM uint32_t DCRDR;                  /*!< Offset: 0x008 (R/W)  Debug Core Register Data Register */
   __IOM uint32_t DEMCR;                  /*!< Offset: 0x00C (R/W)  Debug Exception and Monitor Control Register */
 } CoreDebugType;

#define CoreDebugBASE      (0xE000EDF0UL)                             /*!< Core Debug Base Address*/
#define CoreDebugPtr          ((CoreDebugType *)     CoreDebugBASE   ) /*!< Core Debug configuration struct */

#define CoreDebug_DHCSR_C_DEBUGEN_Pos       0U                                            /*!< CoreDebug DHCSR: C_DEBUGEN Position */
#define CoreDebug_DHCSR_C_DEBUGEN_Msk      (1UL /*<< CoreDebug_DHCSR_C_DEBUGEN_Pos*/)     /*!< CoreDebug DHCSR: C_DEBUGEN Mask */

unsigned char IsDebuggerConnected(void);

#ifdef __cplusplus
 }
#endif
#endif
