#include "FlashIf.h"

void FLASH_If_Init(void)
{
  /* Unlock the Program memory */
  HAL_FLASH_Unlock();

  /* Clear all FLASH flags */
  __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR | FLASH_FLAG_SIZERR |
                         FLASH_FLAG_OPTVERR | FLASH_FLAG_RDERR | FLASH_FLAG_FWWERR |
                         FLASH_FLAG_NOTZEROERR);
  /* Unlock the Program memory */
  HAL_FLASH_Lock();
}


uint32_t FLASH_If_Erase(uint32_t StartSector)
{
  uint32_t eraseres = 0;
  FLASH_EraseInitTypeDef desc;
  
  desc.PageAddress = StartSector;
  desc.TypeErase = FLASH_TYPEERASE_PAGES;
  
  if(StartSector < USER_FLASH_LAST_PAGE_ADDRESS)
  {
    desc.NbPages = (USER_FLASH_LAST_PAGE_ADDRESS - StartSector) / USER_FLASH_PAGE_SIZE;
    if(HAL_FLASHEx_Erase(&desc, &eraseres) != HAL_OK)
    {
      eraseres = 0;
    }
  }
  
  return eraseres;
}



uint32_t FLASH_If_Write(__IO uint32_t* FlashAddress, uint32_t* Data ,uint16_t DataLength)
{
  uint32_t i = 0;
  HAL_StatusTypeDef status = HAL_ERROR;
  
  for (i = 0; (i < DataLength) && (*FlashAddress <= (USER_FLASH_END_ADDRESS-4)); i++)
  {
    status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, *FlashAddress, *(uint32_t*)(Data+i));
    
    if(HAL_OK == status)
    {
      if (*(uint32_t*)*FlashAddress != *(uint32_t*)(Data+i))
      {
        return 2;
      }
      
      *FlashAddress += 4;
    }
    else
    {
      return 1;
    }
  }
  
  return 0;
}

