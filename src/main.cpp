/*
                                                        update this
 Name:    GearIndicator_unoV3.ino
 Created: 11/21/2020 9:54:54 PM
 Author:  alexj
*/
#include <Arduino.h>
#include <Adafruit_ADS1X15.h>
#include <Wire.h>
Adafruit_ADS1115 ads;
//---------------------------------------------------------------------------------
//function defitions 
void gearRecieved();
void setupnano();
void Touch_setup();
void writeTxtDisplaySingle(String displayMessage, int x, int y);
void removeTxtDisplaySingle(String displayMessage, int x, int y);
void writeTxtDisplayString(String displayMessage, int x, int y);
void removeTxtDisplayString(String displayMessage, int x, int y);

void timerFunction();
void onTimeActions();
void loopNano();
void receiveEventnano();
void sendToUnoEventnano();
void GearSelectnano();
void writeTxtDisplaySinglenano(char displayMessage);
void removeTxtDisplaySinglenano(char displayMessage);
void writeTxtDisplayStringnano(char displayMessage[]);
void removeTxtDisplayStringnano(char displayMessage[]);

void nuturalTimerFunctionnano();
void readSwitchesnano();
void debugReadSwitchesnano();
void debugSerialInputSwitchesnano();
int getLastGearnano();

void readVoltage();
void setupVoltageReader();
//---------------------------------------------------------------------------------




// Wire Master Reader
//uno
//i need to fix the tft display parts.
//it is playing up because of the tft screen is using the i2c communication protacol.
// i need to use the "SPI" protocol, and /or run the i2c scanner on the uno for the tft screen address

#define writeGearStringSingle true
#define SerialDebugging false //---------------------------------------------------------------------------------change for not Serial Debugging.
#define voltageDebug false    //---------------------------------------------------------------------------------
#define Mainserialmessages true

#define DigitalGearPins true
#define SerialDebuggingReadVoltage true

#define i2cMode false
#define GearDebuggingAutomatic false
#define GearDebuggingManual false
#define GearNormalMode true


#define TFTmode true
#define tftModeDebug false
#define TouchScreenMode false

#define SDcard false
#define RCAmode false


#if (RCAmode == true)
  #define Mainserialmessages false
  #define UARTserial true
#endif // (RCAmode == false)

#if (TFTmode == true)
  #define UARTserial false
    #if ( TouchScreenMode == true)
        #include <TouchScreen.h>
  #endif
#endif

String SerialInput = "-1";

#define ANSWERSIZE 5 //might need to set as 5
char replyFromSlave[ANSWERSIZE]; //Initialized variable to store recieved data from slave
//#define SLAVE_ADDRESS  8
//#define MASTER_ADDRESS 10
//const int sizeOfMessageSend = 5;// change this to the size of what i am sending to master
//const int sizeOfMessageRecieve = 20;// for future use when uno is sending data to slave. ------------

//char replyFromMaster[ANSWERSIZE]; //Initialized variable to store recieved data


//char sending_message_what_gear[ANSWERSIZE]; //data contains gear and if it is writing/removing
char sendToUno[ANSWERSIZE];
char writeOrRemove[ANSWERSIZE]; //0 remove 1 write
int WOR = -1;


#if (DigitalGearPins == false)
    int switchUP = A0; // should be able to use digital pins for this 
    int switchDOWN = A1;
    int switchLEFT = A2;
    int switchRIGHT = A3;
#endif
#if (DigitalGearPins == true)
    int switchUP = 2; // should be able to use digital pins for this 
    int switchDOWN = 3;
    int switchLEFT = 4;
    int switchRIGHT = 5;
#endif



bool stateofsensorUp = false;
bool stateofsensorDown = false;
bool stateofsensorLeft = false;
bool stateofsensorRight = false;


unsigned long previousTime = 0;
byte seconds;
byte minutes;
byte hours;

char tvoutputTestMSG[] = "testing message";


bool nuturalGear = false;
char gear0[] = "N";       int Nutural = 0;
char gear1[] = "1";       int First = 1;
char gear2[] = "2";       int Second = 2;
char gear3[] = "3";       int Thrid = 3;
char gear4[] = "4";       int Forth = 4;
char gear5[] = "5";       int Fifth = 5;
char gear6[] = "R";       int Sixth = 6;
char lastGear[] = "E";    int LastGear = 7;
char currentGear[] = "E"; int CurrentGear = 8;

char trueCurrentGear[] = "E";
char trueLastGear[] = "E";
int loopCounter = 0;
bool inGearUno = false;// there are several ingear references thats why!?!!?
bool inGearNano = false;// there are several ingear references thats why!?!!?

//char gearRange[8] = {gear0, gear1,gear2,gear3,gear4,gear5,gear6,lastGear};
char gearRange[9] = {
  gear0[0], //0
  gear1[0], //1 
  gear2[0], //2
  gear3[0], //3
  gear4[0], //4
  gear5[0], //5
  gear6[0], //6
  lastGear[0], //7
  currentGear[0],//8
};
int amountOfGears = strlen(gearRange);
int debugGearTimer = 0;

//Params for voltage display
int calib = 7; // Value of calibration of ADS1115 to reduce error
float voltage = 0; // used to store voltage value
//float Radjust = 0.043421905; // Voltage divider factor ( R2 / R1+R2 )
float Radjust = 0.06382978723404255319148936170213;
//R1 = 220k 220,000
//R2 = 15k 15,000
float vbat = 0; //final voltage after calcs- voltage of the battery
//Params for voltage display

//remove later
unsigned long startMillis;  //some global variables available anywhere in the program
unsigned long currentMillis;
const unsigned long period = 1000;  //the value is a number of milliseconds
byte timeToEnableNuturalGear = 2;


const byte ledPin = 13;    //using the built in LED

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

/* //wtf is this for ?
int sensorPinDown = 3;
int sensorPinUp = 2;
int sensorPinLeft = 4;
int sensorPinRight = 5;
*/


//#define ANSWERSIZE 5 // might need to set as 5
//#define SLAVE_ADDRESS 8
//#define MASTER_ADDRESS 10
//const int sizeOfMessageSend = 20;// for future use when uno is sending data to slave. -------------
//const int sizeOfMessageRecieve = 5;// need to change this.
//char message[sizeOfMessageSend];// for future use when uno is sending data to slave. -------------

//char replyFromSlave[ANSWERSIZE]; //Initialized variable to store recieved data from slave
//char tempReplyFromSlave[ANSWERSIZE];

//char response[ANSWERSIZE];// not sure what this is?


/*
char gear0[] = "N";       int Nutural = 0;
char gear1[] = "1";       int First = 1;
char gear2[] = "2";       int Second = 2;
char gear3[] = "3";       int Thrid = 3;
char gear4[] = "4";       int Forth = 4;
char gear5[] = "5";       int Fifth = 5;
char gear6[] = "R";       int Sixth = 6;
char lastGear[] = "E";    int LastGear = 7;
char currentGear[] = "E"; int CurrentGear = 8;*/

char currentRecievedGear[] = "E";

int nuturalTimer = 0;
int nuturalTimerMax = 20;
int gearTimer = 0;


//char gearRange[8] = {gear0, gear1,gear2,gear3,gear4,gear5,gear6,lastGear};
/*
char gearRange[9] = {
  gear0[0], //0
  gear1[0], //1 
  gear2[0], //2
  gear3[0], //3
  gear4[0], //4
  gear5[0], //5
  gear6[0], //6
  lastGear[0], //7
  currentGear[0],//8
};
int amountOfGears = strlen(gearRange);*/
//port 20
//---------------------------
//include files

#if (RCAmode == true)

#include <SPI.h>
#include <TVout.h>
#include <Wire.h>
#include <font8x8ext.h>
#include <font8x8.h>
#include <font6x8.h>
#include <font4x6.h>

TVout TV;
unsigned char x, y;

#endif

#if (TFTmode == true)

  #include <Wire.h>
  #include <SPI.h>            // f.k. for Arduino-1.5.2
  #if (SDcard == true)
    #include <SD.h>             // Use the official SD library on hardware pins
  #endif
  #include <Adafruit_GFX.h>   //Core graphics library
  //#include <MCUFRIEND_kbv.h> //Hardware-specific library
  #include <Adafruit_ST7735.h> // Hardware-specific library for ST7735
  //#include <Adafruit_ST77xx.h>
  //#include <Adafruit_ST7789.h>
  #if (touchScreenMode == true)
    #include <TouchScreen.h>
  #endif
  
#endif // (TFTmode == true)

//include files
//---------------------------------------------------------------------------------------------
//tft specs
#if (touchScreenMode == true)


int dispx, dispy, text_y_center, swapxy;
#define SWAP(x, y) { int t = x; x = y; y = t; }

#define PORTRAIT  0
#define LANDSCAPE 1

#define TOUCH_ORIENTATION PORTRAIT

const int minX = 0;
const int minY = 0;
//maybe add an if touch orientation is landscape swap x/y max values

//tft specs
//---------------------------------------------------------------------------------------------
//calibrate
#define calibrateDebug false

#if( calibrateDebug == true)

#define TS_MINX 906
#define TS_MAXX 116

#define TS_MINY 92
#define TS_MAXY 952

#define SWAP(x, y) { int t = x; x = y; y = t; }

#define TITLE "TouchScreen.h GFX Calibration"



uint32_t cx, cy, cz;
uint32_t rx[8], ry[8];
int32_t clx, crx, cty, cby;
float px, py;
int dispx, dispy, text_y_center, swapxy;
uint32_t calx, caly, cals;


#endif //


//calibrate
//---------------------------------------------------------------------------------------------
//for SD card pre setup

uint16_t read16(File& f) {
    uint16_t result;         // read little-endian
    f.read((uint8_t*)&result, sizeof(result));
    return result;
}

uint32_t read32(File& f) {
    uint32_t result;
    f.read((uint8_t*)&result, sizeof(result));
    return result;
}




#define BMPIMAGEOFFSET 54
#define BUFFPIXEL      20


#define SD_CS     10
#define NAMEMATCH ""        // "" matches any name
//#define NAMEMATCH "tiger"   // *tiger*.bmp
#define PALETTEDEPTH   0     // do not support Palette modes
//#define PALETTEDEPTH   8     // support 256-colour Palett

char bmpfile_01[] = "/bmpFiles/01.bmp";
char bmpfile_02[] = "/bmpFiles/02.bmp";
char bmpfile_03[] = "/bmpFiles/03.bmp";
char bmpfile_04[] = "/bmpFiles/04.bmp";
//char namebuf[32] = "/";   //BMP files in root directory
char namebuf[32] = "/bmpFiles/";  //BMP directory e.g. files in /bmpFiles/*.bmp

File root;
int pathlen;


#define AmountOfFiles 4 //amount of files to display
#define FILE_NAME_SIZE_MAX 20 //file name length


char file_name[AmountOfFiles][FILE_NAME_SIZE_MAX];

MCUFRIEND_kbv tft;
//for SD card pre setup
//---------------------------------------------------------------------------------------------
//for touch screen pre setup
int XP = 8, YP = A3, XM = A2, YM = 9;  //most common configuration

#define MINPRESSURE 10
#define MAXPRESSURE 1000


#if (TOUCH_ORIENTATION == LANDSCAPE)
TouchScreen ts(YM, XM, YP, XP, 300);   //re-initialised after diagnose

#define TS_MINX 914
#define TS_MAXX 111

#define TS_MAXY 79
#define TS_MINY 940
int maxDisplayX = 480;
int maxDisplayY = 320;
#endif//I NEED TO FIX THIS NUMBERS
#if (TOUCH_ORIENTATION == PORTRAIT)
TouchScreen ts(XP, YP, XM, YM, 300);   //re-initialised after diagnose
#define TS_MINX 906
#define TS_MAXX 116

#define TS_MINY 92
#define TS_MAXY 952
int maxDisplayX = 320;
int maxDisplayY = 480;

#endif//I NEED TO FIX THIS NUMBERS


TSPoint tp;
uint16_t ID;                          //global point
int lastX = 0;
int lastY = 0;
#endif

//---------------------------------------------------------------------------------------------
//for tft screen pre setup

#if (TFTmode == true)

  #define TFT_SCLK    13  //13   
  #define TFT_MOSI    12  //11
  #define TFT_CS      11  //10
  #define TFT_RST      8  //8
  #define TFT_DC       10  //9
  //Vcc 5v
  Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);
  
  /*
   same as
  SCK-- SCL
  MOSI -- SDA
  TFT_CS -- CS
  D/C -- RS/DC
  RESET-- RES
  */

//for tft screen pre setup
//---------------------------------------------------------------------------------------------



//---------------------------------------------------------------------------------------------
  //colors
  #define BLACK   0x0000
  #define BLUE    0x001F
  #define RED     0xF800
  #define GREEN   0x07E0
  #define CYAN    0x07FF
  #define MAGENTA 0xF81F
  #define YELLOW  0xFFE0
  #define WHITE   0xFFFF
  //#define GRAY  0x2408        //un-highlighted cross-hair
  #define GRAY      BLUE     //idle cross-hair colour
  #define GRAY_DONE RED      //finished cross-hair
  
  uint16_t        Display_Text_Color = GREEN;
  uint16_t        Display_Backround_Color = BLACK;
  uint16_t        Previous_Backround_Color1;
  
  int FontSizeStartUpMsg = 3;
  int FontSizeGearTxt = 16;
  int FontSizeVoltsTxt = 1;

#endif

    #if ( i2cMode == true )
    String errormessage = " ERROR \n\n arduino \n nano \n not \n detected.";

    #endif

char startingUpMsg[] = "Starting Up";


//colors
//---------------------------------------------------------------------------------------------
//main loops


void setup()
{
    #if (Mainserialmessages == true)
        Serial.begin(9600);
    #endif // (Mainserialmessages == true)

    #if (UARTserial == true)
        //Serial.begin(115200);
    #endif

    #if (i2cMode == true)
        i2cSetup();
    #endif // (i2cMode == true)


    #if (TFTmode == true)
        #if (Mainserialmessages == true)
            Serial.print("\tTFT mode\n");
        #endif
        Touch_setup();
    #endif

    #if (RCAmode == true)
        TVoutSetup();
    #endif // (RCAmode == true)

    #if (TFTmode == false && RCAmode == false)
    #if (Mainserialmessages == true)
        Serial.print("TFT & RCA mode both disabled\n");
        delay(200000); 
    #endif
    #endif // (TFTmode == false && RCAmode == false)

    #if (TFTmode == true && RCAmode == true) //also add tft printout /tv outprintout if both enabled ?
    #if (Mainserialmessages == true)
        serialprint("TFT & RCA mode both enabled\n");
        delay(200000); 
    #endif
    #endif // (TFTmode == true && RCAmode == true)

    //SD_setup();
    /*
    tft.setTextSize(FontSizeGearTxt); //to display gears avaiable

    for (int i = 0; i < amountOfGears; i++)
    {
        writeTxtDisplaySingle(gearRange[i]);
        delay(50);
        removeTxtDisplaySingle(gearRange[i]);
    }
    */
    #if (SerialDebugging == true )
        Serial.println("setting up nano");
    #endif
    setupnano();
    #if (SerialDebugging == true )
        Serial.println("finished setting up nano");
    #endif
    setupVoltageReader();
}

void loop()
{
    // i need this to only print when gear recieved starts with 1 and ignore untill it gets a 0
    //and i need to find out how the tft.print works. 
    //does it send the command to the display and keep going in the loop
    //or does it send the command and wait for the return to say it has been printed
    //either way i need to get the delay right once it has printed.

    //i2cSendData();
    //#if (SerialDebugging == true )
    //    Serial.println("starting main loop");
    //#endif

    gearRecieved();
    readVoltage();
    //////Serial.printprint("gearTimer "); ////Serial.printprintln(gearTimer);

}

void gearRecieved()
{ // most is broken in here
  
    #if (i2cMode == true ) // wtf is this doing ???

            loopNano();

        #if (TFTmode == true)
            //Wire.readBytes(replyFromSlave, ANSWERSIZE); //delay(350); reading max data of answersize

                //////Serial.printreadBytes(replyFromSlave, 5); //Read the //Serial.print data and store in var
                //////Serial.printprintln(replyFromSlave); //Print data on //Serial.print Monitor


            //Wire.beginTransmission(SLAVE_ADDRESS);

            //Wire.readBytes(replyFromSlave, ANSWERSIZE); //delay(350); reading max data of answersize
            //*replyFromSlave = Wire.read();
            //String temp = Wire.readString();

            //TvDisplayArray(inloopMsg);

            //////Serial.printprint("\nrecieved1: "); ////Serial.printprintln(temp);
            //////Serial.printprint("recieved2: "); ////Serial.printprintln(replyFromSlave);


            while (Wire.available()) {//i neeed to add that check function to only print a new gear, check last ect.
                // maybe i can add a function to ask nano for the next gear
                //if(tempReplyFromSlave[0] == )
                Wire.readBytes(replyFromSlave, ANSWERSIZE); //delay(350); reading max data of answersize

                *currentGear = replyFromSlave[1];// need to see if this is doing what i think it is doing
                *currentRecievedGear = replyFromSlave[1];
                *lastGear = replyFromSlave[2];
                gearRange[7] = replyFromSlave[1];


                //////Serial.printprint("recieved: "); ////Serial.println(replyFromSlave);//////Serial.print(response[0]); ////Serial.printprintln(response[1]);


            // i need to re write this so it constantly reads data ( that it is ) and if the data is a remove request 
            // if the current recieved data is the same as the last then ignore. but if it is different then make the change.
                if (replyFromSlave[0] == *"0")//removing
                {

                    //////Serial.printprint("\t\t\t\tremoving "); ////Serial.printprintln(lastGear);   

                    tft.setTextSize(FontSizeGearTxt);

                    removeTxtDisplayString(lastGear, 20, 0);  //why is this one so slow??? i'll have to try gear range
                    delay(250); 
                    //removeTxtDisplaySingle(*lastGear); 


                    inGearUno = false;
                }
                else if (replyFromSlave[0] == *"1") //writing
                {

                    if (inGearUno == false) {// this stops it writing to display over and over again.

                            //////Serial.printprint("printing "); ////Serial.println(replyFromSlave[1]);//delay(2500);
                            // now this can be tft.print to print once

                        tft.setTextSize(FontSizeGearTxt);

                        writeTxtDisplayString(currentGear, 20, 0); //why is this one so slow???
                        delay(250); 
                        inGearUno = true;
                    }

                    if (inGearUno == false)
                    {
                        timerFunction();
                        onTimeActions();

                    }
                }


                //////Serial.print("current: "); ////Serial.println(currentGear);
                //////Serial.print("\t\tlast: "); ////Serial.println(lastGear);
            }

            //maybe i can just use the "ingear" to tell it when to request so it doesn't constantly ask ..

            //Wire.requestFrom(SLAVE_ADDRESS, sizeOfMessageRecieve); //how often do i want to ask for a new gear?


        //if i2c avaiable //also add #if for this stuff

            Wire.requestFrom(SLAVE_ADDRESS, sizeOfMessageRecieve); //how often do i want to ask for a new gear?
            Wire.readBytes(replyFromSlave, ANSWERSIZE); //delay(350); reading max data of answersize
                //give me message "0N1", i'm expecting the size of that message

            //delay(5000);
            if (!Wire.available())// add tft print message if i2c coms broken.
            {
                TvDisplayArray(ingearrecieved3);
                #if (Mainserialmessages == true)
                    Serial.print("oh shit ");
                #endif

                writeTxtDisplayString(errormessage, 0, 0);
                //delay(500); 
                removeTxtDisplayString(errormessage, 0, 0);
            }

            //Wire.endTransmission(SLAVE_ADDRESS);


        //also this is for UART comms
            if (serial.available() > 0) {//i neeed to add that check function to only print a new gear, check last ect.
                // maybe i can add a function to ask nano for the next gear



                //byte c = serial.read();
                //if (c == '\`') {}

            *currentGear = replyFromSlave[1];// need to see if this is doing what i think it is doing
                *currentRecievedGear = replyFromSlave[1];
                *lastGear = replyFromSlave[2];
                gearRange[7] = replyFromSlave[1];


            //////Serial.print("recieved: "); ////Serial.println(replyFromSlave);//////Serial.print(response[0]); ////Serial.println(response[1]);


        // i need to re write this so it constantly reads data ( that it is ) and if the data is a remove request 
        // if the current recieved data is the same as the last then ignore. but if it is different then make the change.
            if (replyFromSlave[0] == *"0")//removing
            {

                //////Serial.print("\t\t\t\tremoving "); ////Serial.println(lastGear);   
                removeTxtDisplayString(lastGear, 20, 0);  //why is this one so slow??? i'll have to try gear range
                delay(250); 
                //removeTxtDisplaySingle(*lastGear); 


                inGearUno = false;
            }
            else if (replyFromSlave[0] == *"1") //writing
            {

                if (inGearUno == false) {// this stops it writing to display over and over again.

                        //////Serial.print("printing "); ////Serial.println(replyFromSlave[1]);//delay(2500);
                        // now this can be tft.print to print once

                    writeTxtDisplayString(currentGear, 20, 0); //why is this one so slow???
                    delay(250); 
                    inGearUno = true;
                }

                if (inGearUno == false)
                {
                    timerFunction();
                    onTimeActions();

                }
            }


            //////Serial.print("current: "); ////Serial.println(currentGear);
            //////Serial.print("\t\tlast: "); ////Serial.println(lastGear);
        }

        //maybe i can just use the "ingear" to tell it when to request so it doesn't constantly ask ..

        //Wire.requestFrom(SLAVE_ADDRESS, sizeOfMessageRecieve); //how often do i want to ask for a new gear?

            //give me message "0N1", i'm expecting the size of that message

        //delay(5000);

            if (!serial.available())// add tft print message if i2c coms broken.
            {
                writeTxtDisplayString(errormessage, 0, 0);
                //delay(500);
                removeTxtDisplayString(errormessage, 0, 0);
            }
            #endif    
    #endif // (i2cMode == true)


    #if (i2cMode == false )
    //loopNano(); not needed

        GearSelectnano();
        //Wire.readBytes(replyFromSlave, ANSWERSIZE); //delay(350); reading max data of answersize

        *currentGear = replyFromSlave[1];// need to see if this is doing what i think it is doing
        *currentRecievedGear = replyFromSlave[1];
        *lastGear = replyFromSlave[2];
        gearRange[7] = replyFromSlave[1];


        //Serial.print("recieved: "); Serial.println(replyFromSlave); 


    // i need to re write this so it constantly reads data ( that it is ) and if the data is a remove request 
    // if the current recieved data is the same as the last then ignore. but if it is different then make the change.
        if (replyFromSlave[0] == *"0")//removing
        {
            //Serial.println("am i here?");
            //Serial.println("is magic in here ?");

            //removeTxtDisplayString(lastGear, 20, 0);  //why is this one so slow??? i'll have to try gear range
            //delay(150); 
            //removeTxtDisplaySingle(*lastGear); 
            inGearUno = false;
        }
        else if (replyFromSlave[0] == *"1") //writing
        {
            //Serial.println("i know i am here?");

            if (inGearUno == false) {// this stops it writing to display over and over again.

                
                //writeTxtDisplayString(currentGear, 20, 0); //why is this one so slow???
                //delay(20); 
                inGearUno = true;
            }
            /*
            if (inGearUno == false)
            {
                timerFunction();
                onTimeActions();

            }*/
        }



    //maybe i can just use the "ingear" to tell it when to request so it doesn't constantly ask ..
        //give me message "0N1", i'm expecting the size of that message

    #endif // (i2cMode == true)
}

//main loops
//---------------------------------------------------------------------------------------------
//I2C setup
#if (i2cMode == true )
void i2cSetup()
{

    Wire.begin(MASTER_ADDRESS);        // join i2c bus (address optional for master)
//Wire.onRequest(requestEvent);
}

//I2C setup
//---------------------------------------------------------------------------------------------
//I2C functions
void requestEvent() { //for future use

  //Wire.write(message); // respond with message of 6 bytes
  // as expected by master
}
void i2cSendData() {

    //////Serial.println("Write data to slave");

    // Write a charatre to the Slave
    //Wire.beginTransmission(SLAVE_ADDRESS);
    //Wire.write("giveMore");
    //Wire.endTransmission();

    //////Serial.println("Receive data");
   // int i = 0;
    //while (Wire.available()) {

       // response[i] = Wire.read();
       // replyFromSlave[i] = Wire.read();
       // i++;
//}

}
#endif // i2cMode == true
//I2C functions
//---------------------------------------------------------------------------------------------
//for SD card functions

#if (SDcard == true)
void SD_setup()
{

    pinMode(10, OUTPUT);
    ////Serial.println("Initializing SD card..");
    if (!SD.begin(10)) {
        ////Serial.println("\ninitialization failed!");
        //while (1); //if sd card fails don't continue
    }
    ////Serial.println("\ninitialization done.");


    //s_width = my_lcd.Get_Display_Width();
    //s_heigh = my_lcd.Get_Display_Height();

    printDirectory(root, 0);

    //////Serial.print("Show BMP files on TFT with ID:0x");

    root = SD.open(namebuf);
    pathlen = strlen(namebuf);

    strcpy(file_name[0], bmpfile_01);
    strcpy(file_name[1], bmpfile_02);
    strcpy(file_name[2], bmpfile_03);
    strcpy(file_name[3], bmpfile_04);

}
void findFileToOpen() {
    char* nm = namebuf + pathlen;
    File f = root.openNextFile();
    uint8_t ret;
    uint32_t start;
    if (f != NULL) {
#ifdef USE_SDFAT
        f.getName(nm, 32 - pathlen);
#else
        strcpy(nm, (char*)f.name());
#endif
        f.close();
        strlwr(nm);
        if (strstr(nm, ".bmp") != NULL && strstr(nm, NAMEMATCH) != NULL) {
            ////Serial.print(namebuf);
            ////Serial.print(F(" - "));
            tft.fillScreen(0);
            start = millis();
            ret = showBMP(namebuf, 5, 5);
            switch (ret) {
            case 0:
                ////Serial.print(millis() - start);
                ////Serial.println(F("ms"));
                delay(5000);
                break;
            case 1:
                ////Serial.println(F("bad position"));
                break;
            case 2:
                ////Serial.println(F("bad BMP ID"));
                break;
            case 3:
                ////Serial.println(F("wrong number of planes"));
                break;
            case 4:
                ////Serial.println(F("unsupported BMP format"));
                break;
            case 5:
                ////Serial.println(F("unsupported palette"));
                break;
            default:
                ////Serial.println(F("unknown"));
                break;
            }
        }
    }
    else root.rewindDirectory();
}
uint8_t showBMP(char* nm, int x, int y)
{
    File bmpFile;
    int bmpWidth, bmpHeight;    // W+H in pixels
    uint8_t bmpDepth;           // Bit depth (currently must be 24, 16, 8, 4, 1)
    uint32_t bmpImageoffset;    // Start of image data in file
    uint32_t rowSize;           // Not always = bmpWidth; may have padding
    uint8_t sdbuffer[3 * BUFFPIXEL];    // pixel in buffer (R+G+B per pixel)
    uint16_t lcdbuffer[(1 << PALETTEDEPTH) + BUFFPIXEL], * palette = NULL;
    uint8_t bitmask, bitshift;
    boolean flip = true;        // BMP is stored bottom-to-top
    int w, h, row, col, lcdbufsiz = (1 << PALETTEDEPTH) + BUFFPIXEL, buffidx;
    uint32_t pos;               // seek position
    boolean is565 = false;      //

    uint16_t bmpID;
    uint16_t n;                 // blocks read
    uint8_t ret;

    if ((x >= tft.width()) || (y >= tft.height()))
        return 1;               // off screen

    bmpFile = SD.open(nm);      // Parse BMP header
    bmpID = read16(bmpFile);    // BMP signature
    (void)read32(bmpFile);     // Read & ignore file size
    (void)read32(bmpFile);     // Read & ignore creator bytes
    bmpImageoffset = read32(bmpFile);       // Start of image data
    (void)read32(bmpFile);     // Read & ignore DIB header size
    bmpWidth = read32(bmpFile);
    bmpHeight = read32(bmpFile);
    n = read16(bmpFile);        // # planes -- must be '1'
    bmpDepth = read16(bmpFile); // bits per pixel
    pos = read32(bmpFile);      // format
    if (bmpID != 0x4D42) ret = 2; // bad ID
    else if (n != 1) ret = 3;   // too many planes
    else if (pos != 0 && pos != 3) ret = 4; // format: 0 = uncompressed, 3 = 565
    else if (bmpDepth < 16 && bmpDepth > PALETTEDEPTH) ret = 5; // palette
    else {
        bool first = true;
        is565 = (pos == 3);               // ?already in 16-bit format
        // BMP rows are padded (if needed) to 4-byte boundary
        rowSize = (bmpWidth * bmpDepth / 8 + 3) & ~3;
        if (bmpHeight < 0) {              // If negative, image is in top-down order.
            bmpHeight = -bmpHeight;
            flip = false;
        }

        w = bmpWidth;
        h = bmpHeight;
        if ((x + w) >= tft.width())       // Crop area to be loaded
            w = tft.width() - x;
        if ((y + h) >= tft.height())      //
            h = tft.height() - y;

        if (bmpDepth <= PALETTEDEPTH) {   // these modes have separate palette
            //bmpFile.seek(BMPIMAGEOFFSET); //palette is always @ 54
            bmpFile.seek(bmpImageoffset - (4 << bmpDepth)); //54 for regular, diff for colorsimportant
            bitmask = 0xFF;
            if (bmpDepth < 8)
                bitmask >>= bmpDepth;
            bitshift = 8 - bmpDepth;
            n = 1 << bmpDepth;
            lcdbufsiz -= n;
            palette = lcdbuffer + lcdbufsiz;
            for (col = 0; col < n; col++) {
                pos = read32(bmpFile);    //map palette to 5-6-5
                palette[col] = ((pos & 0x0000F8) >> 3) | ((pos & 0x00FC00) >> 5) | ((pos & 0xF80000) >> 8);
            }
        }

        // Set TFT address window to clipped image bounds
        tft.setAddrWindow(x, y, x + w - 1, y + h - 1);
        for (row = 0; row < h; row++) { // For each scanline..
            // Seek to start of scan line.  It might seem labor-
            // intensive to be doing this on every line, but this
            // method covers a lot of gritty details like cropping
            // and scanline padding.  Also, the seek only takes
            // place if the file position actually needs to change
            // (avoids a lot of cluster math in SD library).
            uint8_t r, g, b, * sdptr;
            int lcdidx, lcdleft;
            if (flip)   // Bitmap is stored bottom-to-top order (normal BMP)
                pos = bmpImageoffset + (bmpHeight - 1 - row) * rowSize;
            else        // Bitmap is stored top-to-bottom
                pos = bmpImageoffset + row * rowSize;
            if (bmpFile.position() != pos) { // Need seek?
                bmpFile.seek(pos);
                buffidx = sizeof(sdbuffer); // Force buffer reload
            }

            for (col = 0; col < w; ) {  //pixels in row
                lcdleft = w - col;
                if (lcdleft > lcdbufsiz) lcdleft = lcdbufsiz;
                for (lcdidx = 0; lcdidx < lcdleft; lcdidx++) { // buffer at a time
                    uint16_t color;
                    // Time to read more pixel data?
                    if (buffidx >= sizeof(sdbuffer)) { // Indeed
                        bmpFile.read(sdbuffer, sizeof(sdbuffer));
                        buffidx = 0; // Set index to beginning
                        r = 0;
                    }
                    switch (bmpDepth) {          // Convert pixel from BMP to TFT format
                    case 24:
                        b = sdbuffer[buffidx++];
                        g = sdbuffer[buffidx++];
                        r = sdbuffer[buffidx++];
                        color = tft.color565(r, g, b);
                        break;
                    case 16:
                        b = sdbuffer[buffidx++];
                        r = sdbuffer[buffidx++];
                        if (is565)
                            color = (r << 8) | (b);
                        else
                            color = (r << 9) | ((b & 0xE0) << 1) | (b & 0x1F);
                        break;
                    case 1:
                    case 4:
                    case 8:
                        if (r == 0)
                            b = sdbuffer[buffidx++], r = 8;
                        color = palette[(b >> bitshift) & bitmask];
                        r -= bmpDepth;
                        b <<= bmpDepth;
                        break;
                    }
                    lcdbuffer[lcdidx] = color;

                }
                tft.pushColors(lcdbuffer, lcdidx, first);
                first = false;
                col += lcdidx;
            }           // end cols
        }               // end rows
        tft.setAddrWindow(0, 0, tft.width() - 1, tft.height() - 1); //restore full screen
        ret = 0;        // good render
    }
    bmpFile.close();
    return (ret);
}
void printDirectory(File dir, int numTabs) {

    while (true) {
        File entry = dir.openNextFile();
        if (!entry) {
            // no more files
            break;
        }
        for (uint8_t i = 0; i < numTabs; i++) {
            ////Serial.print('\t');
        }
        ////Serial.print(entry.name());
        if (entry.isDirectory()) {
            ////Serial.println("/");
            printDirectory(entry, numTabs + 1);
        }
        else {
            // files have sizes, directories do not
            ////Serial.print("\t\t");
            ////Serial.println(entry.size(), DEC);
        }
        entry.close();
    }
}
//for SD card functions
#endif
//---------------------------------------------------------------------------------------------
//for touch screen functions

//change this to TFT setup
//and make another for touchscreen mode

void Touch_setup() {
#if (TFTmode == true)
    //char correctID[7] = "0x9486";
    amountOfGears = amountOfGears - 2; // -2 for extra in gear range or -4 for the string terminating characters
    tft.initR(INITR_144GREENTAB); // Init ST7735R chip, green tab
    tft.fillScreen(Display_Backround_Color);
    //tft.setTextColor(Display_Text_Color);
    tft.setTextColor(Display_Text_Color,Display_Backround_Color);
    tft.setCursor(0, 0);

    #if (touchScreenMode == true)
        /*
            if (TOUCH_ORIENTATION == LANDSCAPE) {

                tft.setCursor(0, 0);
                tft.setTextColor(WHITE, BLACK);
                tft.println("landscape mode");
                tft.setRotation(TOUCH_ORIENTATION);
                delay(1000);
            }
            else if (TOUCH_ORIENTATION == PORTRAIT) {

                tft.setCursor(0, 0);
                tft.setTextColor(WHITE, BLACK);
                tft.println("PORTRAIT mode");
                tft.setRotation(TOUCH_ORIENTATION);
                delay(1000);
            }

        */
        #endif
        //i need to declare lcd pins here
        pinMode(TFT_SCLK, OUTPUT); delay(10); 
        pinMode(TFT_MOSI, OUTPUT); delay(10);
        pinMode(TFT_CS, OUTPUT);   delay(10);
        pinMode(TFT_RST, OUTPUT);  delay(10);
        pinMode(TFT_DC, OUTPUT);   delay(10);

        //-- writeing data to display
        tft.setTextSize(FontSizeStartUpMsg);
        writeTxtDisplayString(startingUpMsg, 0, 0);

        // -- wring over data on display to erase it
        delay(50); 
        removeTxtDisplayString(startingUpMsg, 0, 0);
        //tft.setTextSize(FontSizeGearTxt);
    #endif
}

void writeTxtDisplaySingle(String displayMessage, int x, int y) //single Characters
{
    #if (TFTmode == true)
        tft.setCursor(x, y);
        //tft.setTextColor(Display_Text_Color);
        tft.setTextColor(Display_Text_Color);
        #if (SerialDebugging == true )
            #if (Mainserialmessages == true)
                Serial.print("unoWriteSingle: "); Serial.println(displayMessage);
            #endif
        #endif
        tft.setTextSize(FontSizeGearTxt);
        tft.print(displayMessage);
        //delay(100);

    #endif

    #if(RCAmode == true)
        TV.println(displayMessage);
        TV.delay(3000);
        TV.clear_screen();
    #endif
}
void removeTxtDisplaySingle(String displayMessage, int x, int y) //single Characters
{
    #if (TFTmode == true)
        tft.setCursor(x, y);
        tft.setTextColor(Display_Backround_Color);
        #if (SerialDebugging == true )
            #if (Mainserialmessages == true)
                Serial.print("unoRemoveSingle: "); Serial.println(displayMessage);
            #endif
        #endif
        tft.setTextSize(FontSizeGearTxt);
        tft.print(displayMessage);
        //delay(100);





    #endif
    #if(RCAmode == true)
        TvDisplayArray(&displayMessage);
    #endif
}

void writeTxtDisplayString(String displayMessage, int x, int y)
{//                 string of characters  

    #if (TFTmode == true)
    
        //int lenDisplayMsg = displayMessage.length();   
        //Wire.endTransmission(SLAVE_ADDRESS);
        #if (i2cMode == true )
            if (displayMessage == errormessage)
            {
                tft.setCursor(x, y);
                tft.setTextColor(Display_Text_Color);
                tft.setTextColor(Display_Text_Color,Display_Backround_Color);
                tft.setTextSize(2);
                tft.print(displayMessage);
            }
            //else {
            #endif
            const char* displayMessageChar = displayMessage.c_str();

            if(strcmp(displayMessageChar,startingUpMsg) == 0 ){
                tft.setTextSize(FontSizeStartUpMsg);
            }
            else{
                tft.setTextSize(FontSizeVoltsTxt);
            }
            tft.setCursor(x, y);
            //tft.setTextColor(Display_Text_Color);
            tft.setTextColor(Display_Text_Color,Display_Backround_Color);


            //tft.setTextSize(FontSizeGearTxt);
            //Previous_Backround_Color = tft.readPixel(x, y);

            //whats faster to print, entire thing at once or 1 character at a time
            /*for (int i = 0; i <= lenDisplayMsg; i++) {
                //prevColorArray[Previous_Backround_Color] = tft.readPixel(x+i, y);

                //tft.print(displayMessage[i]);
                //prevColorArray[Previous_Backround_Color]++;
            }*/
            #if (SerialDebugging == true )
                #if (Mainserialmessages == true)
                    Serial.print("unoWriteString: "); Serial.println(displayMessage); // over here
                #endif
            #endif
            tft.print(displayMessage);
            //delay(50); 
    #if (tftModeDebug == true)
            ////Serial.print("\n\t\tprinting "); ////Serial.println(displayMessage);
    #endif
    #if (i2cMode == true )
        //}
        //Wire.beginTransmission(SLAVE_ADDRESS);
    #endif

    #endif
}
void removeTxtDisplayString(String displayMessage, int x, int y)
{//                   string of characters   
    #if (TFTmode == true)
    //int lenDisplayMsg = displayMessage.length();
    //Wire.endTransmission(SLAVE_ADDRESS);
        #if (i2cMode == true ) 
            if (displayMessage == errormessage)
            {
                tft.setCursor(x, y);
                tft.setTextColor(Display_Backround_Color);
                tft.setTextSize(2);
                tft.print(displayMessage);
            }
        #endif
        //up to here !!
        //need to create code for determing incoming data is a gear range or if it will be volts data
        //and set the text size accordingly?
        //or just hardcode it so gears data never enter this statement.
        const char* displayMessageChar = displayMessage.c_str();

        if(strcmp(displayMessageChar,startingUpMsg) == 0 ){
            tft.setTextSize(FontSizeStartUpMsg);
        }
        else{
            tft.setTextSize(FontSizeVoltsTxt);
        }

        //tft.setTextSize(FontSizeStartUpMsg);
        //tft.setTextSize(FontSizeVoltsTxt);
        tft.setCursor(x, y);
        tft.setTextColor(Display_Backround_Color);
        //whats faster to print, entire thing at once or 1 character at a time
        /*for (int i = 0; i <= lenDisplayMsg; i++) {

        //tft.setTextColor(prevColorArray[Previous_Backround_Color]);
        //tft.print(displayMessage[i]);
        //prevColorArray[Previous_Backround_Color]++;
        }*/

        //tft.setTextColor(Previous_Backround_Color);
        #if (SerialDebugging == true )
            #if (Mainserialmessages == true)
                Serial.print("unoRemoveString: "); Serial.println(displayMessage); //over here
            #endif
        #endif
        tft.print(displayMessage);
        //delay(50); 
        //Serial.print("\n\t\t\tUNOremoving "); Serial.print(displayMessage);  
        #if (i2cMode == true )
        //}
        #endif
    //Wire.beginTransmission(SLAVE_ADDRESS);
    #endif
}

//for touch screen funtions
//---------------------------------------------------------------------------------------------

void timerFunction()
{ // need to find out why this doesn't work all the time
//perhaps use //Serial.monitor to check the seconds. i think it may be messing up because if the seconds go over 10 it has error 404
    #if (TFTmode == true)
        if (millis() >= (previousTime)) {
            previousTime = previousTime + 1000;  // use 100000 for uS
            seconds = seconds + 1;
            #if (Mainserialmessages == true)
                Serial.print(" seconds: ");
                Serial.println(seconds);
            #endif

            if (seconds == 60)
            {
                seconds = 0;
                minutes = minutes + 1;
            }
            if (minutes == 60)
            {
                minutes = 0;
                hours = hours + 1;
            }
        } // end 1 second
    #endif
}
void onTimeActions()
{ //this lets me display nutual gear after set time period
#if (TFTmode == true)

      if (seconds == 60)
      {
        #if (TFTmode == true)
                tft.setTextSize(FontSizeGearTxt);
        #endif
                
          removeTxtDisplayString(lastGear, 20, 0);  //why is this one so slow??? i'll have to try gear range 
          //delay(50); 
          writeTxtDisplayString(currentGear, 20, 0); //why is this one so slow???
      }
#endif

}
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void setupnano() {

    #if (SerialDebugging == true )
        Serial.println("setting up pins");
    #endif
    #if (DigitalGearPins == false)
        pinMode(switchUP, INPUT_PULLUP);delay(10);// what does this do ?
        pinMode(switchDOWN, INPUT_PULLUP);delay(10);
        pinMode(switchLEFT, INPUT_PULLUP);delay(10);
        pinMode(switchRIGHT, INPUT_PULLUP);delay(10);
    #endif
    #if (DigitalGearPins == true)
        pinMode(switchUP, INPUT_PULLUP);delay(10);// what does this do ?
        pinMode(switchDOWN, INPUT_PULLUP);delay(10);
        pinMode(switchLEFT, INPUT_PULLUP);delay(10);
        pinMode(switchRIGHT, INPUT_PULLUP);delay(10);
    #endif
    //pinMode(LED_BUILTIN, OUTPUT);


    #if (RCAmode == false)
    #if (i2cMode == true )
        i2cSetupnano();
    #endif
        amountOfGears = strlen(gearRange);
    #endif

    #if (RCAmode == true)
        TVoutSetup();
    #endif
    #if (SerialDebugging == true )
        Serial.println("finished pins configuring");
    #endif
}
void loopNano() {
    
    //TV.print(help);
    //tvoutDemo();
    GearSelectnano();
    #if (i2cMode == true)
        if (inGearNano == true) { //maybe i can make a real request thing?


            Wire.onRequest(sendToUnoEventnano); // need to be careful with this one

            inGearNano = false;                 // because master (uno) is constantly requesting data this fucks shit up.
        }
    #endif

    #if (GearDebuggingAutomatic == true)
        debugGearTimer++;
    #endif

}
//---------------------------------------------------------------------------------------------
//I2C setup
#if (i2cMode == true)
void i2cSetupnano()
{
    // Initialize I2C communications as Slave

    Wire.begin(SLAVE_ADDRESS);

    // Function to run when data requested from master

    //Wire.onRequest(sendToUnoEventnano); // need to be careful with this one, because

    // Function to run when data received from master

    //Wire.onReceive(receiveEventnano );
    // Setup Serial Monitor     
    //Serial.println("I2C Slave Demonstration");
}
#endif
//I2C setup
//---------------------------------------------------------------------------------------------
//I2C functions
void receiveEventnano() {//wtf is this doing?
//#if (RCAmode == false)
  #if (i2cMode == true )
    String recievedString;
    char recievedChar[10] = "";
    // Read while data received
    while (0 < Wire.available()) {
        //while (Wire.available()) {
        char x = Wire.read();
        recievedString += x;

        //Serial.print(x);
    }
    for (int i = 0; i < sizeof(recievedString) + 1; i++)
    {
        recievedChar[i] = recievedString[i];
    }


    if (strcmp(recievedChar, "giveMore") == 0)
    {   
        #if (Mainserialmessages == true)
            Serial.println("helpmeplz\n");
        #endif
    }

    //Serial.print("\n");
    // Print to Serial Monitor
    //Serial.print("Received event: ");
    //Serial.println(recievedChar);
#endif

}
void sendToUnoEventnano() {

    //Serial.println(sendToUno);

    // Send response back to Master  

    //int returnValue = 0;
    //returnValue = getLastGearnano();
    
    *currentGear = writeOrRemove[1];// need to see if this is doing what i think it is doing
    *currentRecievedGear = writeOrRemove[1];
    *lastGear = writeOrRemove[2];
    gearRange[7] = writeOrRemove[1];

    getLastGearnano();

    #if (GearDebuggingManual == true)
        #if (Mainserialmessages == true)
            Serial.print("current: "); Serial.println(currentGear);
            Serial.print("\t\tlast: "); Serial.println(lastGear);

            Serial.print("WOR: "); Serial.println(writeOrRemove);
        #endif
    #endif
    if (writeOrRemove[0] == *"0")
    {
        //*lastGear = writeOrRemove[1];

            *trueLastGear = writeOrRemove[1];
            //strcpy(writeOrRemove[2], trueLastGear);

        #if (i2cMode == true)
            #if (Mainserialmessages == true)
                Serial.print("\tremoving: ");
                Serial.println(writeOrRemove);
            #endif


                    //Wire.beginTransmission(MASTER_ADDRESS);
                    Wire.write(writeOrRemove, ANSWERSIZE);
                    //Wire.endTransmission(MASTER_ADDRESS);
                    delay(10);
        #endif
        #if (i2cMode == false)
                            //Serial.print("\tSTUremoving: ");
                            //Serial.println(writeOrRemove);
                            strcpy(replyFromSlave, writeOrRemove);
                            //tft.fillScreen(Display_Backround_Color);
                                                            //removeTxtDisplayString(replyFromSlave, 20, 0); //over here temp fix
                            //delay(10); 
                            #if(SerialDebugging == true )
                                #if (Mainserialmessages == true)
                                    Serial.println("/t/tis it over here2??");
                                #endif
                            #endif

                            #if(writeGearStringSingle == true)
                                removeTxtDisplaySingle(trueLastGear, 20, 0);
                            #endif
                            #if(writeGearStringSingle == false)
                                removeTxtDisplayString(trueLastGear, 20, 0);  //why is this one so slow??? i'll have to try gear range
                            #endif


                            //delay(50); 
                            //removeTxtDisplaySingle(*lastGear); 
                            inGearUno = false;


        #endif

        #if (RCAmode == true)
                    TV.delay(200);
                    TV.clear_screen();
        #endif         
    }
    else if (writeOrRemove[0] == *"1")
    {
        writeOrRemove[2] = *trueLastGear;
        #if (i2cMode == true)
            #if (Mainserialmessages == true)
                Serial.print("\t   writing: ");
                Serial.println(writeOrRemove);
            #endif

                //Wire.beginTransmission(MASTER_ADDRESS);
                Wire.write(writeOrRemove, ANSWERSIZE);
                //Wire.endTransmission(MASTER_ADDRESS);
                delay(10);
        #endif

        #if (i2cMode == false)
            //Serial.print("\t   STUwriting: ");
            //Serial.println(writeOrRemove);
            strcpy(replyFromSlave, writeOrRemove);
            //delay(10); 

            //Serial.println("i know i am here?");

            if (inGearUno == false) {// this stops it writing to display over and over again.
                #if(SerialDebugging == true )
                    #if (Mainserialmessages == true)
                        Serial.println("/t/tis it over here1??");
                    #endif
                #endif

                #if(writeGearStringSingle == true)
                    writeTxtDisplaySingle(currentGear, 20, 0);
                #endif
                #if(writeGearStringSingle == false)
                    writeTxtDisplayString(currentGear, 20, 0); //why is this one so slow???
                #endif
                //delay(50); 
                inGearUno = true;
            }
        #endif

                        
        if (inGearUno == false) //this lets me display nutual gear after set time period
        {
            timerFunction();
            onTimeActions();
        }

        #if (RCAmode == true)
                TV.print(writeOrRemove[1]);
        #endif
    }
    
}
//I2C functions
void GearSelectnano()
{

    #if (GearNormalMode == true) //i could add the or if both debugging are off
        readSwitchesnano();
    #endif // (GearDebuggingAutomatic == false)


    #if (GearDebuggingManual == true)
        //debugReadSwitchesnano();
        debugSerialInputSwitchesnano();
        debugReadSwitchesnano();
    #endif // (GearDebuggingAutomatic == true)

    #if ( GearDebuggingAutomatic  == true)
        debugReadSwitchesnano();
    #endif
    //1st/2nd gears
    if (stateofsensorLeft == true) {

        if (stateofsensorUp == true) {
            if (strcmp(currentGear, gear1) != 0) {
                removeTxtDisplaySinglenano(gearRange[LastGear]); // is last gear the same as gear1 if not enter statement and remove the last gear
                                                             // if last gear the same as gear1 ignore removing it
                strcpy(&gearRange[LastGear], gear1);
                strcpy(lastGear, gear1);
            }
            if (strcmp(currentGear, gear1) != 0) {
                writeTxtDisplaySinglenano(gearRange[First]);   //is current gear the same as gear1 if it is enter and write the first gear
                                                           //is current gear the same as gear1 if its not ignore it
                strcpy(&gearRange[CurrentGear], gear1);
                strcpy(currentGear, gear1);

            }

        }
        
        else if (stateofsensorDown == true) {
            if (strcmp(currentGear, gear2) != 0) {
                removeTxtDisplaySinglenano(gearRange[LastGear]); // is last gear the same as gear1 if not enter statement and remove the last gear
                                                             // if last gear the same as gear1 ignore removing it
                strcpy(&gearRange[LastGear], gear2);
                strcpy(lastGear, gear2);
            }
            if (strcmp(currentGear, gear2) != 0) {
                writeTxtDisplaySinglenano(gearRange[Second]);   //is current gear the same as gear1 if it is enter and write the first gear
                                                          //is current gear the same as gear1 if its not ignore it
                strcpy(&gearRange[CurrentGear], gear2);
                strcpy(currentGear, gear2);

            }

        }
        
        else if (stateofsensorUp == false && stateofsensorDown == false)// inbetween first/second gear show nutural
        {
            //start nutural timer
            if (nuturalGear == false) {
                //startMillis = millis();
                nuturalTimerFunctionnano();
            }
            if (nuturalGear == true)
            {
                if (strcmp(currentGear, gear0) != 0) {
                    removeTxtDisplaySinglenano(gearRange[LastGear]); // is last gear the same as gear1 if not enter statement and remove the last gear
                                                                 // if last gear the same as gear1 ignore removing it
                    strcpy(&gearRange[LastGear], gear0);
                    strcpy(lastGear, gear0);
                }
                if (strcmp(currentGear, gear0) != 0) {
                    writeTxtDisplaySinglenano(gearRange[Nutural]);   //is current gear the same as gear1 if it is enter and write the first gear
                                                                 //is current gear the same as gear1 if its not ignore it
                    strcpy(&gearRange[CurrentGear], gear0);
                    strcpy(currentGear, gear0);

                }
            }
        }
        
    }
    // 5th/reverse gears   
    else if (stateofsensorRight == true) {

        if (stateofsensorUp == true) {//5th gear
            if (strcmp(currentGear, gear5) != 0) {
                removeTxtDisplaySinglenano(gearRange[LastGear]); // is last gear the same as gear1 if not enter statement and remove the last gear
                                                 // if last gear the same as gear1 ignore removing it
                strcpy(&gearRange[LastGear], gear5);
                strcpy(lastGear, gear5);
            }
            if (strcmp(currentGear, gear5) != 0) {
                writeTxtDisplaySinglenano(gearRange[Fifth]);   //is current gear the same as gear1 if it is enter and write the first gear
                                                          //is current gear the same as gear1 if its not ignore it
                strcpy(&gearRange[CurrentGear], gear5);
                strcpy(currentGear, gear5);
            }

        }
        else if (stateofsensorDown == true) {//reverse gear

            if (strcmp(currentGear, gear6) != 0) {
                removeTxtDisplaySinglenano(gearRange[LastGear]); // is last gear the same as gear1 if not enter statement and remove the last gear
                                                     // if last gear the same as gear1 ignore removing it
                strcpy(&gearRange[LastGear], gear6);
                strcpy(lastGear, gear6);
            }
            if (strcmp(currentGear, gear6) != 0) {
                writeTxtDisplaySinglenano(gearRange[Sixth]);   //is current gear the same as gear1 if it is enter and write the first gear
                                                          //is current gear the same as gear1 if its not ignore it
                strcpy(&gearRange[CurrentGear], gear6);
                strcpy(currentGear, gear6);
            }
        }
        else if (stateofsensorUp == false && stateofsensorDown == false)
        {
            //start nutural timer
            if (nuturalGear == false) {
                //startMillis = millis();
                nuturalTimerFunctionnano();
            }
            if (nuturalGear == true)
            {

                if (strcmp(currentGear, gear0) != 0) {
                    removeTxtDisplaySinglenano(gearRange[LastGear]); // is last gear the same as gear1 if not enter statement and remove the last gear
                                                                    // if last gear the same as gear1 ignore removing it
                    strcpy(&gearRange[LastGear], gear0);
                    strcpy(lastGear, gear0);
                }
                if (strcmp(currentGear, gear0) != 0) {
                    writeTxtDisplaySinglenano(gearRange[Nutural]);   //is current gear the same as gear1 if it is enter and write the first gear
                                                                    //is current gear the same as gear1 if its not ignore it
                    strcpy(&gearRange[CurrentGear], gear0);
                    strcpy(currentGear, gear0);
                }
            }
        }


    }
    // 3rd/4th gears
    else if (stateofsensorUp == false || stateofsensorDown == false)
    {
        if (stateofsensorUp == true) { //3rd gear

            if (strcmp(currentGear, gear3) != 0) {
                removeTxtDisplaySinglenano(gearRange[LastGear]); // is last gear the same as gear1 if not enter statement and remove the last gear
                                                     // if last gear the same as gear1 ignore removing it
                strcpy(&gearRange[LastGear], gear3);
                strcpy(lastGear, gear3);
            }
            if (strcmp(currentGear, gear3) != 0) {
                writeTxtDisplaySinglenano(gearRange[Thrid]);   //is current gear the same as gear1 if it is enter and write the first gear
                                                          //is current gear the same as gear1 if its not ignore it
                strcpy(&gearRange[CurrentGear], gear3);
                strcpy(currentGear, gear3);

            }
        }
        else if (stateofsensorDown == true) { //4th gear    
            if (strcmp(currentGear, gear4) != 0) {
                removeTxtDisplaySinglenano(gearRange[LastGear]); // is last gear the same as gear1 if not enter statement and remove the last gear
                                                     // if last gear the same as gear1 ignore removing it
                strcpy(&gearRange[LastGear], gear4);
                strcpy(lastGear, gear4);
            }
            if (strcmp(currentGear, gear4) != 0) {
                writeTxtDisplaySinglenano(gearRange[Forth]);   //is current gear the same as gear1 if it is enter and write the first gear
                                                          //is current gear the same as gear1 if its not ignore it
                strcpy(&gearRange[CurrentGear], gear4);
                strcpy(currentGear, gear4);


            }
        }
        else //nutural gear
        {
            //start nutural timer
            if (nuturalGear == false) {
                //startMillis = millis();
                nuturalTimerFunctionnano();
            }
            if (nuturalGear == true)
            {
                if (strcmp(currentGear, gear0) != 0) {
                    removeTxtDisplaySinglenano(gearRange[LastGear]); // is last gear the same as gear1 if not enter statement and remove the last gear
                                                                 // if last gear the same as gear1 ignore removing it
                    strcpy(&gearRange[LastGear], gear0);
                    strcpy(lastGear, gear0);
                }
                if (strcmp(currentGear, gear0) != 0) {
                    writeTxtDisplaySinglenano(gearRange[Nutural]);   //is current gear the same as gear1 if it is enter and write the first gear
                                                                 //is current gear the same as gear1 if its not ignore it
                    strcpy(&gearRange[CurrentGear], gear0);
                    strcpy(currentGear, gear0);

                }
            }
        }

    }
    
}
//single characters to print
void writeTxtDisplaySinglenano(char displayMessage)
{                                                //single Characters
    WOR = 1;
    
    if (stateofsensorUp == true) {//this helps prevent nutual showing in between gears
        nuturalGear = false;
    }
    if (stateofsensorDown == true) {//this helps prevent nutual showing in between gears
        nuturalGear = false;
    }

    strcpy(sendToUno, &displayMessage);
    //Serial.print("\t\t\t\t\t\tdisplayMessage"); Serial.println(displayMessage);
    inGearNano = true;

    if (WOR == 1) {
        strcpy(&writeOrRemove[0], "1");
        writeOrRemove[1] = *sendToUno;
        strcpy(&writeOrRemove[2], lastGear);

        //Serial.print("currentGear"); Serial.println(currentGear);
    }
    //delay(50); 
    sendToUnoEventnano();

}
void removeTxtDisplaySinglenano(char displayMessage)
{                                                   //single Characters
    WOR = 0;
    //Serial.print("\t\t\t\t\t\tdisplayMessage"); Serial.println(displayMessage);
    strcpy(sendToUno, &displayMessage);
    inGearNano = false;

    if (WOR == 0) {
        strcpy(&writeOrRemove[0], "0");
        writeOrRemove[1] = *sendToUno;
        strcpy(&writeOrRemove[2], lastGear);
    }
    //delay(50); 
    sendToUnoEventnano();

}
//strings to print
void writeTxtDisplayStringnano(char displayMessage[])
{//                                            string of characters  

    #if(SerialDebugging == true )
        #if (Mainserialmessages == true)
            Serial.print("WriteString\t");
            Serial.println(displayMessage);
        #endif
    #endif
    //strcpy(sendToUno, &displayMessage[0]);

}
void removeTxtDisplayStringnano(char displayMessage[])
{//                                            string of characters
    #if(SerialDebugging == true )
        #if (Mainserialmessages == true)
            Serial.print("RemoveString\t");
            Serial.println(displayMessage);
        #endif
    #endif
}

void nuturalTimerFunctionnano() {


    currentMillis = millis();  //get the current "time" (actually the number of milliseconds since the program started)
    // might be able to do a simalir thing .. i just need to read the current mills. and after I'VE counted the time. have the if statement trigger on that current mills number

    //if (currentMillis >= period)  // if greater than should probally do if the value is inbetween this range...
    
    if (currentMillis - startMillis >= period)
    {
        startMillis = currentMillis;  //IMPORTANT to save the start time of the current LED state.
        seconds = seconds + 1;
    }
    
    if (nuturalGear == false) {
        if (seconds == timeToEnableNuturalGear) {
            nuturalGear = true;
            seconds = 0;
        }
        else if (seconds >= timeToEnableNuturalGear + 2) { seconds = 0; } //prevents millis overflow
    }

    else {
        nuturalGear = false;
    }
    //# if(MainSerialmessages == true )
        //Serial.print(" seconds: ");
        //Serial.println(seconds);
    //#endif

}
void readSwitchesnano()
{

    #if (DigitalGearPins == flase)
        float temp1 = analogRead(switchUP); delay(10);
        float temp2 = analogRead(switchDOWN); delay(10);
        float temp3 = analogRead(switchLEFT); delay(10);
        float temp4 = analogRead(switchRIGHT); delay(10);
        float voltageUP;
        float voltageDOWN;
        float voltageLEFT;
        float voltageRIGHT;
    #endif
    #if (DigitalGearPins == true)
        float temp1 = digitalRead(switchUP); delay(10);
        float temp2 = digitalRead(switchDOWN); delay(10);
        float temp3 = digitalRead(switchLEFT); delay(10);
        float temp4 = digitalRead(switchRIGHT); delay(10);
    #endif

    #if (DigitalGearPins == flase)
        if (temp1 < 1000) {
            voltageUP = temp1 * (5.0 / 1023.0);
        }
        else {
            voltageUP = 0;
        }
        if (temp2 < 1000) {
            voltageDOWN = temp2 * (5.0 / 1023.0);
        }
        else {
            voltageDOWN = 0;
        }
        if (temp3 < 1000) {
            voltageLEFT = temp3 * (5.0 / 1023.0);
        }
        else {
            voltageLEFT = 0;
        }
        if (temp4 < 1000) {
            voltageRIGHT = temp4 * (5.0 / 1023.0);
        }
        else {
            voltageRIGHT = 0;
        }

        if (voltageUP > 2.0) {
            stateofsensorUp = true;
        }
        else {
            stateofsensorUp = false;
        }
        if (voltageDOWN > 2.0) {
            stateofsensorDown = true;
        }
        else {
            stateofsensorDown = false;
        }
        if (voltageLEFT > 2.0) {
            stateofsensorLeft = true;
        }
        else {
            stateofsensorLeft = false;
        }
        if (voltageRIGHT > 2.0) {
            stateofsensorRight = true;
        }
        else {
            stateofsensorRight = false;
        }
    #endif
    
    #if (DigitalGearPins == true)
        if (temp1 == LOW) {
            //voltageUP = temp1 * (5.0 / 1023.0);
            stateofsensorUp = true;
        }
        else {
            //voltageUP = 0;
            stateofsensorUp = false;
        }
        if (temp2 == LOW) {
            //voltageDOWN = temp2 * (5.0 / 1023.0);
            stateofsensorDown = true;
        }
        else {
            //voltageDOWN = 0;
            stateofsensorDown = false;
        }
        if (temp3 == LOW) {
            //voltageLEFT = temp3 * (5.0 / 1023.0);
            stateofsensorLeft = true;
        }
        else {
            //voltageLEFT = 0;
            stateofsensorLeft = false;
        }
        if (temp4 == LOW) {
            //voltageRIGHT = temp4 * (5.0 / 1023.0);
            stateofsensorRight = true;
        }
        else {
            //voltageRIGHT = 0;
            stateofsensorRight = false;
    }
    

        /*if (voltageUP > 2.0) {
            stateofsensorUp = true;
        }
        else {
            stateofsensorUp = false;
        }
        if (voltageDOWN > 2.0) {
            stateofsensorDown = true;
        }
        else {
            stateofsensorDown = false;
        }
        if (voltageLEFT > 2.0) {
            stateofsensorLeft = true;
        }
        else {
            stateofsensorLeft = false;
        }
        if (voltageRIGHT > 2.0) {
            stateofsensorRight = true;
        }
        else {
            stateofsensorRight = false;
        }*/
    #endif

    #if (voltageDebug == true)
        #if (DigitalGearPins == false)
            #if (Mainserialmessages == true)
                Serial.print("\tup: ");Serial.println(voltageUP); 
                Serial.print("\t"); Serial.println(temp1);

                Serial.print("\t\t\tdown: ");Serial.println(voltageDOWN); 
                Serial.print("\t\t\t"); Serial.println(temp2);

                Serial.print("\t\t\t\t\t\tleft: ");Serial.println(voltageLEFT); 
                Serial.print("\t\t\t\t\t\t"); Serial.println(temp3);

                Serial.print("\t\t\t\t\t\t\t\t\t\tright: ");Serial.println(voltageRIGHT); 
                Serial.print("\t\t\t\t\t\t\t\t\t\t"); Serial.println(temp4);
                delay(1250); 
            #endif
        #endif

        #if (DigitalGearPins == true)
            #if (Mainserialmessages == true)
                Serial.print("\tup: ");Serial.println(voltageUP); 
                Serial.print("\t"); Serial.println(temp1);

                Serial.print("\t\t\tdown: ");Serial.println(voltageDOWN); 
                Serial.print("\t\t\t"); Serial.println(temp2);

                Serial.print("\t\t\t\t\t\tleft: ");Serial.println(voltageLEFT); 
                Serial.print("\t\t\t\t\t\t"); Serial.println(temp3);

                Serial.print("\t\t\t\t\t\t\t\t\t\tright: ");Serial.println(voltageRIGHT); 
                Serial.print("\t\t\t\t\t\t\t\t\t\t"); Serial.println(temp4);
                delay(1250); 
            #endif
        #endif
    #endif
}
void debugReadSwitchesnano()
{
    #if (GearDebuggingAutomatic == true)
        if (debugGearTimer >= 7) {
            debugGearTimer = 0;
        }

        delay(1500);
    #endif

    if (debugGearTimer == 0) //start with all off
    {
        stateofsensorUp = false;
        stateofsensorDown = false;
        stateofsensorLeft = false;
        stateofsensorRight = false;
    }
    else if (debugGearTimer == 1) //simulate first gear
    {
        stateofsensorUp = true;
        stateofsensorDown = false;
        stateofsensorLeft = true;
        stateofsensorRight = false;
    }
    else if (debugGearTimer == 2) {
        stateofsensorUp = false;
        stateofsensorDown = true;
        stateofsensorLeft = true;
        stateofsensorRight = false;
    }
    else if (debugGearTimer == 3) {
        stateofsensorUp = true;
        stateofsensorDown = false;
        stateofsensorLeft = false;
        stateofsensorRight = false;
    }
    else if (debugGearTimer == 4) {
        stateofsensorUp = false;
        stateofsensorDown = true;
        stateofsensorLeft = false;
        stateofsensorRight = false;
    }
    else if (debugGearTimer == 5) {
        stateofsensorUp = true;
        stateofsensorDown = false;
        stateofsensorLeft = false;
        stateofsensorRight = true;
    }
    else if (debugGearTimer == 6) {
        stateofsensorUp = false;
        stateofsensorDown = true;
        stateofsensorLeft = false;
        stateofsensorRight = true;
    }
    else if (debugGearTimer == -1) {// wtf is this doing ????
        stateofsensorUp = true;
        stateofsensorDown = true;
        stateofsensorLeft = true;
        stateofsensorRight = true;
    }

}
void debugSerialInputSwitchesnano()
{
    #if (GearDebuggingManual == true)
        if (Serial.available() > 0) {
            // read the incoming byte:
            //Serial.print("input ");
            SerialInput = Serial.readString();

            // say what you got:
            //Serial.print(" Received: ");
            //Serial.println(SerialInput);
        }
        if (SerialInput == "0")
        {
            Serial.println("nutural gear");
            SerialInput = "-1";
            debugGearTimer = 0;

        }
        if (SerialInput == gear1)
        {
            Serial.println("first gear");
            SerialInput = "-1";
            debugGearTimer = 1;
        }
        if (SerialInput == gear2)
        {
            Serial.println("second gear");
            SerialInput = "-1";
            debugGearTimer = 2;
        }
        if (SerialInput == gear3)
        {
            Serial.println("third gear");
            SerialInput = "-1";
            debugGearTimer = 3;
        }
        if (SerialInput == gear4)
        {
            Serial.println("forth gear");
            SerialInput = "-1";
            debugGearTimer = 4;
        }
        if (SerialInput == gear5)
        {
            Serial.println("fifth gear");
            SerialInput = "-1";
            debugGearTimer = 5;
        }
        if (SerialInput == gear6)
        {
            Serial.println("reverse gear");
            SerialInput = "-1";
            debugGearTimer = 6;
        }
        else
        {
            //need a print once thing
            //Serial.println("input error");

        }
    #endif
}

int getLastGearnano()
{
    if (loopCounter == 0) {
        *trueCurrentGear = writeOrRemove[1];
    }

    if (loopCounter == 1) {
        //last = first;
        strcpy(trueLastGear, trueCurrentGear);
    #if (RCAmode == false)
            //Serial.print("true current"); Serial.println(trueCurrentGear);
            //Serial.print("true last"); Serial.println(trueLastGear);
    #endif
    }

    loopCounter++;
    if (loopCounter == 2)
    {
        loopCounter = 0;
    }


    if (loopCounter == 1)
    {
        strcpy(lastGear, trueLastGear);
        return 1;
    }

    else{
        return 1;
    }
}
//tvout
#if (RCAmode == true )
void tvoutDemo() {

    TV.clear_screen();

    x = 0;
    y = 0;
    for (char i = 32; i < 127; i++) {              // this prints the alt code characters from 32 -127
        TV.print_char(x * 6, y * 8, i);                    // the res of the characters in pixel size ?
        x++;
        if (x >= TV.char_line()) {                   // i think this one is for if the character prints and gets to the end of the display make new line and continue
            y++;
            x = 0;
        }
    }

    TV.delay(500);
    TV.clear_screen();
    TV.println("Fill the Screen\nPixel by Pixel");
    TV.delay(500);
    TV.clear_screen();
    for (x = 0; x < TV.hres(); x++) {
        for (y = 0; y < TV.vres(); y++) {
            TV.set_pixel(x, y, 1); //this is the color 0= black 1 = white
        }
    }
    TV.delay(500);
    TV.clear_screen();


    TV.print("Draw some lines");
    TV.delay(500);
    x = TV.hres() - 1;
    for (y = 0; y < TV.vres(); y++) {
        TV.draw_line(0, y, x - y, y, 2);
    }
    TV.delay(500);
    TV.clear_screen();
    
}
void TvDisplayArray(char TVdisplayMessage[])
{//                                            array of characters  
    #if (RCAmode == true )
        delay(200);

        TV.println(TVdisplayMessage);
        TV.delay(3000);
        TV.clear_screen();

    #endif
}
void TVoutSetup() {
    #if (RCAmode == true )
        x = 0;
        y = 0;
        TV.begin(NTSC);  //for devices with only 1k sram(m168) use TV.begin(_NTSC,128,56)
        //TV.begin(PAL);  //for devices with only 1k sram(m168) use TV.begin(_NTSC,128,56)
        TV.select_font(font8x8ext);
        TV.delay(50);
        TV.clear_screen();

        TvDisplayArray(startingUpMsg);
        TV.delay(1000);
    #endif
}
#endif

void readVoltage()
{
    int x = 0;
    int y = 120;
    char voltstr[20] = "Volts: ";
    #if (SerialDebuggingReadVoltage == true )
        Serial.println("starting voltage reader");
    #endif
    int16_t adc0; // 16 bits ADC read of input A0
    adc0 = ads.readADC_SingleEnded(0);
    voltage = ((adc0 + calib) * 0.1875)/1000;

    //unsigned long currentMillis = millis();
    vbat = voltage/Radjust;

    //Prevent displaying negative voltage when battery is disconnected  
    if (vbat < 0.1)
    {
        vbat = 0.01;
    }
    #if (SerialDebuggingReadVoltage == true)
        Serial.print("Volts: ");
        Serial.println(vbat,2);
    #endif
    char vOut[6];
    dtostrf(vbat,4,2, vOut);
    
    writeTxtDisplayString(voltstr,x,y);
    writeTxtDisplayString(vOut,x+40,y);
    //removeTxtDisplayString(vOut,x+40,y);
    #if (SerialDebuggingReadVoltage == true)
        Serial.print("\t\t\t\t");
        Serial.println(voltstr);
    #endif

    //delay(1);
    #if (SerialDebuggingReadVoltage == true )
        Serial.println("finished voltage reader");
    #endif

}
void setupVoltageReader()
{
    char ADSerror[25] = "Failed to initialize ADS";
    #if (SerialDebuggingReadVoltage == true )
        Serial.println("setting up voltage reader");
    #endif
    ads.begin();
    if (!ads.begin()) {
        #if (Mainserialmessages == true)
            Serial.println("Failed to initialize ADS.");
        #endif
        writeTxtDisplayString(ADSerror,0,80);
    }
    // Setup 3V comparator on channel 0
    //ads.startComparator_SingleEnded(0, 1000);
    #if (SerialDebuggingReadVoltage == true )
        Serial.println("finished setting up voltage reader");
    #endif
}