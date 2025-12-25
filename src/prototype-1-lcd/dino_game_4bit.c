#include <reg51.h>

/* LCD Connections */
#define LCD_DATA_PORT P2
sbit RS = P3^5;
sbit EN = P3^7;

/* Controls */
sbit BTN_JUMP    = P1^0;
sbit BUZZER      = P1^1;
sbit BTN_RESTART = P1^2;
sbit BTN_START   = P1^3;

/* Globals */
unsigned char code dino[8] = {0x00, 0x07, 0x05, 0x17, 0x1C, 0x1F, 0x0D, 0x0C};
unsigned char code tree[8] = {0x03, 0x1B, 0x1B, 0x1B, 0x1B, 0x1F, 0x0E, 0x0E};

bool inMenu = true;
bool dinoOnGround = true;
bool playState = false;
unsigned int score = 0;
unsigned int highScore = 0;
unsigned char prng_seed = 1;

void delay_us(unsigned int us) { while(us--) { /* Calibrated for ~1us at 12MHz */ } }

void delay_ms(unsigned int ms) {
    unsigned int i, j;
    for (i = 0; i < ms; i++)
        for (j = 0; j < 120; j++);  // ~1ms at 11.0592MHz
}

void lcd_pulse() { 
    EN = 1; 
    delay_us(5); 
    EN = 0; 
    delay_ms(1); 
}

void lcd_cmd(unsigned char cmd) {
    unsigned char temp;
    temp = cmd & 0xF0;
    LCD_DATA_PORT = (LCD_DATA_PORT & 0x0F) | temp;
    RS = 0;
    lcd_pulse();
    temp = (cmd << 4) & 0xF0;
    LCD_DATA_PORT = (LCD_DATA_PORT & 0x0F) | temp;
    RS = 0;
    lcd_pulse();
}

void lcd_data(unsigned char dat) {
    unsigned char temp;
    temp = dat & 0xF0;
    LCD_DATA_PORT = (LCD_DATA_PORT & 0x0F) | temp;
    RS = 1;
    lcd_pulse();
    temp = (dat << 4) & 0xF0;
    LCD_DATA_PORT = (LCD_DATA_PORT & 0x0F) | temp;
    RS = 1;
    lcd_pulse();
}

void lcd_init() {
    delay_ms(20);
    LCD_DATA_PORT = (LCD_DATA_PORT & 0x0F) | 0x30; RS = 0; lcd_pulse(); delay_ms(5);
    lcd_pulse(); delay_ms(1); lcd_pulse(); delay_ms(1);
    LCD_DATA_PORT = (LCD_DATA_PORT & 0x0F) | 0x20; lcd_pulse();
    lcd_cmd(0x28); // 4-bit mode, 2-line, 5x7 font
    lcd_cmd(0x0C); // Display ON, Cursor OFF
    lcd_cmd(0x06); // Entry Mode
    lcd_cmd(0x01); // Clear Display
    delay_ms(2);
}

void lcd_clear() {
    lcd_cmd(0x01);
}

void lcd_gotoxy(unsigned char row, unsigned char col) {
    unsigned char row_addr[] = {0x80, 0xC0, 0x94, 0xD4};
    lcd_cmd(row_addr[row] + col);
}

void lcd_puts(char *s) {
    while (*s) {
        lcd_data(*s++);
    }
}

/* HELPER FUNCTIONS */

unsigned char rand_8bit() {
    prng_seed ^= prng_seed << 1;
    prng_seed ^= prng_seed >> 1;
    prng_seed ^= prng_seed << 2;
    return prng_seed;
}

int random(int min, int max) {
    return (rand_8bit() % (max - min + 1)) + min;
}

void lcd_print_number(unsigned int n) {
    char buffer[6];
    int i = 4;
    buffer[5] = '\0';
    if (n == 0) {
        lcd_data('0');
        return;
    }
    while (n > 0) {
        buffer[i--] = (n % 10) + '0';
        n /= 10;
    }
    lcd_puts(&buffer[i+1]);
}

void beep(void) {
    BUZZER = 1;
    delay_ms(100);
    BUZZER = 0;
}

void lcd_load_chars() {
    unsigned char i;
    lcd_cmd(0x40 + (6*8)); for(i=0; i<8; i++) lcd_data(tree[i]);
    lcd_cmd(0x40 + (7*8)); for(i=0; i<8; i++) lcd_data(dino[i]);
}

/* GAME FUNCTIONS */

void startDinoGame() {
    int i, dist, distTwo;
    playState = true;
    score = 0;
    lcd_clear();
    lcd_load_chars();

    while (playState) {
        dist = random(4, 9);
        distTwo = random(4, 9);

        for (i = 15; i >= -(dist + distTwo); i--) {
            lcd_gotoxy(0, 13);
            lcd_print_number(score);

            if (BTN_JUMP == 0) {
                beep();
                lcd_gotoxy(0, 1);
                lcd_data(7);      // Write dino (char 7)
                lcd_gotoxy(1, 1);
                lcd_data(' ');    // Clear ground
                dinoOnGround = false;
            } else {
                lcd_gotoxy(1, 1);
                lcd_data(7);      // Write dino (char 7)
                lcd_gotoxy(0, 1);
                lcd_data(' ');    // Clear air
                dinoOnGround = true;
            }

            lcd_gotoxy(1, i);
            lcd_data(6);          // Write tree (char 6)
            lcd_gotoxy(1, i + dist);
            lcd_data(6);
            lcd_gotoxy(1, i + dist + distTwo);
            lcd_data(6);

            if ((i == 1 || (i + dist) == 1 || (i + dist + distTwo) == 1) && dinoOnGround) {
                lcd_clear();
                lcd_gotoxy(0, 0);
                lcd_puts("GAME OVER");
                if (score > highScore) highScore = score;
                lcd_gotoxy(1, 0);
                lcd_puts("High:");
                lcd_print_number(highScore);
                delay_ms(2000);
                playState = false;
                inMenu = true;
                lcd_clear();
                return;
            }

            score++;
            delay_ms(300);

            if (BTN_RESTART == 0) {
                playState = false;
                inMenu = true;
                lcd_clear();
                return;
            }
        }
    }
}

void startTerrainGame() {
    int pos = 0;
    lcd_clear();
    lcd_gotoxy(0, 0);
    lcd_puts("Terrain Game");
    delay_ms(1000);
    lcd_clear();

    while (true) {
        lcd_clear();
        lcd_gotoxy(1, pos);
        lcd_data('#');
        lcd_gotoxy(0, 1);
        lcd_puts("RUN!");
        delay_ms(100);

        pos--;
        if (pos < 0) pos = 15;

        if (BTN_RESTART == 0) {
            inMenu = true;
            lcd_clear();
            return;
        }
    }
}

void showMenu() {
    lcd_clear();
    lcd_gotoxy(0, 0);
    lcd_puts("Select Game:");
    lcd_gotoxy(1, 0);
    lcd_puts("1.Dino 2.Terrain");
    delay_ms(200);

    if (BTN_JUMP == 0) {
        delay_ms(200);
        startDinoGame();
    }
    if (BTN_START == 0) {
        delay_ms(200);
        startTerrainGame();
    }
}

void main(void) {
    P1 = 0xFF; // Set P1 as input
    BUZZER = 0;
    lcd_init();
    lcd_load_chars();
    prng_seed = 1;

    while (1) {
        if (inMenu) {
            showMenu();
        }
    }
}