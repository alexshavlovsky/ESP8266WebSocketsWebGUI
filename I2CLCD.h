#include "Arduino.h"
#include "brzo_i2c.h"
#include <inttypes.h>
#include "Print.h"

// commands
#define LCD_CLEARDISPLAY 0x01
#define LCD_RETURNHOME 0x02
#define LCD_ENTRYMODESET 0x04
#define LCD_DISPLAYCONTROL 0x08
#define LCD_CURSORSHIFT 0x10
#define LCD_FUNCTIONSET 0x20
#define LCD_SETCGRAMADDR 0x40
#define LCD_SETDDRAMADDR 0x80

// flags for display entry mode
#define LCD_ENTRYRIGHT 0x00
#define LCD_ENTRYLEFT 0x02
#define LCD_ENTRYSHIFTINCREMENT 0x01
#define LCD_ENTRYSHIFTDECREMENT 0x00

// flags for display on/off control
#define LCD_DISPLAYON 0x04
#define LCD_DISPLAYOFF 0x00
#define LCD_CURSORON 0x02
#define LCD_CURSOROFF 0x00
#define LCD_BLINKON 0x01
#define LCD_BLINKOFF 0x00

// flags for display/cursor shift
#define LCD_DISPLAYMOVE 0x08
#define LCD_CURSORMOVE 0x00
#define LCD_MOVERIGHT 0x04
#define LCD_MOVELEFT 0x00

// flags for function set
#define LCD_8BITMODE 0x10
#define LCD_4BITMODE 0x00
#define LCD_2LINE 0x08
#define LCD_1LINE 0x00
#define LCD_5x10DOTS 0x04
#define LCD_5x8DOTS 0x00

// flags for backlight control
#define LCD_BACKLIGHT 0x08
#define LCD_NOBACKLIGHT 0x00

#define En B00000100  // Enable bit
#define Rw B00000010  // Read/Write bit
#define Rs B00000001  // Register select bit

class I2CLCD : public Print {
public:
    I2CLCD(uint8_t lcd_Addr,uint8_t lcd_cols,uint8_t lcd_rows);
    I2CLCD(uint8_t lcd_Addr);
    void begin(uint8_t cols, uint8_t rows, uint8_t charsize = LCD_5x8DOTS);
    void clear();
    void home();
    void noDisplay();
    void display();
    void noBlink();
    void blink();
    void noCursor();
    void cursor();
    void scrollDisplayLeft();
    void scrollDisplayRight();
    void printLeft();
    void printRight();
    void leftToRight();
    void rightToLeft();
    void shiftIncrement();
    void shiftDecrement();
    void noBacklight();
    void backlight();
    void autoscroll();
    void noAutoscroll();
    void createChar(uint8_t, uint8_t[]);
    void setCursor(uint8_t, uint8_t);
    virtual size_t write(uint8_t);
    void command(uint8_t);
    void init();


private:
    void init_priv();
    void send(uint8_t, uint8_t);
    void write4bits(uint8_t);
    void expanderWrite(uint8_t);
    void pulseEnable(uint8_t);
    uint8_t _Addr;
    uint8_t _displayfunction;
    uint8_t _displaycontrol;
    uint8_t _displaymode;
    uint8_t _numlines;
    uint8_t _cols;
    uint8_t _rows;
    uint8_t _backlightval;
};


inline size_t I2CLCD::write(uint8_t value) {
    send(value, Rs);
    return 1;
}

I2CLCD::I2CLCD(uint8_t lcd_Addr,uint8_t lcd_cols,uint8_t lcd_rows) {
    _Addr = lcd_Addr;
    _cols = lcd_cols;
    _rows = lcd_rows;
    _backlightval = LCD_NOBACKLIGHT;
}

I2CLCD::I2CLCD(uint8_t lcd_Addr) {
    _Addr = lcd_Addr;
    _cols = 20;
    _rows = 4;
    _backlightval = LCD_NOBACKLIGHT;
}

void I2CLCD::init() {
    init_priv();
}

void I2CLCD::init_priv() {
    _displayfunction = LCD_4BITMODE | LCD_2LINE | LCD_5x8DOTS;
    begin(_cols, _rows);
}

void I2CLCD::begin(uint8_t cols, uint8_t lines, uint8_t dotsize) {
/*    if(lines > 1) {
        _displayfunction |= LCD_2LINE;
    }*/
    _numlines = lines;

    // for some 1 line displays you can select a 10 pixel high font
/*    if((dotsize != 0) && (lines == 1)) {
        _displayfunction |= LCD_5x10DOTS;
    }*/

    // SEE PAGE 45/46 FOR INITIALIZATION SPECIFICATION!
    // according to datasheet, we need at least 40ms after power rises above 2.7V
    // before sending commands. Arduino can turn on way befer 4.5V so we'll wait 50
    delay(50);

    // Now we pull both RS and R/W low to begin commands
    expanderWrite(_backlightval);	// reset expanderand turn backlight off (Bit 8 =1)
    delay(1000);

    //put the LCD into 4 bit mode
    // this is according to the hitachi HD44780 datasheet
    // figure 24, pg 46

    // we start in 8bit mode, try to set 4 bit mode
    write4bits(0x03 << 4);
    delayMicroseconds(4500); // wait min 4.1ms

    // second try
    write4bits(0x03 << 4);
    delayMicroseconds(4500); // wait min 4.1ms

    // third go!
    write4bits(0x03 << 4);
    delayMicroseconds(150);

    // finally, set to 4-bit interface
    write4bits(0x02 << 4);


    // set # lines, font size, etc.
    command(LCD_FUNCTIONSET | _displayfunction);

    // turn the display on with no cursor or blinking default
    _displaycontrol = LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF;
    display();

    // clear it off
    clear();

    // Initialize to default text direction (for roman languages)
    _displaymode = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;

    // set the entry mode
    command(LCD_ENTRYMODESET | _displaymode);

    home();

}

/********** high level commands, for the user! */
void I2CLCD::clear() {
    command(LCD_CLEARDISPLAY);// clear display, set cursor position to zero
    delayMicroseconds(2000);  // this command takes a long time!
}

void I2CLCD::home() {
    command(LCD_RETURNHOME);  // set cursor position to zero
    delayMicroseconds(2000);  // this command takes a long time!
}

void I2CLCD::setCursor(uint8_t col, uint8_t row) {
    int row_offsets[] = { 0x00, 0x40, 0x14, 0x54 };
    if(row > _numlines) {
        row = _numlines-1;    // we count rows starting w/0
    }
    command(LCD_SETDDRAMADDR | (col + row_offsets[row]));
}

// Turn the display on/off (quickly)
void I2CLCD::noDisplay() {
    _displaycontrol &= ~LCD_DISPLAYON;
    command(LCD_DISPLAYCONTROL | _displaycontrol);
}
void I2CLCD::display() {
    _displaycontrol |= LCD_DISPLAYON;
    command(LCD_DISPLAYCONTROL | _displaycontrol);
}

// Turns the underline cursor on/off
void I2CLCD::noCursor() {
    _displaycontrol &= ~LCD_CURSORON;
    command(LCD_DISPLAYCONTROL | _displaycontrol);
}
void I2CLCD::cursor() {
    _displaycontrol |= LCD_CURSORON;
    command(LCD_DISPLAYCONTROL | _displaycontrol);
}

// Turn on and off the blinking cursor
void I2CLCD::noBlink() {
    _displaycontrol &= ~LCD_BLINKON;
    command(LCD_DISPLAYCONTROL | _displaycontrol);
}
void I2CLCD::blink() {
    _displaycontrol |= LCD_BLINKON;
    command(LCD_DISPLAYCONTROL | _displaycontrol);
}

// These commands scroll the display without changing the RAM
void I2CLCD::scrollDisplayLeft(void) {
    command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVELEFT);
}
void I2CLCD::scrollDisplayRight(void) {
    command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVERIGHT);
}

// This is for text that flows Left to Right
void I2CLCD::leftToRight(void) {
    _displaymode |= LCD_ENTRYLEFT;
    command(LCD_ENTRYMODESET | _displaymode);
}

// This is for text that flows Right to Left
void I2CLCD::rightToLeft(void) {
    _displaymode &= ~LCD_ENTRYLEFT;
    command(LCD_ENTRYMODESET | _displaymode);
}

// This will 'right justify' text from the cursor
void I2CLCD::autoscroll(void) {
    _displaymode |= LCD_ENTRYSHIFTINCREMENT;
    command(LCD_ENTRYMODESET | _displaymode);
}

// This will 'left justify' text from the cursor
void I2CLCD::noAutoscroll(void) {
    _displaymode &= ~LCD_ENTRYSHIFTINCREMENT;
    command(LCD_ENTRYMODESET | _displaymode);
}

// Allows us to fill the first 8 CGRAM locations
// with custom characters
void I2CLCD::createChar(uint8_t location, uint8_t charmap[]) {
    location &= 0x7; // we only have 8 locations 0-7
    command(LCD_SETCGRAMADDR | (location << 3));
    for(int i=0; i<8; i++) {
        write(charmap[i]);
    }
}

// Turn the (optional) backlight off/on
void I2CLCD::noBacklight(void) {
    _backlightval=LCD_NOBACKLIGHT;
    expanderWrite(0);
}

void I2CLCD::backlight(void) {
    _backlightval=LCD_BACKLIGHT;
    expanderWrite(0);
}

/*********** mid level commands, for sending data/cmds */

inline void I2CLCD::command(uint8_t value) {
    send(value, 0);
}


/************ low level data pushing commands **********/

// write either command or data
void I2CLCD::send(uint8_t value, uint8_t mode) {
    uint8_t highnib=value&0xf0;
    uint8_t lownib=(value<<4)&0xf0;
    write4bits((highnib)|mode);
    write4bits((lownib)|mode);
}

void I2CLCD::write4bits(uint8_t value) {
    expanderWrite(value);
    pulseEnable(value);
}

void I2CLCD::expanderWrite(uint8_t _data) {
    if(enLcd) {
        uint8_t bf[2];
        brzo_i2c_start_transaction(_Addr, I2C_CLOCK_RATE);
        bf[0] = _data | _backlightval;
        brzo_i2c_write(bf, 1, false);
        if(brzo_i2c_end_transaction()!=0) enLcd=0;
    }
}

void I2CLCD::pulseEnable(uint8_t _data) {
    expanderWrite(_data | En);	// En high
    delayMicroseconds(1);		// enable pulse must be >450ns

    expanderWrite(_data & ~En);	// En low
    delayMicroseconds(50);		// commands need > 37us to settle
}

I2CLCD lcd(LCD_I2C_ADDRESS);

char LCDscr[81];
bool LCDdoinit=true;
int LCDscroll=4;

void LCDprintln(String s) {
//    Serial.println(s);
    if(LCDdoinit) {
        LCDdoinit=false;
        for(int i=0; i<80; i++) {
            LCDscr[i]=' ';
        }
        LCDscr[80]='\0';
    }
    for(int i=0; i<20; i++) {
        switch(LCDscroll) {
        case 4:
            LCDscr[i]=LCDscr[40+i];
        case 3:
            LCDscr[40+i]=LCDscr[20+i];
        case 2:
            LCDscr[20+i]=LCDscr[60+i];
        }
    }
    int z=s.length()-1;
    if(z>19) z=19;
    for(int i=0; i<20; i++) {
        if(i<=z) LCDscr[60+i]=s.charAt(i);
        else LCDscr[60+i]=' ';
    }
    lcd.home();
    lcd.print(LCDscr);
}

void LCDprintLeft(String s, uint8_t siz) {
    int l=s.length();
    if(l>siz) s=s.substring(0,siz);
    else for(int i=0; i<(siz-l); i++) s+=' ';
    lcd.print(s);
}

void LCDprintRight(String s, uint8_t siz) {
    int l=s.length();
    if(l>siz) s=s.substring(0,siz);
    else for(int i=0; i<(siz-l); i++) s=' '+s;
    lcd.print(s);
}

void initLCD() {
    lcd.init();
    lcd.backlight();
}
