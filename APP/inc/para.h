#ifndef _PARA_H
#define _PARA_H


#define RUN_ADDR_BASE				                                  0x00								//参数初始化标志位，避免每次上电都写EEPROMP
#define SLAVE_ADDR					(RUN_ADDR_BASE		+ 0x01)
#define TEM_UP_THR                                      (SLAVE_ADDR             + 0x01) //温度1上阀值
#define TEM_DO_THR                                      (TEM_UP_THR             + 0x02)
#define TEM_DU_THR                                      (TEM_DO_THR             + 0x02)
#define USER_DEFAULT_LEN				(TEM_DU_THR             + 0x02)

//---------------------------------------------------
#define COMPANY						(USER_DEFAULT_LEN       + 0x40) 
#define DEV_ENCODING				        (COMPANY		+ 0x40)
#define HWVERSION					(DEV_ENCODING		+ 0x40)
#define SFVERSION					(HWVERSION	        + 0x40)
#define DEV_ID						(SFVERSION		+ 0x40)
#define CUSTOMERCODE                                    (DEV_ID		        + 0x40)  


void ReadPara(void);


#endif