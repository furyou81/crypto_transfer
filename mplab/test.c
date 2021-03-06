	#define MIN(a, b)				(((a) > (b)) ? (b) : (a))
	#define MAX(a, b)				(((a) > (b)) ? (a) : (b))

typedef enum
	{
		FALSE=0,
		TRUE
	} bool_e;

/* Type de base pour le dsPIC */
	typedef unsigned char Uint8;
	typedef signed char Sint8;
	typedef unsigned short Uint16;
	typedef signed short Sint16;
	typedef unsigned long Uint32;
	typedef signed long Sint32;

  #ifndef LCD_MIDAS_4X20_H
#define	LCD_MIDAS_4X20_H

#define RESET_PIN	LATGbits.LATG7
#define RESET_TRIS	TRISGbits.TRISG7

/*
	These functions must be called once prior to others, in the right order :
*/
void LCD_I2C_init(void);		//First init : I2C
void LCD_init(void);			//Second init : LCD


/*
	Write the text in "string" (until the '\0') at the current cursor position
*/
void LCD_Write_text(char * string);

/*
	If you need to change the contrast. 7 is a good choice in my case.
*/
void LCD_set_contrast(Uint8 contrast);

/*
	line : from 0 to LCD_SIZE_LINE-1
	column : from 0 to LCD_SIZE_COLUMN-1
*/
void LCD_set_cursor(Uint8 line, Uint8 column);

/*
	To show or mask cursor... If show, to blink or not it
*/
void LCD_cursor_display( bool_e show, bool_e blink);

/*
	Test process, should be called in loop...
*/
void LCD_test(void);

/*
	Clear the display
*/
void LCD_clear_display(void);

#endif	/* LCD_MIDAS_4X20_H */

#include "QS/QS_all.h"
#include <i2c.h>

#define LCDADDR 0x78
#define LCD_SIZE_LINE	4
#define LCD_SIZE_COLUMN	20
 

#define CONTROL_BYTE_FOR_DATA						0x40
#define CONTROL_BYTE_FOR_COMMAND					0x00

#define COMMAND_CLEAR_DISPLAY 						0x01
#define COMMAND_8BIT_4LINES_NORMAL_RE1_IS0			0x3A	//Extended command access RE = 1
#define COMMAND_8BIT_4LINES_REVERSE_RE1_IS0			0x3B	//Extended command access RE = 1
#define COMMAND_8BIT_4LINES_RE0_IS1					0x39	//Extented command access IS = 1
#define COMMAND_8BIT_4LINES_RE0_IS0					0x38	//Normal mode...


//Command from extended set (RE = 1, IS = 0)
#define COMMAND_BS1_1								0x1E
#define COMMAND_POWER_DOWN_DISABLE					0x02
#define COMMAND_SEGMENT_NORMAL_WAY					0x05
#define COMMAND_NW									0x09

//Command from extended set (RE = 0, IS = 1)
#define COMMAND_DISPLAY_ON_CURSOR_ON_BLINK_ON		0x0F
#define COMMAND_DISPLAY_ON_CURSOR_ON_BLINK_OFF		0x0E
#define COMMAND_DISPLAY_ON_CURSOR_OFF_BLINK_OFF		0x0C
#define COMMAND_BS0_1								0x1C
#define COMMAND_INTERNAL_DIVIDER					0x13
#define COMMAND_CONTRAST							0x77
#define COMMAND_POWER_ICON_CONTRAST					0x5C
#define COMMAND_FOLLOWER_CONTROL					0x6E


#define ADDRESS_DDRAM								0x80





//////////////////////////////////////////////I2C : low level functions //////////////////////

void LCD_I2C_init(void)
{
	// Baud rate is set for 100 Khz
	Uint16 config2 = 100;
	  // Configure I2C for 7 bit address mode
	Uint16 config1 = (	I2C_ON & I2C_IDLE_CON & I2C_CLK_HLD &
				I2C_IPMI_DIS & I2C_7BIT_ADD &
				I2C_SLW_DIS & I2C_SM_DIS &
				I2C_GCALL_DIS & I2C_STR_DIS &
				I2C_ACK & I2C_ACK_DIS & I2C_RCV_DIS &
				I2C_STOP_DIS & I2C_RESTART_DIS &
				I2C_START_DIS);
	OpenI2C(config1,config2);
	ConfigIntI2C(MI2C_INT_OFF & MI2C_INT_PRI_3
				& SI2C_INT_OFF & SI2C_INT_PRI_5);
}


void LCD_I2C_send_byte(Uint8 c)
{
	IdleI2C();
	MasterWriteI2C(c);
	IdleI2C();
	if(I2CSTATbits.ACKSTAT)
		debug_printf("NO_ACK\n");
}

//rw : 0 to write, 1 to read
typedef enum
{
	WRITING_FRAME = 0,
	READING_FRAME = 1
}read_write_e;

void LCD_I2C_begin_frame(read_write_e rw)
{
	IdleI2C();
	StartI2C();
	LCD_I2C_send_byte(LCDADDR | rw);		// LCD's I2C slave address
}



void LCD_I2C_end_frame(void)
{
	IdleI2C();
	StopI2C();
}



Uint8 LCD_I2C_read_byte(bool_e send_ack)
{
	Uint8 ret;
	IdleI2C();
	ret = MasterReadI2C();
	IdleI2C();
	if(send_ack)
	{
		I2CCONbits.ACKDT = 0;
		I2CCONbits.ACKEN = 1;
	}
	return ret;
}

////////////////////////////////////////////////////////////////////////
/// PRIVATE FUNCTIONS : low level for LCD


void LCD_send_command(Uint8 command)
{
	LCD_I2C_begin_frame(WRITING_FRAME);
	LCD_I2C_send_byte(CONTROL_BYTE_FOR_COMMAND);		// Control byte (C0 = 0, D/C = C)
	LCD_I2C_send_byte(command);
	LCD_I2C_end_frame();
}


void LCD_clear_display(void)
{
	LCD_send_command(COMMAND_CLEAR_DISPLAY);	//Clear display
}

/*
	Initialize the LCD.
*/
void LCD_init(void)
{
	Uint32 i;
	RESET_TRIS = 0;
	for(i=0;i<50000;i++);	//Delay > 5ms.
	RESET_PIN = 0;
	for(i=0;i<100000;i++);	//Delay > 10ms.
	RESET_PIN = 1;
	for(i=0;i<10000;i++);	//Delay > 1ms

	LCD_send_command(COMMAND_CLEAR_DISPLAY);	//Clear display

	//RE = 1  |  IS = 0
	LCD_send_command(COMMAND_8BIT_4LINES_NORMAL_RE1_IS0);		//Function set : 8bit 4line RE = 1, IS = 0, Not reverse
	//LCD_Command(COMMAND_8BIT_4LINES_REVERSE_RE1_IS0);	//Function set : RE = 1, REVERSE
	LCD_send_command(COMMAND_BS1_1);	//Set BS1 = 1 (1/6 bias)
	LCD_send_command(COMMAND_POWER_DOWN_DISABLE);	//Power down disable
	LCD_send_command(COMMAND_SEGMENT_NORMAL_WAY);	// Segment bidirection : in the right way
	LCD_send_command(COMMAND_NW);	//NW = 1

	//RE = 0  |  IS = 1
	LCD_send_command(COMMAND_8BIT_4LINES_RE0_IS1);	//Function set : RE = 0, IS = 1
	LCD_send_command(COMMAND_DISPLAY_ON_CURSOR_ON_BLINK_ON);	//Display ON (with cursor...)
	LCD_send_command(COMMAND_BS0_1);	//Set BS0 = 1 (1/6 bias) / Internal frequency..
	LCD_send_command(COMMAND_INTERNAL_DIVIDER);	//Internal divider
	LCD_send_command(COMMAND_CONTRAST);	//Constrast
	LCD_send_command(COMMAND_POWER_ICON_CONTRAST);	//Power Icon control contrast
	LCD_send_command(COMMAND_FOLLOWER_CONTROL);	//Follower Control

	//RE = 0  | IS = 0
	LCD_send_command(COMMAND_8BIT_4LINES_RE0_IS0);	//Function set : 8bit 4line RE = 0, IS = 0

}

////////////////////////////////////////////////////////////////////////


//Contrast : 0 to 63.. (in my case, < 10 is a good choice)
void LCD_set_contrast(Uint8 contrast)
{
	LCD_send_command(COMMAND_8BIT_4LINES_RE0_IS1);	//Function set : RE = 0, IS = 1
	LCD_send_command(0x70 | (contrast & 0x0F));			//Constrast
	LCD_send_command(COMMAND_POWER_ICON_CONTRAST | ((contrast >> 4) & 0x03));	//Power Icon control contrast
	LCD_send_command(COMMAND_8BIT_4LINES_RE0_IS0);	//Function set : RE = 0, IS = 0
}

#define SIZE_READ 160
//Function used only for debug...
void LCD_Dump(void)
{
	Uint8 ret[SIZE_READ],i;
	printf("READ %x ->\n  0000 ",(int)(0));

	LCD_send_command(0);

	LCD_I2C_begin_frame(READING_FRAME);
	LCD_I2C_send_byte(CONTROL_BYTE_FOR_COMMAND);		// Control byte (C0 = 0, D/C = C)

	for(i=0;i<SIZE_READ;i++)
	{
		ret[i] = LCD_I2C_read_byte((i<SIZE_READ-1)?TRUE:FALSE);	//Send Ack	-> NO ack at the last byte
	}

	LCD_I2C_end_frame();

	for(i=0;i<SIZE_READ;i++)
	{
		if(i == 0)
			printf("xx ");
		else
			printf("%02X ", ret[i]);
		if(i%8 == 7)
			printf("\n  %04X ", i+1);
	}
	printf("\n");
}

void LCD_cursor_display(bool_e show, bool_e blink)
{
	LCD_send_command(COMMAND_8BIT_4LINES_RE0_IS1);	//Function set : RE = 0, IS = 1
	LCD_send_command(0x0C | ((show)?0x02:0x00) | ((blink)?0x01:0x00));
	LCD_send_command(COMMAND_8BIT_4LINES_RE0_IS0);	//Function set : RE = 0, IS = 0
}

/*
	line : from 0 to LCD_SIZE_LINE-1
	column : from 0 to LCD_SIZE_COLUMN-1
*/
void LCD_set_cursor(Uint8 line, Uint8 column)
{
	line = MIN(line,LCD_SIZE_LINE-1);
	column = MIN(column,LCD_SIZE_COLUMN-1);

	LCD_send_command(ADDRESS_DDRAM | (line*0x20 + column));
}


Uint8 Read_AC(void)
{
	Uint8 ac, id;
	LCD_I2C_begin_frame(WRITING_FRAME);
	LCD_I2C_send_byte(CONTROL_BYTE_FOR_COMMAND);
	LCD_I2C_end_frame();

	LCD_I2C_begin_frame(READING_FRAME);
	ac = LCD_I2C_read_byte(TRUE);
	id = LCD_I2C_read_byte(FALSE);	//id should be 0x1A
	LCD_I2C_end_frame();

	//printf("ac=%02X | id=%02X\n",ac,id);
	return ac;
}

/*
 * string ends with '\0'...
 */
void LCD_Write_text(char * string)
{
	Uint16 i;

	LCD_I2C_begin_frame(WRITING_FRAME);
	LCD_I2C_send_byte(CONTROL_BYTE_FOR_DATA);     // Control byte for Data

	for(i=0;string[i];i++)
	{
		switch(string[i])	//For special characters
		{
			case '�':	LCD_I2C_send_byte(0x5E);	break;
			case '�':	LCD_I2C_send_byte(0xA5);	break;
			case '�':	LCD_I2C_send_byte(0x7B);	break;
			case '�':	LCD_I2C_send_byte(0x7F);	break;
			//case '':	LCD_I2C_send_byte(0x86);	break;
			case '�':	LCD_I2C_send_byte(0xA9);	break;
			case '�':	LCD_I2C_send_byte(0xC7);	break;
			//case '�':	LCD_I2C_send_byte(0x89);	break;
			case '�':	LCD_I2C_send_byte(0xA4);	break;
			//case '�':	LCD_I2C_send_byte(0x8B);	break;
			//case '�':	LCD_I2C_send_byte(0x8C);	break;
			//case '':	LCD_I2C_send_byte(0x8D);	break;
			//case '�':	LCD_I2C_send_byte(0x8E);	break;
			case '|':	LCD_I2C_send_byte(0xFE);	break;
			case (char)(0xFF):	LCD_I2C_send_byte(0x1F);	break;	//FULL SQUARE

			default:
				LCD_I2C_send_byte(string[i]);
			break;
		}
	}

	LCD_I2C_end_frame();
}

/*
This function must be called PERIODICALLY..
 */
void LCD_test(void)
{
	Uint8 i;

	static bool_e done = FALSE;
	static Uint32 n = 0;
	char tmp[10];
	i = 0;
	if(!done)
	{
		LCD_cursor_display(FALSE,FALSE);
		LCD_set_cursor(0, 0);
		LCD_Write_text("Bonjour !");
		done = TRUE;
		LCD_set_cursor(1, 3);
		LCD_Write_text("Ligne 2");

		LCD_set_cursor(2, 0);
		LCD_Write_text("D�but Ligne 3");

		LCD_set_cursor(29, 0);
		LCD_Write_text("�������...!");
	}
	else
	{
		LCD_set_cursor(0, 10);
		n++;
		sprintf(tmp, "%ld", n);
		LCD_Write_text(tmp);
	}

}

int     main()
{

    return 0;
}