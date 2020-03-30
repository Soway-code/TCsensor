#include "eeprom.h"
//#include "para.h"
/*
void Eeprom_WriteByte(uint16_t pWriteAddr, uint8_t pData)
{
    uint32_t WriteAddr;
    uint32_t TimeOut = 0xffffff;
    if(pWriteAddr > EEPROM_BYTE_SIZE)
    {
        return;
    }
    WriteAddr = EEPROM_BASE_ADDR + pWriteAddr;
    HAL_FLASH_Unlock();
    //DIS_INT;
    *(uint8_t *)WriteAddr = pData;
    while(__HAL_FLASH_GET_FLAG(FLASH_FLAG_BSY) && TimeOut > 0)
    {
        TimeOut--;
    }
    HAL_FLASH_Lock();
    //EN_INT;       
}


void Eeprom_ReadByte(uint16_t pReadAddr, uint8_t *pReadData)
{
    uint32_t ReadAddr;
    if(pReadAddr > EEPROM_BYTE_SIZE)
    {
        return;
    }
    ReadAddr = EEPROM_BASE_ADDR + pReadAddr;
    //DIS_INT;
    *pReadData = *(uint8_t *)ReadAddr;
    //EN_INT;
}
*/

void Eeprom_WriteNBytes(uint16_t pWriteAddr, uint8_t *pBuffer, uint16_t pLen)
{
    uint32_t WriteAddr;
    uint32_t TimeOut = 0xffffff;
    
    if(pWriteAddr > EEPROM_BYTE_SIZE)
    {
        return;
    }
    WriteAddr = EEPROM_BASE_ADDR + pWriteAddr;
    //DIS_INT;
    HAL_FLASH_Unlock();
    while(pLen--)
    {
        *(uint8_t *)WriteAddr++ = *pBuffer++;
        while(__HAL_FLASH_GET_FLAG(FLASH_FLAG_BSY) && TimeOut > 0)
        {
            TimeOut--;
        }
    }
    HAL_FLASH_Lock();
    //EN_INT;
}


void Eeprom_ReadNBytes(uint16_t pReadAddr, uint8_t *pBuffer, uint16_t pLen)
{
    uint32_t ReadAddr;
    if(pReadAddr > EEPROM_BYTE_SIZE)
    {
        return;
    }
    ReadAddr = EEPROM_BASE_ADDR + pReadAddr;
    //DIS_INT;
    while(pLen--)
    {
        *pBuffer++ = *(uint8_t *)ReadAddr++;
    }
    //EN_INT;
}

/*
void Eeprom_WNBytesMul3T(uint16_t WriteAddr, uint8_t *pBuffer, uint16_t NumToWrite)
{
    Eeprom_WriteNBytes(WriteAddr, pBuffer, NumToWrite);
    Eeprom_WriteNBytes(WriteAddr + 2 * 0x80, pBuffer, NumToWrite);
    Eeprom_WriteNBytes(WriteAddr + 4 * 0x80, pBuffer, NumToWrite);
}
*/