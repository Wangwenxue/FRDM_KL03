#include "common.h"
#include "uart.h"
#include "FC_protocol.h"
#include "flash_FTFL.h" /* include flash driver header file */

/********************************************************************/
////////////////////////////////////////////////////
// variable declare
uint16_t uiNumberCount = 0;
unsigned char g_ucFC_State;
static unsigned char m_uiRecCount;
static REC_FRAME_HEAD  m_RecFrame;
static unsigned char *m_pRecFrame;
static uint8_t  m_ucDataBuff[WRITE_BLOCK_SIZE];
const FC_IDENT_INFO m_MCU_Info = {
	FC_PROTOCOL_VERSION,			
	IDENT_SDID,	
	FLASH_NUM,
	USER_FLASH_START_ADDR,		
	USER_FLASH_END_ADDR,			
	RELOCATION_VERTOR_ADDR,		
	INTERRUPT_VERTOR_ADDR,		
	ERASE_BLOCK_SIZE,			
	WRITE_BLOCK_SIZE,			
	K15_STRING
};
/*
#ifdef FLASH_LOCATION
#pragma section="FlashConfig"
typedef union _FlashConfig_t
{
  unsigned int Data[4];
  struct {
    unsigned int BackDoorKey[2];
    unsigned char Fsec;             // FSEC 
    unsigned char Fopt;             // FOPT
    unsigned char Dfprot;           // DFPROT
    unsigned char Fprot;            // FPROT
    unsigned int Reverse;
  };
} FlashConfig_t;



#pragma location = "FlashConfig"
__root const FlashConfig_t Config @ "FlashConfig" =
{
 0xFFFFFFFF,
 0xFFFFFFFF, 
 0xA1FFFFFF, 
 0xFFFFFFFF,
};

#endif
*/
/**************************************************************/
void FC_Init( void );
void UART_Send(unsigned char *pData,uint16_t uiSize);
unsigned char FC_Communication( void );
void JumpToUserApplication(uint32_t userStartup);
void LONG_Convert(uint32_t *pLong);

void FC_Init( void )
{
	m_pRecFrame = (uint8_t *)&m_RecFrame;
	g_ucFC_State = FC_STATE_NULL;
	m_uiRecCount = 0;
}

void UART_Send(uint8_t *pData,uint16_t uiSize)
{
	unsigned int i;
	for(i=0;i<uiSize;i++)
	{
		uart0_putchar(TERM_PORT,pData[i]);
	}
}
	

unsigned char FC_Communication( void )
{
	uint8_t uiReadData,i;
	uint8_t *pAddress;
	ADDRESS_TYPE * pTempAddress;
	if(!(UART0_S1 & UART0_S1_RDRF_MASK))
	{
		return 0;
	}

	// read data from uart
	uiReadData = UART0_D;
	switch( g_ucFC_State )
	{
		case FC_STATE_NULL:
		{
			if( uiReadData == FC_CMD_ACK )
			{
				uart0_putchar( TERM_PORT,0xfc );
				g_ucFC_State = FC_STATE_WORKING;
			}
			else
			{
				return 0;
			}
		}
		break;
		case FC_STATE_WORKING:
			{ 
				switch( uiReadData )
				{
				case FC_CMD_IDENT:
					{
						uart0_putchar( TERM_PORT,m_MCU_Info.Version);
						uart0_putchar( TERM_PORT,m_MCU_Info.Sdid>>8);
						uart0_putchar( TERM_PORT,m_MCU_Info.Sdid);
						
						pTempAddress = (ADDRESS_TYPE *)&m_MCU_Info.BlocksCnt;
						for(i=0;i<7;i++)
						{
							uart0_putchar( TERM_PORT,pTempAddress[i].Bytes.hh);
							uart0_putchar( TERM_PORT,pTempAddress[i].Bytes.hl);
							uart0_putchar( TERM_PORT,pTempAddress[i].Bytes.lh);
							uart0_putchar( TERM_PORT,pTempAddress[i].Bytes.ll);
						}
						i = 0;
						do
						{
							uart0_putchar( TERM_PORT,m_MCU_Info.IdString[i]);
						}while(m_MCU_Info.IdString[i++]);
									
				}
				break;
				case FC_CMD_ERASE:
					{
						g_ucFC_State = FC_STATE_EREASE;
				}
				break;
				case FC_CMD_WRITE:
					{
						g_ucFC_State = FC_STATE_WRITE_ADDRESS;
				}
				break;
				case FC_CMD_READ:
					{
						g_ucFC_State = FC_STATE_READ;
				}
				break;
				case FC_CMD_QUIT:
					{
						 SCB_VTOR = RELOCATION_VERTOR_ADDR;
						 JumpToUserApplication(RELOCATION_VERTOR_ADDR);   
                                                // SCB_VTOR = 0x1ffffc00;
                                                // JumpToUserApplication(0x1ffffc00);  
				}
				break;
				default:
					//uart0_putchar( TERM_PORT,0xfc );
					break;
			}
			m_uiRecCount = 0;
		}
		break;
		
		case FC_STATE_EREASE:
			{
				m_pRecFrame[m_uiRecCount++] = uiReadData;
				if( m_uiRecCount >= sizeof(uint32_t) )
				{
					// erase
					LONG_Convert(&m_RecFrame.uiAddress);
					if(!Flash_SectorErase(m_RecFrame.uiAddress))
					{
						uart0_putchar( TERM_PORT,FC_CMD_ACK );
					}
					else
					{
						uart0_putchar( TERM_PORT,FC_CMD_NACK );
					}
					
					g_ucFC_State = FC_STATE_WORKING;
				}
		}
		break;
		case FC_STATE_WRITE_ADDRESS:
			{
				m_pRecFrame[m_uiRecCount++] = uiReadData;
				if( m_uiRecCount >= sizeof(uint32_t) )
				{
					g_ucFC_State = FC_STATE_WRITE_LEN;
				}
				
		}
		break;
		case FC_STATE_WRITE_LEN:
			{
				m_pRecFrame[m_uiRecCount++] = uiReadData;
                                g_ucFC_State = FC_STATE_WRITE_DATA;
		}
		break;
		case FC_STATE_WRITE_DATA:
			{
				m_pRecFrame[m_uiRecCount++] = uiReadData;
				if( m_uiRecCount > (m_RecFrame.Length + sizeof(uint32_t) ))
				{
					LONG_Convert(&m_RecFrame.uiAddress);
                    Memcpy_Byte((uint8_t *)&m_ucDataBuff[0],(uint8_t *)&m_RecFrame.DataBuff[0],m_RecFrame.Length);
			uiNumberCount ++;
                        if( uiNumberCount == 2)
                        {
                          uiNumberCount = 1;
                        }
                     
                                  if( !Flash_ByteProgram(m_RecFrame.uiAddress,
				   (uint32_t *)&m_ucDataBuff[0],m_RecFrame.Length) )
					{
						uart0_putchar( TERM_PORT,FC_CMD_ACK );
					}
					else
					{
						uart0_putchar( TERM_PORT,FC_CMD_NACK );
					}
                  
					g_ucFC_State = FC_STATE_WORKING;
				}
			}	
			break;
		case FC_STATE_READ:
			{
				m_pRecFrame[m_uiRecCount++] = uiReadData;
				if( m_uiRecCount > sizeof(uint32_t) )
				{
					LONG_Convert(&m_RecFrame.uiAddress);
					pAddress = (uint8_t *)m_RecFrame.uiAddress;
					for( i=0;i<m_RecFrame.Length;i++)
					{
						uart0_putchar( TERM_PORT,pAddress[i] );
					}
					g_ucFC_State = FC_STATE_WORKING;
				}
		}
		break;
		default:
			break;
		}
	return 1;
}

void JumpToUserApplication(uint32_t userStartup)
{
	#ifndef KEIL
    /* set up stack pointer */  
    asm("LDR     r1, [r0]");
    asm("mov     r13, r1");
    /* jump to application reset vector */
    asm("ADDS      r0,r0,#0x04 ");
    asm("LDR      r0, [r0]");
    asm("BX       r0");
	#else
	    /* set up stack pointer */ 
__asm
	{	
     LDR     R1, [R0]
     mov     R13, R1
    /* jump to application reset vector */
     ADDS      R0,R0,#0x04
     LDR      R0, [R0]
     BX       R0
  }
	#endif
}

void LONG_Convert(uint32_t *pLong)
{
	unsigned char *p;
	unsigned char ucTemp;
	p = ( unsigned char *)pLong;
	ucTemp = p[0];
	p[0] = p[3];
	p[3] = ucTemp;
	ucTemp = p[1];
	p[1] = p[2];
	p[2] = ucTemp;
}
void Memcpy_Byte(uint8_t * Dest,uint8_t * Src,uint32_t Size)
{
	while(Size--)
	{
		*Dest++ = *Src++;
	}
}
