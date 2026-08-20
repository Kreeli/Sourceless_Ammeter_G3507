#include "oled_spi.h"
#include "oledfont.h"
unsigned int OLED_GRAM[128][8];
// 向SSD1306写入一个字节
// dat: 要写入的数据/命令内容
// cmd: 命令类型标志 0-命令模式，1-数据模式
void OLED_WR_Byte(u8 dat,u8 cmd)
{
    u8 i;
    if(cmd)
      OLED_DC_Set();
    else
      OLED_DC_Clr();
    OLED_CS_Clr();

      //发送数据
      DL_SPI_transmitData8(SPI_OLED_INST, dat);
      //等待SPI总线空闲
      while(DL_SPI_isBusy(SPI_OLED_INST));

    OLED_CS_Set();
    OLED_DC_Set();
}

void OLED_Set_Pos(unsigned char x, unsigned char y)
{
    OLED_WR_Byte(0xb0 + y, OLED_CMD);
    OLED_WR_Byte(((x & 0xf0) >> 4) | 0x10, OLED_CMD);
    OLED_WR_Byte((x & 0x0f) | 0x01, OLED_CMD);
}
// 开启OLED显示
void OLED_Display_On(void)
{
    OLED_WR_Byte(0X8D, OLED_CMD); // SET DCDC命令
    OLED_WR_Byte(0X14, OLED_CMD); // DCDC ON
    OLED_WR_Byte(0XAF, OLED_CMD); // DISPLAY ON
}
// 关闭OLED显示
void OLED_Display_Off(void)
{
    OLED_WR_Byte(0X8D, OLED_CMD); // SET DCDC命令
    OLED_WR_Byte(0X10, OLED_CMD); // DCDC OFF
    OLED_WR_Byte(0XAE, OLED_CMD); // DISPLAY OFF
}
// 清空显存并刷新屏幕，执行后屏幕全黑
void OLED_Clear(void)
{
    u8 i, n;
    for (i = 0; i < 8; i++)
    {
        for (n = 0; n < 128; n++)
            OLED_GRAM[n][i] = 0;
    }
    OLED_Refresh();
}

// 在指定位置显示一个字符,包括部分字符
// x:0~127
// y:0~7 (页地址)
void OLED_ShowChar(u8 x, u8 y, char chr)
{
    unsigned char c = 0, i = 0;
    c = chr - ' '; // 得到偏移后的值
    if (x > Max_Column - 1)
    {
        x = 0;
        y = y + 2;
    }
    if (SIZE == 16)
    {
        for (i = 0; i < 8; i++)
            OLED_GRAM[x + i][y] = F8X16[c * 16 + i];
        for (i = 0; i < 8; i++)
            OLED_GRAM[x + i][y + 1] = F8X16[c * 16 + i + 8];
    }
    else
    {
        for (i = 0; i < 6; i++)
            OLED_GRAM[x + i][y] = F6x8[c][i];
    }
}
// m^n函数
u32 oled_pow(u8 m, u8 n)
{
    u32 result = 1;
    while (n--)
        result *= m;
    return result;
}
// 显示数字
// x,y :起点坐标
// len :数字的位数
// size2:字体大小
// num:数值(0~4294967295);
void OLED_ShowNum(u8 x, u8 y, u32 num, u8 len, u8 size2)
{
    u8 t, temp;
    u8 enshow = 0;
    for (t = 0; t < len; t++)
    {
        temp = (num / oled_pow(10, len - t - 1)) % 10;
        if (enshow == 0 && t < (len - 1))
        {
            if (temp == 0)
            {
                OLED_ShowChar(x + (size2 / 2) * t, y, ' ');
                continue;
            }
            else
                enshow = 1;
        }
        OLED_ShowChar(x + (size2 / 2) * t, y, temp + '0');
    }
}
// 显示一个字符号串
void OLED_ShowString(u8 x, u8 y, char *chr)
{
    unsigned char j = 0;
    while (chr[j] != '\0')
    {
        OLED_ShowChar(x, y, chr[j]);
        x += 8;
        if (x > 120)
        {
            x = 0;
            y += 2;
        }
        j++;
    }
}
// 显示汉字
void OLED_ShowCHinese(u8 x, u8 y, u8 no)
{
    u8 t;
    for (t = 0; t < 16; t++)
    {
        OLED_GRAM[x + t][y] = Hzk[2 * no][t];
    }
    for (t = 0; t < 16; t++)
    {
        OLED_GRAM[x + t][y + 1] = Hzk[2 * no + 1][t];
    }
}

/***********功能描述：显示显示BMP图片128×64起始点坐标(x,y),x的范围0～127，y为需要使用的页的范围1～8*****************/
void OLED_DrawBMP(unsigned char x0, unsigned char y0, unsigned char x1, unsigned char y1, const unsigned char BMP[])
{
    unsigned int j = 0;
    unsigned char x, y;
    unsigned char page_start = y0 / 8;
    unsigned char page_end = (y1 - 1) / 8;
    
    for(y = page_start; y <= page_end; y++)
    {
        for(x = x0; x < x1; x++)
        {
            OLED_GRAM[x][y] = BMP[j++];
        }
    }
    OLED_Refresh();
} 

// 画点函数
// x:0~127
// y:0~63
// t:1 填充 0,清空
void OLED_DrawPoint(u8 x, u8 y, u8 t){
    u8 pos, bx= 0;
    if (x > 127 || y > 63)
        return; // 超出范围直接返回
    
    pos = y / 8; // 计算页地址
    bx = y % 8;  // 计算位偏移
    if(t)
        OLED_GRAM[x][pos] |= 0x01 << bx;
    else
        OLED_GRAM[x][pos] &= ~(0x01 << bx);
}
// 更新显示
void OLED_Refresh(void){
    u8 i, n;
    for (i = 0; i < 8; i++){
        OLED_WR_Byte(0xb0 + i, OLED_CMD); // 设置页地址（0~7）
        OLED_WR_Byte(0x02, OLED_CMD);     // 设置显示位置—列低地址
        OLED_WR_Byte(0x10, OLED_CMD);     // 设置显示位置—列高地址
        for (n = 0; n < 128; n++)// 统一写入数据
            OLED_WR_Byte(OLED_GRAM[n][i], OLED_DATA);
    } // 更新显示
}
// 按周期数算延时, 改主频不用动 (CPUCLK_FREQ 5MHz: 1ms=5000周期, 1us=5周期)
void delay_ms(unsigned int ms)
{
	if (ms == 0) return;   // delay_cycles(0) 会跑 2^31 次循环, 等于死机
	delay_cycles((uint32_t)ms * (CPUCLK_FREQ / 1000u));
}

void delay_us(unsigned int us)
{
	if (us == 0) return;   // delay_cycles(0) 会跑 2^31 次循环, 等于死机
	delay_cycles((uint32_t)us * (CPUCLK_FREQ / 1000000u));
}
//OLED划线函数
void OLED_DrawLine(uint8_t x_source, uint8_t y_source, uint8_t x_destination, uint8_t y_destination, u8 draw_mode) {
    // 计算x轴和y轴方向的绝对长度
    uint8_t x_length = abs((int)(x_destination - x_source)); 
    uint8_t y_length = abs(y_destination - y_source); 
    
    // 算法所需变量声明
    uint8_t dm, i;      // dm = 最大长度（x或y方向），i = 循环计数器
    float dx, dy;       // 每步x和y的增量值
    float x, y;         // 当前浮点坐标
    
    // 判断线是更接近水平还是垂直
    // 这将决定我们是在x方向还是y方向取更多步长
    if (x_length >= y_length) {
        dm = x_length;  // 更接近水平线
    }
    else {
        dm = y_length;  // 更接近垂直线
    }
    
    // 计算每步x和y的增量
    // 这些分数值确保我们能在'dm'步内精确到达终点
    dx = (float)(x_destination - x_source) / dm;
    dy = (float)(y_destination - y_source) / dm;
    
    // 初始化起点坐标，添加0.5偏移量用于四舍五入
    // +0.5可以确保浮点转整型时的像素位置准确
    x = (float)x_source + 0.5;
    y = (float)y_source + 0.5;
    
    // 逐点绘制直线
    for (i = 0; i <= dm; i++) {
        if (draw_mode) {
            OLED_DrawPoint(x, y,1);   // 在当前坐标画点
        }
        else {
            OLED_DrawPoint(x, y,0);  // 在当前坐标擦除点
        }
        // 沿着直线移动到下一个位置
        x += dx;
        y += dy;
    }
}

void OLED_DrawWave(uint8_t step,float* waveData){
    uint8_t PointX,PiontY;
    uint8_t Xtime;
    uint8_t range  = 63;
    float scale = (float)range/64;
    uint8_t PiontXlast,PiontYlast;

    PointX = 0;
    PiontY = waveData[0] *scale;

    for(Xtime =  0;Xtime<128;Xtime+=step){
        PiontXlast = PointX;
        PiontYlast = PiontY;

        PointX = Xtime;

        PiontY = waveData[Xtime] *scale;
        OLED_DrawLine(PiontXlast,PiontYlast,PointX,PiontY,1);
        OLED_Refresh();
        delay_ms(20);
    }
}

// 初始化SSD1306
//OLED的初始化
void OLED_Init(void)
{
    OLED_RES_Clr();
    delay_cycles(200000);
    OLED_RES_Set();

    OLED_WR_Byte(0xAE,OLED_CMD);//--turn off oled panel
    OLED_WR_Byte(0x00,OLED_CMD);//---set low column address
    OLED_WR_Byte(0x10,OLED_CMD);//---set high column address
    OLED_WR_Byte(0x40,OLED_CMD);//--set start line address  Set Mapping RAM Display Start Line (0x00~0x3F)
    OLED_WR_Byte(0x81,OLED_CMD);//--set contrast control register
    OLED_WR_Byte(0x01,OLED_CMD);// Set SEG Output Current Brightness
    OLED_WR_Byte(0xA1,OLED_CMD);//--Set SEG/Column Mapping     0xa0左右反置 0xa1正常
    OLED_WR_Byte(0xC8,OLED_CMD);//Set COM/Row Scan Direction   0xc0上下反置 0xc8正常
    OLED_WR_Byte(0xA6,OLED_CMD);//--set normal display
    OLED_WR_Byte(0xA8,OLED_CMD);//--set multiplex ratio(1 to 64)
    OLED_WR_Byte(0x3f,OLED_CMD);//--1/64 duty
    OLED_WR_Byte(0xD3,OLED_CMD);//-set display offset        Shift Mapping RAM Counter (0x00~0x3F)
    OLED_WR_Byte(0x00,OLED_CMD);//-not offset
    OLED_WR_Byte(0xd5,OLED_CMD);//--set display clock divide ratio/oscillator frequency
    OLED_WR_Byte(0x80,OLED_CMD);//--set divide ratio, Set Clock as 100 Frames/Sec
    OLED_WR_Byte(0xD9,OLED_CMD);//--set pre-charge period
    OLED_WR_Byte(0xF1,OLED_CMD);//Set Pre-Charge as 15 Clocks & Discharge as 1 Clock
    OLED_WR_Byte(0xDA,OLED_CMD);//--set com pins hardware configuration
    OLED_WR_Byte(0x12,OLED_CMD);
    OLED_WR_Byte(0xDB,OLED_CMD);//--set vcomh
    OLED_WR_Byte(0x40,OLED_CMD);//Set VCOM Deselect Level
    OLED_WR_Byte(0x20,OLED_CMD);//-Set Page Addressing Mode (0x00/0x01/0x02)
    OLED_WR_Byte(0x02,OLED_CMD);//
    OLED_WR_Byte(0x8D,OLED_CMD);//--set Charge Pump enable/disable
    OLED_WR_Byte(0x14,OLED_CMD);//--set(0x10) disable
    OLED_WR_Byte(0xA4,OLED_CMD);// Disable Entire Display On (0xa4/0xa5)
    OLED_WR_Byte(0xA6,OLED_CMD);// Disable Inverse Display On (0xa6/a7)
    OLED_Clear();
    OLED_WR_Byte(0xAF,OLED_CMD);
}
