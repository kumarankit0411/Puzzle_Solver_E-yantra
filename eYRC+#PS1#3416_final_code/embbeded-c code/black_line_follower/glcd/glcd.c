#include <stdarg.h>
#include "glcd.h"
#include "delay.h"



GLCD_Config GLCD;


/*************************************************************************************************
                                Local Functions
*************************************************************************************************/
static void glcd_CmdWrite( uint8_t var_lcdCmd_u8);
static void glcd_DataWrite( uint8_t var_data_u8);
static void glcd_BusyCheck();
static void glcd_SelectPage0();
static void glcd_SelectPage1();
/************************************************************************************************/
								


/**************************************************************************************************
void GLCD_Init
***************************************************************************************************
 * Function name:  GLCD_Init
 * I/P Arguments:  none
 * Return value    : none

 * description  :This function is used to initialize the GLCD.
                 
**************************************************************************************************/
void GLCD_Init()
{
	M_GlcdControlBusDirection = C_PortOutput_U8; /* Configure the data bus and Control bus as Output */
    M_GlcdDataBusDirection = C_PortOutput_U8;
	M_GlcdControlBusDirection1 = C_PortOutput_U8;


    /* Select the Page0/Page1 and Enable the GLCD */
    glcd_SelectPage0();
    glcd_CmdWrite(0x3F);
    glcd_SelectPage1();
    glcd_CmdWrite(0x3F);
    DELAY_ms(10);

    /* Select the Page0/Page1 and Enable the GLCD */
    glcd_SelectPage0();
    glcd_CmdWrite(0xc0);
    glcd_SelectPage1();
    glcd_CmdWrite(0xc0);

   /* Clear the complete LCD and move the cursor to beginning of page0*/
    GLCD_Clear();
}










/***************************************************************************************************
                         void GLCD_Clear()
 ***************************************************************************************************
 * I/P Arguments: none.
 * Return value    : none

 * description  :This function clears the LCD and moves the cursor to beginning of first line on page0
 ***************************************************************************************************/
void GLCD_Clear()
{
    uint8_t line,cursor,inversion;

    inversion = GLCD.Invertdisplay;
	GLCD.Invertdisplay = 0x00;
	
    for(line=0;line<8;line++)  /* loop through all the 8lines to clear the display */
    {
         GLCD_GoToLine(line);  /* Go to beginning of the specified line on page0 */
        
        for(cursor=0;cursor<128;cursor++) /* Clear all the 128 pixels of selected line */
        {
            if(cursor==64)  /* Has the cursor reached end of page0 */
            {
                GLCD_GoToPage(1); /*  then set it to beginning of page1 */
                
            }
            glcd_DataWrite(0x00); /* Clear each pixel by displaying blank */
        }
    }
	
	GLCD.Invertdisplay = inversion;

    GLCD_GoToLine(0);
}













/***************************************************************************************************
      void GLCD_SetCursor(uint8_t pageNumber,uint8_t lineNumber,uint8_t CursorPosition)
 ***************************************************************************************************
 * I/P Arguments: char row,char col
                 row -> line number(line1=1, line2=2),
                        For 2line LCD the I/P argument should be either 1 or 2.
                 col -> char number.
                        For 16-char LCD the I/P argument should be between 0-15.
 * Return value    : none

 * description  :This function moves the Cursor to specified position

                   Note:If the Input(Line/Char number) are out of range 
                        then no action will be taken
 ***************************************************************************************************/
 /* TODO: change the var names, Add logic for page handling */
void GLCD_SetCursor(uint8_t pageNumber,uint8_t lineNumber,uint8_t CursorPosition)
{
    if(    ((pageNumber == 0x00)   || (pageNumber == 0x01))
	    && ((lineNumber >=0x00)    && (lineNumber <= C_GlcdLastLine_U8))
	    && ((CursorPosition>=0x00) && (CursorPosition <= 63)) )
	  {
	    if(pageNumber==0x00)  /* Check for page number and set accordingly */
         {
           glcd_SelectPage0();
         }
        else
	     {
            glcd_SelectPage1();
	      }	 

        GLCD.PageNum = pageNumber; /* Keep the track of page selected */
        GLCD.LineNum=lineNumber | C_FirstLineNumberAddress_U8; /* Select the specified line number */
        GLCD.CursorPos=CursorPosition |0x40; /* Select the specified cursor position */
        glcd_CmdWrite(GLCD.CursorPos); /* Command the LCD to move to specified page,line,cursor*/
        glcd_CmdWrite(GLCD.LineNum);
	}
}







void GLCD_GetCursor(uint8_t *page_ptr,uint8_t *line_ptr,uint8_t *cursor_ptr)
{

    *page_ptr=GLCD.PageNum;
    *line_ptr=GLCD.LineNum;
    *cursor_ptr=GLCD.CursorPos;
}



/***************************************************************************************************
                      void GLCD_GoToPage(uint8_t pageNumber)
 ***************************************************************************************************
 * I/P Arguments: uint8_t: Line number.
 * Return value    : none

 * description  :This function moves the Cursor to beginning of the specified line.
        If the requested line number is out of range, it will not move the cursor.

     Note: The line numbers run from 1 to Maxlines,
 ***************************************************************************************************/
 /* TODO: change the desp and variable name */
void GLCD_GoToPage(uint8_t pageNumber)
{

    if((pageNumber==0) || (pageNumber ==1))
    { /* for 128/64 GLCD only page 0&1 are supported.
        Select the specified page and move the cursor accordingly */
        if(pageNumber == 0)
        {

            glcd_SelectPage0();
        }
        else
        {
            glcd_SelectPage1();
        }
        GLCD.PageNum=pageNumber;
        GLCD.CursorPos=0x40;
        glcd_CmdWrite(GLCD.LineNum);
        glcd_CmdWrite(GLCD.CursorPos);
    }
}







/***************************************************************************************************
                         void GLCD_GoToLine(uint8_t var_lineNumber_u8)
 ***************************************************************************************************
 * I/P Arguments: uint8_t: Line number.
 * Return value    : none

 * description  :This function moves the Cursor to beginning of the specified line.
        If the requested line number is out of range, it will not move the cursor.

     Note: The line numbers run from 0 to Maxlines-1, For 128x64 the line numbers will be 0-7
***************************************************************************************************/
/* Todo: All constants for the magic numbers */
void  GLCD_GoToLine(uint8_t var_lineNumber_u8)
{
    if(var_lineNumber_u8 <= C_GlcdLastLine_U8)
    {   /* If the line number is within range
         then move it to specified line on page0 and keep track*/
        GLCD.LineNum = var_lineNumber_u8+C_FirstLineNumberAddress_U8;
        GLCD_GoToPage(0);
    }
}





/***************************************************************************************************
                         void  GLCD_GoToNextLine()
 ***************************************************************************************************
 * I/P Arguments: none
 * Return value    : none

 * description  :This function moves the Cursor to beginning of the next line.
        If the cursor is on last line and NextLine command is issued then 
        it will move the cursor to first line.
 ***************************************************************************************************/
void  GLCD_GoToNextLine()
{
    /*Increment the current line number.
      In case it exceeds the limit, rool it back to first line */
    GLCD.LineNum++;
    if(GLCD.LineNum > C_LastLineNumberAddress_U8)
      GLCD.LineNum = C_FirstLineNumberAddress_U8;
    GLCD_GoToPage(0); /* Finally move it to next line on page0 */
}




void GLCD_EnableDisplayInversion()
{
	GLCD.Invertdisplay = 0xff;
}



void GLCD_DisableDisplayInversion()
{
	GLCD.Invertdisplay = 0x00;
}




/***************************************************************************************************
                       void GLCD_DisplayChar( char var_lcdData_u8)
 ***************************************************************************************************
 * I/P Arguments: ASCII value of the char to be displayed.
 * Return value    : none

 * description  : This function sends a character to be displayed on LCD.
                  Any valid ascii value can be passed to display respective character

 ***************************************************************************************************/
 /* Add the comments for decoding the character, Even offset handling*/
void GLCD_DisplayChar(uint8_t var_lcdData_u8)
{
    uint8_t dat;
	const uint8_t *ptr;

    if(((GLCD.PageNum == 0x01) && (GLCD.CursorPos>=0x7c)) || (var_lcdData_u8=='\n'))
    {
        /* If the cursor has reached to end of line on page1
        OR NewLine command is issued Then Move the cursor to next line */
        GLCD_GoToNextLine();
    }
       if(var_lcdData_u8!='\n') /* TODO */
    {
        //ptr= &ARR_GlcdFont_U8[var_lcdData_u8]; /* Get the address of the Character pattern from LookUp */
        while(1)
        {
            if((GLCD.PageNum == 0x00) && (GLCD.CursorPos==0x80))
            {
                /* If the cursor has reached to end of line on page0
                     Then Move the cursor to Page1 */
                GLCD_GoToPage(1);
            }

           // dat= pgm_read_byte(ptr++);/* Get the data to be displayed for LookUptable*/

            if(dat==0xff) /* Exit the loop if End of char is encountered */
                break;

            glcd_DataWrite(dat); /* Display the data and keep track of cursor */
            GLCD.CursorPos++;
        }
    }
}






/***************************************************************************************************
                       void GLCD_DisplayString(char *ptr_stringPointer_u8)
 ***************************************************************************************************
 * I/P Arguments: String(Address of the string) to be displayed.
 * Return value    : none

 * description  :
               This function is used to display the ASCII string on the lcd.
                 1.The ptr_stringPointer_u8 points to the first char of the string
                    and traverses till the end(NULL CHAR)and displays a char each time.

 ***************************************************************************************************/
#if (Enable_GLCD_DisplayString==1)
void GLCD_DisplayString(char *ptr_stringPointer_u8)
{
    while((*ptr_stringPointer_u8)!=0)
        GLCD_DisplayChar(*ptr_stringPointer_u8++); // Loop through the string and display char by char
}
#endif


/***************************************************************************************************
                       static void glcd_CmdWrite( uint8_t var_lcdCmd_u8)
 ***************************************************************************************************
 * I/P Arguments: 8-bit command supported by LCD.
 * Return value    : none

 * description :This function sends a command to GLCD.
                Some of the commonly used commands are defined in lcd.h.
                For more commands refer the data sheet and send the supported command.                
                The behavior is undefined if unsupported commands are sent.    
 ***************************************************************************************************/
static void glcd_CmdWrite( uint8_t var_cmd_u8)
{
    glcd_BusyCheck();
    M_GlcdDataBus = var_cmd_u8;
    M_GlcdClearBit(M_GlcdControlBus,GLCD_RS);           // Select the Command Register by pulling RS LOW
#ifdef GLCD_RW
    M_GlcdClearBit(M_GlcdControlBus,GLCD_RW);           // Select the Write Operation  by pulling RW LOW
#endif
    M_GlcdSetBit(M_GlcdControlBus,GLCD_EN);             // Send a High-to-Low Pulse at Enable Pin
    DELAY_us(2);
    M_GlcdClearBit(M_GlcdControlBus,GLCD_EN);
}






/*************************************************************************************************
                       static void glcd_DataWrite( uint8_t dat)
 *************************************************************************************************
 * I/P Arguments: uint8_t: 8-bit value to be sent to LCD.
 * Return value    : none

 * description : This functions is used to send a byte of data to LCD.                 .    
 *************************************************************************************************/
static void glcd_DataWrite( uint8_t var_data_u8)
{
    glcd_BusyCheck();
    M_GlcdDataBus = var_data_u8 ;
    M_GlcdSetBit(M_GlcdControlBus,GLCD_RS);           // Select the Data Register by pulling RS High
#ifdef GLCD_RW
    M_GlcdClearBit(M_GlcdControlBus,GLCD_RW);           // Select the Write Operation  by pulling RW LOW
#endif
    M_GlcdSetBit(M_GlcdControlBus,GLCD_EN);             // Send a High-to-Low Pulse at Enable Pin
    DELAY_us(2);
    M_GlcdClearBit(M_GlcdControlBus,GLCD_EN);
}







/*************************************************************************************************
                       static void glcd_BusyCheck()
 *************************************************************************************************
 * I/P Arguments: none.
 * Return value    : none

 * description : This functions is used check whether LCD is busy.
                 It waits till the LCD is busy by polling the LCD busy flag.
                 After completing the previous operation, LCDs clears its internal busy flag.
 *************************************************************************************************/
static void glcd_BusyCheck()
{
    uint8_t busyflag;
    
#ifdef GLCD_RW                    //Perform Busy check if GLCD_RW pin is used

    util_UpdateBit(M_GlcdDataBusDirection,GLCD_D7,C_PinInput_U8); // Configure busy pin as input
    M_GlcdClearBit(M_GlcdControlBus,GLCD_RS);           // Select the Command Register by pulling RS LOW
    M_GlcdSetBit(M_GlcdControlBus,GLCD_RW);             // Select the Read Operation for busy flag by setting RW
    do
    {
        M_GlcdClearBit(M_GlcdControlBus,GLCD_EN);             // Send a High-to-Low Pulse at Enable Pin
        DELAY_us(2);    
        M_GlcdSetBit(M_GlcdControlBus,GLCD_EN);
        DELAY_us(2);
        busyflag = util_GetBitStatus(M_GlcdDataBusInput,GLCD_D7);
    }while(busyflag);

    util_UpdateBit(M_GlcdDataBusDirection,GLCD_D7,C_PinOutput_U8);
#else
    /* Busy flag cannot be read as GLCD_RW is not available hence Extra delay of 1ms is added 
      to ensure the LCD completes previous operation and ready to receive new commands/data */
    DELAY_ms(1);  
#endif
}


static void glcd_SelectPage0() 
 { 
   M_GlcdSetBit(M_GlcdControlBus1,GLCD_CS1); 
   M_GlcdClearBit(M_GlcdControlBus1,GLCD_CS2); 
 }

static void glcd_SelectPage1() 
 {
   M_GlcdSetBit(M_GlcdControlBus1,GLCD_CS2);  
   M_GlcdClearBit(M_GlcdControlBus1,GLCD_CS1); 
 }




