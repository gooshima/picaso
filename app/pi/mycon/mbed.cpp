#include "mbed.h"
#include "Timer.h"
#include "neopixel.h"
//#include "SDFileSystem.h"
#include "I2Cdev.h"
#include "MPU6050_6Axis_MotionApps20.h"
#include "math.h"
#include "iSDIO.h"

#define NUMBER_OF_LEDS 24

// static instances
Serial pc(USBTX, USBRX);
Timer timer;

//          2     7    5    1   GND=3&6 VCC=4
SD_iSDIO sd(p11, p12, p13, p14, "sd"); // MOSI, MISO, SCLK, SSEL

// MPU control/status vars
bool dmpReady = false;  // set true if DMP init was successful
uint8_t mpuIntStatus;   // holds actual interrupt status byte from MPU
uint8_t devStatus;      // return status after each device operation (0 = success, !0 = error)
uint16_t packetSize;    // expected DMP packet size (default is 42 bytes)
uint16_t fifoCount;     // count of all bytes currently in FIFO
uint8_t fifoBuffer[64]; // FIFO storage buffer



char str[5];
uint8_t buffer[512];



// orientation/motion vars
Quaternion q;           // [w, x, y, z]         quaternion container
VectorInt16 aa;         // [x, y, z]            accel sensor measurements
VectorInt16 aaReal;     // [x, y, z]            gravity-free accel sensor measurements
VectorInt16 aaWorld;    // [x, y, z]            world-frame accel sensor measurements
VectorFloat gravity;    // [x, y, z]            gravity vector
float euler[3];         // [psi, theta, phi]    Euler angle container
float ypr[3];           // [yaw, pitch, roll]   yaw/pitch/roll container and gravity vector

InterruptIn checkpin(p29);

MPU6050 mpu;
volatile bool mpuInterrupt = false;     // indicates whether MPU interrupt pin has gone high
void dmpDataReady()
{
    mpuInterrupt = true;
}

typedef struct COLOR_PALETTE {
    unsigned char red;
    unsigned char green;
    unsigned char blue;
} ColorPalette_t;

typedef struct LED_PATTERN {
    signed long dulation_ms;
    unsigned char amplitude;
    unsigned char palette_no[NUMBER_OF_LEDS];
} LedPattern_t;

const ColorPalette_t colorPalette[256] = {
    {0,0,0},
    {0,0,255},
    {0,255,0},
    {0,255,255},
    {255,0,0},
    {255,0,255},
    {255,255,0},
    {255,255,255}
};

LedPattern_t led_pattern_0[] = {
    { 150,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 },
    { -1,   0 ,   0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 }
};

LedPattern_t led_pattern_1[] = {
    {150, 255,    4,  1,  1,  1,  4,  1,  1,  1,  4,  1,  1,  1,  4,  1,  1,  1,  4,  1,  1,  1,  4,  1,  1,  1,},
    {150,  64,    1,  4,  1,  1,  1,  4,  1,  1,  1,  4,  1,  1,  1,  4,  1,  1,  1,  4,  1,  1,  1,  4,  1,  1,},
    {150, 128,    1,  1,  4,  1,  1,  1,  4,  1,  1,  1,  4,  1,  1,  1,  4,  1,  1,  1,  4,  1,  1,  1,  4,  1,},
    {150, 128,    1,  1,  1,  4,  1,  1,  1,  4,  1,  1,  1,  4,  1,  1,  1,  4,  1,  1,  1,  4,  1,  1,  1,  4,},
    { -1,   0 ,   0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 }
};

LedPattern_t led_pattern_2[] = {
    {150,255,1,6,6,6,1,6,6,6,1,6,6,6,1,6,6,6,1,6,6,6,1,6,6,6},
    {150,255,6,1,6,6,6,1,6,6,6,1,6,6,6,1,6,6,6,1,6,6,6,1,6,6},
    {150,255,6,6,1,6,6,6,1,6,6,6,1,6,6,6,1,6,6,6,1,6,6,6,1,6},
    {150,255,6,6,6,1,6,6,6,1,6,6,6,1,6,6,6,1,6,6,6,1,6,6,6,1},
    { -1,   0 ,   0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 }
};

LedPattern_t  * PatternTable[] = {
    led_pattern_0,
    led_pattern_1,
    led_pattern_2,
};

const int led_remap_table[NUMBER_OF_LEDS] = {
//    0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23
    0,1,2,3,7,6,5,4,8,9,10,11,15,14,13,12,16,17,18,19,23,22,21,20
};

unsigned char CurrentStepNo = 0;
unsigned long next_time = 0;

bool getMotion()
{
    bool ret = false;
#if 0
    // read raw accel/gyro measurements from device
    accelgyro.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

    // these methods (and a few others) are also available
    //accelgyro.getAcceleration(&ax, &ay, &az);
    //accelgyro.getRotation(&gx, &gy, &gz);

//neon_acc
//    if(ax<=freefall && ay<=freefall && az<=freefall){
//      if(freefall_flag == 0){
//        freefall_flag = 1;
//        ledstate = !ledstate;
//        digitalWrite(LED_RED, ledstate);
//      }
//    }else if(freefall_flag == 1){
//      freefall_flag = 0;
//      ledstate = !ledstate;
//      digitalWrite(LED_RED, ledstate);
//    }

//neon_gyro
    switch(movestate) {
        case 0:
            if(ax > thres) {
                movestate = 1;
                ret = true;
                while(ax > thres) accelgyro.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
            } else if (ax < -thres) {
                movestate = 2;
                while(ax < -thres) accelgyro.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
            }
            break;

        case 1:
            if(ax < -thres) {
                movestate = 0;
                while(ax < -thres) accelgyro.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
            }
            break;

        case 2:
            if(ax > thres) {
                movestate =0;
                while(ax > thres) accelgyro.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
            }
            break;

        default:
            break;
    }

//    Serial.println(movestate);
    // display tab-separated accel/gyro x/y/z values
//    Serial.print(ax); Serial.print(" ");
//    Serial.print(ay); Serial.print(" ");
//    Serial.print(az); Serial.print(" ");
//    Serial.print(gx); Serial.print(" ");
//    Serial.print(gy); Serial.print(" ");
//    Serial.println(gz);

    // blink LED to indicate activity
#endif
    return ret;
}

void notify_FlashAir()
{
    //static bool flag = false;

    //digitalWrite(13,flag ? HIGH : LOW);
    //flag = !flag;
}




int16_t receive_FlashAirSharedMem(uint16_t address)
{
    uint16_t val = 0;
    uint8_t mem;
    
    if (!sd.readExtMemory(1, 1, 0x1000, 1, &mem)) {
        return -1;
    }
    return mem;
}

void update_led(unsigned char * led,unsigned char amplitude,neopixel::PixelArray& ledArray)
{
    int i;
    char r,g,b;
    int palette_no;
    struct neopixel::Pixel pixel[NUMBER_OF_LEDS];

    for(i=0; i<NUMBER_OF_LEDS; i++) {
        palette_no = led[led_remap_table[i]];
        r = colorPalette[palette_no].red * amplitude / 256;
        g = colorPalette[palette_no].green * amplitude / 256;
        b = colorPalette[palette_no].blue * amplitude / 256;
        pixel[i].red = r;
        pixel[i].green = g;
        pixel[i].blue = b;
    }
    ledArray.update(pixel,NUMBER_OF_LEDS);
}

void sequencer(unsigned char pattern_no,neopixel::PixelArray& ledArray)
{
    static unsigned char last_pattern_no = 0;
    unsigned long current_time = timer.read_ms();
    LedPattern_t  * led_array = PatternTable[pattern_no];

    // If the pattern number is updated,then the sequencer must reset
    if(pattern_no != last_pattern_no) {
        CurrentStepNo = 0;
        current_time = 0;
        last_pattern_no = 0;
    }

    update_led(
        (led_array)[CurrentStepNo].palette_no,
        (unsigned char)(led_array)[CurrentStepNo].amplitude,
        ledArray);

    if(next_time <= current_time) {
        //pc.puts("Update led\r\n");
        CurrentStepNo++;
        LedPattern_t led_pattern = (led_array)[CurrentStepNo];
        signed long s = led_pattern.dulation_ms;
        if(s == -1) {
            CurrentStepNo = 0;
        }
        LedPattern_t led_pattern_next = (led_array)[CurrentStepNo];
        next_time = current_time + led_pattern_next.dulation_ms;
    }
    last_pattern_no = pattern_no;
}

int led_process(double val)
{
    static double val_max = 0;
    static double val_min = 0;
    double step;

    pc.printf("\t%f\t",val);

    if(val_max < val) {
        val_max = val;
    }
    if(val_min > val) {
        val_min = val;
    }

    step = (val_max - val_min) / 5;
    val -= val_min;

    pc.printf("%d",(int)(val / step));

    return (int)(val / step);

}

void applyLed(int num,neopixel::PixelArray& ledArray)
{
    struct neopixel::Pixel pixel[7];
    static uint8_t r[7];
    static uint8_t g[7];
    static uint8_t b[7];
    const int decrease_step_r = 10;
    const int decrease_step_g = 6;
    const int decrease_step_b = 3;
    int i;

    num = 4 - num;

    r[0] = 0xff;
    g[0] = 0x50;
    b[0] = 0x50;
    r[num+1] = 0xff;
    g[num+1] = 0x50;
    b[num+1] = 0x50;

    num++;

    for(i=0; i<7; i++) {
        pixel[i].red = r[i];
        pixel[i].green = g[i];
        pixel[i].blue = b[i];

        if(r[i] > decrease_step_r) {
            r[i]-=decrease_step_r;
        }

        if(g[i] > decrease_step_g) {
            g[i]-=decrease_step_g;
        }
        if(b[i] > decrease_step_b) {
            b[i]-=decrease_step_b;
        }
    }
    ledArray.update(pixel,7);
}

void printIPAddress(uint8_t* p)
{
    pc.printf("IP Address = ");
    pc.printf("%d", p[0]);
    pc.printf(".");
    pc.printf("%d", p[1]);
    pc.printf(".");
    pc.printf("%d", p[2]);
    pc.printf(".");
    pc.printf("%d\r\n", p[3]);
}
void printByte(uint8_t value)
{
    pc.printf("%x", value >> 4);
    pc.printf("%x", value & 0xF);
}
void printBytes(uint8_t* p, uint32_t len)
{
    for (int i = 0; i < len; ++i) {
        printByte(p[i]);
    }
}

uint8_t iSDIO_status()
{
    pc.printf("\nRead iSDIO Status Register");
    // Read iSDIO Status Register (E7 1.10 2.2.2.1)
    memset(buffer, 0, 0x200);
    if (!sd.readExtMemory(1, 1, 0x400, 0x200, buffer)) {
        return false;
    }
#if 0
    for (int i = 0; i < 0x200; i++) {
        pc.printf("%2x ", buffer[i]);
        if ((i & 0xf) == 0xf) pc.printf("\n");
    }
#endif
    // Show values in the common status area.
    pc.printf("\n == iSDIO Status Registers == ");
    pc.printf("\n [0400h] Command Write Status: ");
    if (buffer[0x000] & 0x01) pc.printf("CWU ");
    if (buffer[0x000] & 0x02) pc.printf("CWA ");
    pc.printf("\n [0420h] iSDIO Status: ");
    if (buffer[0x020] & 0x01) pc.printf("CRU ");
    if (buffer[0x020] & 0x02) pc.printf("ESU ");
    if (buffer[0x020] & 0x04) pc.printf("MCU ");
    if (buffer[0x020] & 0x08) pc.printf("ASU ");
    pc.printf("\n [0422h] iSDIO Int Enable: ");
    if (buffer[0x022] & 0x01) pc.printf("CRU_ENA ");
    if (buffer[0x022] & 0x02) pc.printf("ESU_ENA ");
    if (buffer[0x022] & 0x04) pc.printf("MCU_ENA ");
    if (buffer[0x022] & 0x08) pc.printf("ASU_ENA ");
    pc.printf("\n [0424h] Error Status: ");
    if (buffer[0x024] & 0x01) pc.printf("CRE ");
    if (buffer[0x024] & 0x02) pc.printf("CWE ");
    if (buffer[0x024] & 0x04) pc.printf("RRE ");
    if (buffer[0x024] & 0x08) pc.printf("APE ");
    pc.printf("\n [0426h] Memory Status: ");
    if (buffer[0x026] & 0x01) pc.printf("MEX ");
    if (buffer[0x026] & 0x02) pc.printf("FAT ");
    for (int i = 0; i < 8; ++i) {
        uint8_t addr = 0x40 + i * 0x14;
        pc.printf("\n [04");
        printByte(addr);
        pc.printf("h] Command Response Status #");
        pc.printf("%d", i + 1);
        pc.printf(": ");
        if (buffer[addr] & 0x01) {
            pc.printf("id = ");
            pc.printf("%d", get_u16(buffer + addr + 2));
            pc.printf(", sequence id = ");
            pc.printf("%d", get_u32(buffer + addr + 4));
            pc.printf(", status = ");
            switch (buffer[addr + 8]) {
                case 0x00:
                    pc.printf("Initial");
                    break;
                case 0x01:
                    pc.printf("Command Processing");
                    break;
                case 0x02:
                    pc.printf("Command Rejected");
                    break;
                case 0x03:
                    pc.printf("Process Succeeded");
                    break;
                case 0x04:
                    pc.printf("Process Terminated");
                    break;
                default:
                    pc.printf("Process Failed ");
                    pc.printf("%h", buffer[addr + 8]);
                    break;
            }
        } else {
            pc.printf("Not registered");
        }
    }
    // Show values in the application status area.
    pc.printf("\n == Wireless LAN Status Registers ==");
    pc.printf("\n [0500h] DLNA Status: ");
    if (buffer[0x100] & 0x01) pc.printf("ULR ");
    if (buffer[0x100] & 0x02) pc.printf("DLU ");
    if (buffer[0x100] & 0x04) pc.printf("CBR ");
    if (buffer[0x100] & 0x08) pc.printf("CDR ");
    pc.printf("\n [0501h] P2P Status: ");
    if (buffer[0x101] & 0x01) pc.printf("ILU ");
    if (buffer[0x101] & 0x02) pc.printf("FLU ");
    pc.printf("\n [0502h] PTP Status: ");
    if (buffer[0x102] & 0x01) pc.printf("RPO ");
    if (buffer[0x102] & 0x02) pc.printf("RPD ");
    if (buffer[0x102] & 0x04) pc.printf("RPC ");
    if (buffer[0x102] & 0x08) pc.printf("CPI ");
    if (buffer[0x102] & 0x10) pc.printf("DPI ");
    if (buffer[0x102] & 0x20) pc.printf("CIL ");
    pc.printf("\n [0504h] Application: ");
    pc.printf((char *)buffer[0x104]);
    pc.printf("\n [0506h] WLAN: ");
    if ((buffer[0x106] & 0x01) == 0x00) pc.printf("No Scan, ");
    if ((buffer[0x106] & 0x01) == 0x01) pc.printf("Scanning, ");
    if ((buffer[0x106] & 0x06) == 0x00) pc.printf("No WPS, ");
    if ((buffer[0x106] & 0x06) == 0x02) pc.printf("WPS with PIN, ");
    if ((buffer[0x106] & 0x06) == 0x04) pc.printf("WPS with PBC, ");
    if ((buffer[0x106] & 0x08) == 0x00) pc.printf("Group Client, ");
    if ((buffer[0x106] & 0x08) == 0x08) pc.printf("Group Owner ");
    if ((buffer[0x106] & 0x10) == 0x00) pc.printf("STA, ");
    if ((buffer[0x106] & 0x10) == 0x10) pc.printf("AP, ");
    if ((buffer[0x106] & 0x60) == 0x00) pc.printf("Initial, ");
    if ((buffer[0x106] & 0x60) == 0x20) pc.printf("Infrastructure, ");
    if ((buffer[0x106] & 0x60) == 0x40) pc.printf("Wi-Fi Direct, ");
    if ((buffer[0x106] & 0x80) == 0x00) pc.printf("No Connection, ");
    if ((buffer[0x106] & 0x80) == 0x80) pc.printf("Connected, ");
    pc.printf("\n [0508h] SSID: ");
    for (int i = 0; i < 32 && buffer[0x108 + i] != 0; ++i) {
        pc.printf("%c", (char)buffer[0x108 + i]);
    }
    pc.printf("\n [0528h] Encryption Mode: ");
    switch (buffer[0x128]) {
        case 0 :
            pc.printf("Open System and no encryption");
            break;
        case 1 :
            pc.printf("Open System and WEP");
            break;
        case 2 :
            pc.printf("Shared Key and WEP");
            break;
        case 3 :
            pc.printf("WPA-PSK and TKIP");
            break;
        case 4 :
            pc.printf("WPA-PSK and AES");
            break;
        case 5 :
            pc.printf("WPA2-PSK and TKIP");
            break;
        case 6 :
            pc.printf("WPA2-PSK and AES");
            break;
        default:
            pc.printf("Unknown");
    }
    pc.printf("\n [0529h] Signal Strength: ");
    pc.printf("%d", buffer[0x129]);
    pc.printf("\n [052Ah] Channel: ");
    if (buffer[0x12A] == 0) pc.printf("No connection");
    else pc.printf("%d", buffer[0x12A]);
    pc.printf("\n [0530h] MAC Address: ");
    printBytes(buffer + 0x130, 6);
    pc.printf("\n [0540h] ID: ");
    for (int i = 0; i < 16 && buffer[0x140 + i] != 0; ++i) {
        pc.printf("%c", (char)buffer[0x140 + i]);
    }
    pc.printf("\n [0550h] IP Address: ");
    printIPAddress(buffer + 0x150);
    pc.printf("\n [0554h] Subnet Mask: ");
    printIPAddress(buffer + 0x154);
    pc.printf("\n [0558h] Default Gateway: ");
    printIPAddress(buffer + 0x158);
    pc.printf("\n [055Ch] Preferred DNS Server: ");
    printIPAddress(buffer + 0x15C);
    pc.printf("\n [0560h] Alternate DNS Server: ");
    printIPAddress(buffer + 0x160);
    pc.printf("\n [0564h] Proxy Server: ");
    if ((buffer[0x164] & 0x01) == 0x00) pc.printf("Disabled");
    if ((buffer[0x164] & 0x01) == 0x01) pc.printf("Enabled");
    pc.printf("\n [0570h] Date: ");
    pc.printf("%d", buffer[0x171] + 1980);
    pc.printf("-");
    pc.printf("%d", buffer[0x170] >> 4);
    pc.printf("-");
    pc.printf("%d", buffer[0x170] & 0xF);
    pc.printf("\n [0572h] Time: ");
    pc.printf("%d", buffer[0x173] >> 3);
    pc.printf(":");
    pc.printf("%d", buffer[0x172] << 3 | buffer[0x170] >> 3);
    pc.printf(":");
    pc.printf("%d", (buffer[0x172] & 0x1F) * 2);
    pc.printf("\n [0574h] HTTP Status: ");
    pc.printf("%d", buffer[0x174] & 0xEF);
    if ((buffer[0x174] & 0x80) == 0x00) pc.printf(" (No Processing)");
    if ((buffer[0x174] & 0x80) == 0x80) pc.printf(" (Processing)");
    pc.printf("\n [0575h] Power Save Management: ");
    if ((buffer[0x175] & 0x01) == 0x00) pc.printf("Power Save Mode Off");
    if ((buffer[0x175] & 0x01) == 0x01) pc.printf("Power Save Mode On");
    pc.printf("\n [0576h] File System Management: ");
    if ((buffer[0x176] & 0x01) == 0x00) pc.printf("FS Information may be modified");
    if ((buffer[0x176] & 0x01) == 0x01) pc.printf("FS Information shall not be modified");
    pc.printf("\n");

    return true;
}

// FlashAirがWiFi APに接続するまで待つ
void waitForFlashAirToConnectAP(void)
{
    while(1) {

        memset(buffer, 0, 0x10);
        if (sd.readExtMemory(1, 1, 0x400 + 0x150, 0x4, buffer)) {
            if(buffer[0] != 0) {
                printf("Connected to AP,");
                printIPAddress(buffer);
                break;
            } else {
                printf("waiting...\n");
            }
        }
        wait_ms(5000);
    }
}

// クライアントがFlashAirに接続するまで待つ
int waitForWifiConnection(void)
{
    uint8_t buffer[64];
    // Connection wait
    while(1) {

        memset(buffer, 0, 0x14);
        if (!sd.readExtMemory(1, 1, 0x506, 0x14, buffer)) {
            return -1;
        }
        uint8_t resp = get_u8(buffer);
        if ((resp & 0x80) != 0) {
            break;
        }
        pc.printf(".\n");
        wait_ms(5000);
    }

    // Dump status
    if (iSDIO_status() == false) {
        pc.printf("Failed to read status.\n");
        return -1;
    }
    return 1;
}




struct neopixel::Pixel pixel[3];
void hey(neopixel::PixelArray& ledArray, char str[5]){

        //struct neopixel::Pixel pixel[3];


/*
        pixel[0].red = 255;
        pixel[0].green = 0;
        pixel[0].blue = 0;

        pixel[1].red = 0;
        pixel[1].green = 255;
        pixel[1].blue = 0;

        pixel[2].red = 0;
        pixel[2].green = 0;
        pixel[2].blue = 255;

        ledArray.update(pixel, 3);
        */

/*
              Serial.println("led array is");
              Serial.println(str[0]);
              Serial.println(str[1]);
              Serial.println(str[2]);
              */

              switch(str[0]){
                case '1'://白
                  //strip.setPixelColor(0, strip.Color(255,255,255));
                          pixel[0].red = 255;
                          pixel[0].green = 255;
                          pixel[0].blue = 255;
                  break;
                case '2'://赤
                  //strip.setPixelColor(0, strip.Color(255,0, 0));
                          pixel[0].red = 255;
                          pixel[0].green = 0;
                          pixel[0].blue = 0;
                  break;
                case '3'://緑
                  //strip.setPixelColor(0, strip.Color(0,255,0));
                          pixel[0].red = 0;
                          pixel[0].green = 255;
                          pixel[0].blue = 0;
                  break;
                case '4'://青
                  //strip.setPixelColor(0, strip.Color(0,0,255));
                          pixel[0].red = 0;
                          pixel[0].green = 0;
                          pixel[0].blue = 255;
                  break;
                case '5'://無
                  //strip.setPixelColor(0, strip.Color(0,0,0));
                          pixel[0].red = 0;
                          pixel[0].green = 0;
                          pixel[0].blue = 0;
                  break;
                default:
                  //strip.setPixelColor(0, strip.Color(0,0,0));
                          pixel[0].red = 255;
                          pixel[0].green = 255;
                          pixel[0].blue = 255;
                  break;
              }
              //strip.show();

              switch(str[1]){
                case '1'://白
                  //strip.setPixelColor(1, strip.Color(255,255,255));
                  pixel[1].red = 255;
                          pixel[1].green = 255;
                          pixel[1].blue = 255;
                  break;
                case '2'://赤
                  //strip.setPixelColor(1, strip.Color(255,0, 0));
                  pixel[1].red = 255;
                          pixel[1].green = 0;
                          pixel[1].blue = 0;
                  break;
                case '3'://緑
                  //strip.setPixelColor(1, strip.Color(0,255,0));
                  pixel[1].red = 0;
                          pixel[1].green = 255;
                          pixel[1].blue = 0;
                  break;
                case '4'://青
                  //strip.setPixelColor(1, strip.Color(0,0,255));
                  pixel[1].red = 0;
                          pixel[1].green = 255;
                          pixel[1].blue = 255;
                  break;
                case '5'://無
                  //strip.setPixelColor(1, strip.Color(0,0,0));
                  pixel[1].red = 0;
                          pixel[1].green = 0;
                          pixel[1].blue = 0;
                  break;
                default:
                  //strip.setPixelColor(1, strip.Color(0,0,0));
                  pixel[1].red = 255;
                          pixel[1].green = 255;
                          pixel[1].blue = 255;
                  break;
              }
              //strip.show();

              switch(str[2]){
                case '1'://白
                  //strip.setPixelColor(2, strip.Color(255,255,255));
                  pixel[2].red = 255;
                          pixel[2].green = 255;
                          pixel[2].blue = 255;
                  break;
                case '2'://赤
                  //strip.setPixelColor(2, strip.Color(255,0, 0));
                  pixel[2].red = 255;
                                            pixel[2].green = 0;
                                            pixel[2].blue = 0;
                  break;
                case '3'://緑
                  //strip.setPixelColor(2, strip.Color(0,255,0));
                  pixel[2].red = 0;
                                            pixel[2].green = 255;
                                            pixel[2].blue = 0;
                  break;
                case '4'://青
                  //strip.setPixelColor(2, strip.Color(0,0,255));
                  pixel[2].red = 0;
                                            pixel[2].green = 0;
                                            pixel[2].blue = 255;
                  break;
                case '5'://無
                  //strip.setPixelColor(2, strip.Color(0,0,0));
                  pixel[2].red = 0;
                                            pixel[2].green = 0;
                                            pixel[2].blue = 0;
                  break;
                default:
                  //strip.setPixelColor(2, strip.Color(0,0,0));
                  pixel[2].red = 255;
                                            pixel[2].green = 255;
                                            pixel[2].blue = 255;
                  break;
              }
              //strip.show();
              ledArray.update(pixel, 3);


}

void receive_FlashAir(neopixel::PixelArray& ledArray, char str[5])
{
    int val;

    //memset(buffer, 0, 10);
    //memset(buffer, 0, 0x200);
    if (!sd.readExtMemory(1, 1, 0x1000, 0x200, buffer)) {
        //return -1;
    } else {
    /*
        buffer[4] = 0;
        val = atoi((const char*)buffer);
        */
            //pc.printf("%d",(int)(val / step));
            pc.printf("yeah============");
            str[0] = buffer[0];
            pc.printf("%d",str[0]);
            str[1] = buffer[1];
            pc.printf("%d",str[1]);
            str[2] = buffer[2];
            pc.printf("%d",str[2]);
            str[3] = buffer[3];
            pc.printf("%d",str[3]);
            str[4] = 0;
            pc.printf("yeah============");
            val = atoi(str);
    }


    hey(ledArray, str);

    //return val;
    //return val;
}








int main()
{
    uint8_t devStatus;
    memset(buffer, 0, 0x200);

    char pattern_no[5];
    bool motion_result;

    timer.start();

    pc.baud(115200);

    DigitalIn(p5, PullDown);
    neopixel::PixelArray ledArray(p5);

    // Activate FlashAir
    {
        FILE *fp = fopen("/sd/mbed.txt", "w");
        fclose(fp);
    }

    waitForFlashAirToConnectAP();
    pc.printf("WiFi Connected\n");

    while(1) {
        //pattern_no = receive_FlashAir();
        receive_FlashAir(ledArray, pattern_no);
        //pc.printf("YEAH = Pattern No = %d\n",pattern_no);
        //hey(ledArray, pattern_no);
        wait_ms(450);
    }
}




