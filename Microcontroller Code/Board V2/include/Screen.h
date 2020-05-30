#ifndef Screen_H
#define Screen_H

#include "UserInterface.h"
#include "LiquidCrystal_PCF8574.h"
#include "ProgramUtilities.h"
#include "Sensors.h"
#include "JsonInterface.h"


class LCD: public JsonInterface
{
    public:
        static LCD *s_instance;

        static LCD* instance()
        {
            if(s_instance == __null)
            {
                s_instance = new LCD();
            }
            return s_instance;
        };

        int address;
        int rows;
        int columns;

        LiquidCrystal_PCF8574 *lcd;

        LCD()
        {
            address = 0x27;
            rows = 4;
            columns = 20;
        };
        LCD(int i_address, int i_rows, int i_columns)
        {
            address = i_address;
            rows = i_rows;
            columns = i_columns;
        };

        void begin()
        {
            lcd = new LiquidCrystal_PCF8574(address);
        };

        void startup()
        {
            lcd->begin(columns, rows);
            lcd->home();
            lcd->clear();
            lcd->noBlink();
            lcd->noCursor();
            lcd->setBacklight(255);
        };

        void toJson(JsonObject inputJsonObject)
        {
            JsonObject lcdJson = inputJsonObject.createNestedObject(name);
            lcdJson["Status"] = "Untested";
        };

        void fromJson(JsonObject inputJsonObject)
        {
          // TODO
        };
};

class Screen : public JsonInterface
{
    public:
        Screen()
        {
            name = "Screen";
        };

        char pattern0[20];
        char pattern1[20];
        char pattern2[20];
        char pattern3[20];

        bool drawRow0;
        bool drawRow1;
        bool drawRow2;
        bool drawRow3;

        char buff[12];

        
        //Userinterface UI;
        LCD lcd;

        void draw()
        {
            
            if(drawRow0)
            {
                lcd.lcd->setCursor(0, 0);
                lcd.lcd->print(pattern0);
            }
            if(drawRow1)
            {
                lcd.lcd->setCursor(0, 1);
                lcd.lcd->print(pattern1);
            }
            if(drawRow2)
            {
                lcd.lcd->setCursor(0, 2);
                lcd.lcd->print(pattern2);
            }
            if(drawRow3)
            {
                lcd.lcd->setCursor(0, 3);
                lcd.lcd->print(pattern3);
            }
        };

        void init()
        {
            drawRow0 = true;
            drawRow1 = true;
            drawRow2 = true;
            drawRow3 = true;
            
            //UI = *Userinterface::instance();
            lcd = *LCD::instance();
            
        };
    
    protected:
        void row0()
        {
            drawRow0 = true;
            sprintf(pattern0, " ");
        };
        void row1()
        {
            drawRow1 = true;
            sprintf(pattern1, " ");
        };
        void row2()
        {
            drawRow2 = true;
            sprintf(pattern2, " ");
        };
        void row3()
        {
            drawRow3 = true;
            sprintf(pattern3, " ");
        };
    
    public:
        void update()
        {
            row0();
            row1();
            row2();
            row3();
            draw();
        };
        void toJson(JsonObject inputJsonObject)
        {
            JsonObject screenJson = inputJsonObject.createNestedObject(name);
            screenJson["Status"] = "Untested";
            screenJson["pattern0"] = pattern0;
            screenJson["pattern1"] = pattern1;
            screenJson["pattern2"] = pattern2;
            screenJson["pattern3"] = pattern3;
        };

        void fromJson(JsonObject inputJsonObject)
        {
          // TODO
        };

};

class RunScreen: public Screen
{
    public:
        Sensors io = *Sensors::instance();
        
        void begin()
        {
            init();
        };
        
        void row0()
        {
            sprintf(buff, "%8.3f", io.force.force);
            sprintf(pattern0,  "%s: %s %s", "Force:", buff, "Kg");
        };

        void row1()
        {
            sprintf(buff, "%8.3f", io.distance.distance);
            sprintf(pattern1,  "%s: %s %s", "Dist:", buff, "mm");
        };
};

#endif