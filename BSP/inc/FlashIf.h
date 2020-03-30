#ifndef __FLASHIF_H
#define __FLASHIF_H


#include "stm32l0xx.h"

/* Define the address from where user application will be loaded.
   Note: this area is reserved for the IAP code                  */
#define APPLICATION_ADDRESS     (uint32_t)0x08002000      /* Start user code address: ADDR_FLASH_PAGE_8 */
//#define USER_FLASH_PAGE_SIZE    0x400
#define USER_FLASH_PAGE_SIZE    FLASH_PAGE_SIZE
#define USER_FLASH_END_ADDRESS        0x080003FFF
#define USER_FLASH_LAST_PAGE_ADDRESS  0x08003F80

/* Notable Flash addresses */
//#define FLASH_START_BANK1             ((uint32_t)0x08000000)
//#define FLASH_LAST_SECTOR_BANK1       ((uint32_t)0x08017000)
//#define FLASH_LAST_PAGE_BANK1         ((uint32_t)0x08017F00)
//#define FLASH_END_BANK1               ((uint32_t)0x08017FFF)
//#define FLASH_START_BANK2             ((uint32_t)0x08018000)
//#define USER_FLASH_END_ADDRESS        (uint32_t)0x08020000

/* Define the user application size */
#define USER_FLASH_SIZE   (USER_FLASH_END_ADDRESS - APPLICATION_ADDRESS + 1) /* Small default template application */


/* Exported macro ------------------------------------------------------------*/
/* ABSoulute value */
//#define ABS_RETURN(x,y)               (((x) < (y)) ? (y) : (x))

/* Get the number of sectors from where the user program will be loaded */
#define FLASH_SECTOR_NUMBER           (uint32_t)((APPLICATION_ADDRESS - 0x08000000) >> 12)

/* Compute the mask to test if the Flash memory, where the user program will be
  loaded, is write protected */
#define FLASH_PROTECTED_SECTORS       (~(uint32_t)((1 << FLASH_SECTOR_NUMBER) - 1))

/* Exported functions ------------------------------------------------------- */
void FLASH_If_Init(void);
uint32_t FLASH_If_Erase(uint32_t StartSector);
uint32_t FLASH_If_GetWriteProtectionStatus(void);
uint32_t FLASH_If_Write(__IO uint32_t* FlashAddress, uint32_t* Data ,uint16_t DataLength);
uint32_t FLASH_If_WriteProtectionConfig(uint32_t protectionstate);

#endif
