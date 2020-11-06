/**
  ******************************************************************************
  * @file    system_HW32l003.h
  * @author  Software Team
  * @version V1.0.0
  * @date    21-Oct-2019
  * @brief   CMSIS Cortex-M0+ Device Peripheral Access Layer Header File. 
  *          This file contains all the peripheral register's definitions, bits 
  *          definitions and memory mapping for HW32L0xx devices.            
  *            
  *          This file contains:
  *           - Data structures and the address mapping for all peripherals
  *           - Peripheral's registers declarations and bits definition
  *  
  ******************************************************************************
  */
	
#include "stdint.h"

/** @addtogroup CMSIS
  * @{
  */

/** @addtogroup HW32l003_system
  * @{
  */  
  
/**
  * @brief Define to prevent recursive inclusion
  */
#ifndef __SYSTEM_HW32L003_H
#define __SYSTEM_HW32L003_H

#ifdef __cplusplus
 extern "C" {
#endif 

/* Includes ------------------------------------------------------------------*/
/** @addtogroup HW32L003_System_Includes
  * @{
  */

/**
  * @}
  */

/* Exported constants --------------------------------------------------------*/
/** @addtogroup HW32L003_System_Exported_Constants
  * @{
  */

/**
  * @}
  */

/* Exported types ------------------------------------------------------------*/
/** @addtogroup HW32L003_System_Exported_types
  * @{
  */
extern uint32_t SystemCoreClock;          /*!< System Clock Frequency (Core Clock) */	

/**
  * @}
  */
	
	
/* Exported macro ------------------------------------------------------------*/
/** @addtogroup HW32L003_System_Exported_Macros
  * @{
  */

/**
  * @}
  */


/* Exported functions --------------------------------------------------------*/
/** @addtogroup HW32L003_System_Exported_Functions
  * @{
  */
extern void SystemInit(void);
extern void SystemCoreClockUpdate(void);

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /*__SYSTEM_HW32L003_H */

/**
  * @}
  */ /* End of group HW32l003_system */
  
/**
  * @}
  */ /* End of group CMSIS */
	
/************************ (C) COPYRIGHT XMC *****END OF FILE****/
