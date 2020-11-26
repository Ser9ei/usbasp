#include <avr/io.h>
#include "clock.h"
#include <util/delay.h>
#include "isp.h"
#include "clock.h"
#include "usbasp.h"
#include "microwire.h"


void mwSendData(unsigned char data,unsigned char n)
{
	unsigned char i;
	for(i=0; i < n; i++)
	{
		if ((data >> (7-i)) & 1)
		{
			ISP_OUT |= (1 << ISP_MOSI);
		}
		else
		{
			ISP_OUT &= ~(1 << ISP_MOSI);
		}
		
		ISP_OUT |= (1 << ISP_SCK);
		ispDelay();
        ISP_OUT &= ~(1 << ISP_SCK);
		ispDelay();
	}
}


unsigned char mwReadByte()
{
	unsigned char dd,i;

    dd =0 ;
	for(i=0;i<8;i++)
	{
		dd = dd<<1;
		ISP_OUT |= (1 << ISP_SCK);
		ispDelay();
		ISP_OUT &= ~(1 << ISP_SCK);
		ispDelay();

		if ((ISP_IN & (1 << ISP_MISO)) != 0)
		{
				dd++;
		}

	}
	return dd;
}

void mwEnd()
{
	// set CS to 0
    ISP_OUT &= ~(1 <<ISP_RST);
	// set CLK to 0
                ISP_OUT &= ~(1 << ISP_SCK);
	ispDelay();
}

uchar mwBusy()
{
	CS_HI();
	_delay_us(10);
	ispDelay();
	if ((ISP_IN & (1 << ISP_MISO)) != 0)
	{
		CS_LOW();
		return(0);
	}
	else
	{
		CS_LOW();
		return(1); //����� ������
	}
}

unsigned char mwGetAdrLen()
{
	CS_HI();
	_delay_us(10);
	ispDelay();
	
	mwSendData(0xC0, 8); // 1 start bit + read opcode (10) + 5 zeros
	
	unsigned char dd,i;
	dd=0xFF;
  for(i=0;i<7;i++)
  {
    ISP_OUT |= (1 << ISP_SCK);  // set CLK to 1
	  ispDelay();
	  if ((ISP_IN & (1 << ISP_MISO)) == 0)
	  {
	    dd=(i+6);
	    ISP_OUT &= ~(1 << ISP_SCK); // set CLK to 0
	    ispDelay();
	    break;
	  }
	  ISP_OUT &= ~(1 << ISP_SCK); // set CLK to 0
	  ispDelay();
	}
	CS_LOW();
	return dd;

}