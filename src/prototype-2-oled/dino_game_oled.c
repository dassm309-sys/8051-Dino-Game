#include <reg51.h>

/* OLED I2C Mapping:
   P2.1 -> SCL
   P2.2 -> SDA
   
   Buttons (active LOW):
   P2.3 -> START
   P2.4 -> JUMP
   P2.5 -> RESTART
*/

sbit OLED_SCL    = P2^1;
sbit OLED_SDA    = P2^2;
sbit BTN_START   = P2^3;
sbit BTN_JUMP    = P2^4;
sbit BTN_RESTART = P2^5;

/* SSD1306 I2C address (0x3C << 1) */
#define OLED_ADDR  0x78

/* GAME VARIABLES */
bit game_running = 0;
bit dino_up      = 0;
unsigned char jump_timer = 0;
unsigned char obstacle_x = 120; // start from right
unsigned int  score      = 0;

/* SPRITES IN CODE MEMORY */
unsigned char code dino8[8] = {
    0x18, 0x3C, 0x7E, 0x5A,
    0x18, 0x18, 0x38, 0x00
};

unsigned char code cactus8[8] = {
    0x10, 0x10, 0x14, 0x14,
    0x7C, 0x10, 0x10, 0x00
};

/* 10 digits x 8 bytes */
unsigned char code digit8x8[10][8] = {
    {0x3C,0x66,0x6E,0x76,0x66,0x66,0x3C,0x00}, //0
    {0x18,0x38,0x18,0x18,0x18,0x18,0x7E,0x00}, //1
    {0x3C,0x66,0x06,0x0C,0x30,0x60,0x7E,0x00}, //2
    {0x3C,0x66,0x06,0x1C,0x06,0x66,0x3C,0x00}, //3
    {0x0C,0x1C,0x3C,0x6C,0x7E,0x0C,0x0C,0x00}, //4
    {0x7E,0x60,0x7C,0x06,0x06,0x66,0x3C,0x00}, //5
    {0x1C,0x30,0x60,0x7C,0x66,0x66,0x3C,0x00}, //6
    {0x7E,0x06,0x0C,0x18,0x30,0x30,0x30,0x00}, //7
    {0x3C,0x66,0x66,0x3C,0x66,0x66,0x3C,0x00}, //8
    {0x3C,0x66,0x66,0x3E,0x06,0x0C,0x38,0x00}  //9
};

/* UTILS */
void delay_ms(unsigned int ms) {
    unsigned int i, j;
    for (i = 0; i < ms; i++)
        for (j = 0; j < 120; j++);
}

/* I2C BITBANG */
void i2c_start(void) {
    OLED_SDA = 1; OLED_SCL = 1;
    OLED_SDA = 0;
    OLED_SCL = 0;
}

void i2c_stop(void) {
    OLED_SDA = 0; OLED_SCL = 1;
    OLED_SDA = 1;
}

void i2c_write(unsigned char dat) {
    unsigned char i;
    for (i = 0; i < 8; i++) {
        OLED_SDA = (dat & 0x80) ? 1 : 0;
        OLED_SCL = 1;
        OLED_SCL = 0;
        dat <<= 1;
    }
    /* ACK */
    OLED_SDA = 1;
    OLED_SCL = 1;
    OLED_SCL = 0;
}

/* OLED LOW-LEVEL */
void oled_cmd(unsigned char cmd) {
    i2c_start();
    i2c_write(OLED_ADDR);
    i2c_write(0x00); // command stream
    i2c_write(cmd);
    i2c_stop();
}

void oled_write_data(unsigned char val) {
    i2c_start();
    i2c_write(OLED_ADDR);
    i2c_write(0x40); // data stream
    i2c_write(val);
    i2c_stop();
}

void oled_setpos(unsigned char x, unsigned char page) {
    oled_cmd(0xB0 | (page & 0x07));
    oled_cmd(0x00 | (x & 0x0F));
    oled_cmd(0x10 | ((x >> 4) & 0x0F));
}

void oled_clear(void) {
    unsigned char page, col;
    for (page = 0; page < 8; page++) {
        oled_setpos(0, page);
        for (col = 0; col < 128; col++) {
            oled_write_data(0x00);
        }
    }
}

void oled_init(void) {
    delay_ms(100);
    oled_cmd(0xAE); // display off
    oled_cmd(0x20); oled_cmd(0x00); // horizontal addressing
    oled_cmd(0xB0);
    oled_cmd(0xC8);
    oled_cmd(0x00);
    oled_cmd(0x10);
    oled_cmd(0x40);
    oled_cmd(0x81); oled_cmd(0x7F);
    oled_cmd(0xA1);
    oled_cmd(0xA6);
    oled_cmd(0xA8); oled_cmd(0x3F);
    oled_cmd(0xA4);
    oled_cmd(0xD3); oled_cmd(0x00);
    oled_cmd(0xD5); oled_cmd(0xF0);
    oled_cmd(0xD9); oled_cmd(0x22);
    oled_cmd(0xDA); oled_cmd(0x12);
    oled_cmd(0xDB); oled_cmd(0x20);
    oled_cmd(0x8D); oled_cmd(0x14);
    oled_cmd(0xAF); // display on
    oled_clear();
}

void oled_draw_sprite(unsigned char x, unsigned char page, unsigned char code *spr) {
    unsigned char i;
    oled_setpos(x, page);
    for (i = 0; i < 8; i++)
        oled_write_data(spr[i]);
}

void oled_draw_digit(unsigned char x, unsigned char page, unsigned char d) {
    unsigned char i;
    if (d > 9) d = 0;
    oled_setpos(x, page);
    for (i = 0; i < 8; i++)
        oled_write_data(digit8x8[d][i]);
}

void oled_draw_score(unsigned int s) {
    unsigned char d0, d1, d2, d3;
    unsigned char c;
    d0 = (s / 1000) % 10;
    d1 = (s / 100) % 10;
    d2 = (s / 10) % 10;
    d3 = s % 10;

    /* clear score area (first 40px of page 0) */
    oled_setpos(0, 0);
    for (c = 0; c < 40; c++)
        oled_write_data(0x00);

    oled_draw_digit(0, 0, d0);
    oled_draw_digit(10, 0, d1);
    oled_draw_digit(20, 0, d2);
    oled_draw_digit(30, 0, d3);
}

/* MAIN */
void main(void) {
    unsigned char c;
    P2 |= 0x38; // Set P2.3, P2.4, P2.5 as inputs
    oled_init();
    oled_clear();

    game_running = 0;
    score = 0;

    while (1) {
        /* WAIT FOR START */
        if (!game_running) {
            if (BTN_START == 0) {
                delay_ms(40);
                if (BTN_START == 0) {
                    game_running = 1;
                    score = 0;
                    obstacle_x = 120;
                    dino_up = 0;
                    oled_clear();
                    oled_draw_score(score);
                }
            }
            continue;
        }

        /* RESTART CHECK */
        if (BTN_RESTART == 0) {
            delay_ms(40);
            if (BTN_RESTART == 0) {
                game_running = 0;
                oled_clear();
                continue;
            }
        }

        /* JUMP CHECK */
        if (BTN_JUMP == 0 && dino_up == 0) {
            delay_ms(20);
            if (BTN_JUMP == 0) {
                dino_up = 1;
                jump_timer = 5;
            }
        }

        /* CLEAR Game Rows (Page 5 and 6) */
        oled_setpos(0, 5);
        for (c = 0; c < 128; c++) oled_write_data(0x00);
        oled_setpos(0, 6);
        for (c = 0; c < 128; c++) oled_write_data(0x00);

        /* MOVE OBSTACLE */
        if (obstacle_x > 0) obstacle_x--;
        else obstacle_x = 120;

        /* JUMP LOGIC */
        if (dino_up) {
            if (jump_timer > 0) jump_timer--;
            else dino_up = 0;
        }

        /* DRAW OBJECTS */
        if (dino_up) oled_draw_sprite(0, 5, dino8);
        else oled_draw_sprite(0, 6, dino8);
        
        oled_draw_sprite(obstacle_x, 6, cactus8);

        /* COLLISION DETECTION */
        if (!dino_up) {
            if (obstacle_x <= 6) {
                oled_clear();
                oled_draw_score(score);
                game_running = 0;
                continue;
            }
        }

        /* SCORE UPDATE */
        score++;
        if (score > 9999) score = 0;
        oled_draw_score(score);

        delay_ms(80);
    }
}