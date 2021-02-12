#ifndef garden_h
#define garden_h

#include "Adafruit_NeoPixel.h"
#include "Arduino.h"

//////////////////////////////////////////////////////////////
////////////////////////// COLOR ////////////////////////////
//////////////////////////////////////////////////////////////
// a color helper class with some things I'll find useful later
//////////////////////////////////////////////////////////////
class Color{
    public:
        Color(byte _r, byte _g, byte _b){
            r = _r;
            g = _g;
            b = _b;
        };
        Color(){
            r = 255;
            g = 255;
            b = 255;
        };

        byte r, g, b;

        uint32_t as24bit(){
            uint32_t c;
            c = r;
            c <<= 8;
            c |= g;
            c <<= 8;
            c |= b;
            return c;
        }; 

        Color& setRGB(byte _r, byte _g, byte _b){
            r = _r;
            g = _g;
            b = _b;
            return *this;
        };
        
        Color& setHSV(float h, float s, float v){
            float c = v * s;
            float x = c * (1 - abs(int(h/60.0)%2 -1));
            float m = v - c;

            float rp, gp, bp;

            if(h <= 0    && h < 60 ){rp = c; gp = x; bp = 0;}
            if(h <= 60   && h < 120){rp=  x; gp = c; bp = 0;}
            if(h <= 120  && h < 180){rp=  0; gp = c; bp = x;}
            if(h <= 180  && h < 240){rp=  0; gp = x; bp = c;}
            if(h <= 240  && h < 300){rp=  x; gp = 0; bp = c;}
            if(h <= 300  && h < 360){rp=  c; gp = 0; bp = x;}

            r = (rp + m)*255;
            g = (gp + m)*255;
            b = (bp + m)*255;

            return *this;
        };

        Color& vary(byte depth){
            r += random(0, depth);
            g += random(0, depth);
            b += random(0, depth);
            return *this;
        };

        Color& rand(byte min, byte max){
            r = random(min, max);
            g = random(min, max);
            b = random(min, max);

            return *this;
        };

        Color& attenuate(float intensity){

            // Serial.println("   ");
            // Serial.println(intensity);
            // Serial.println(r);
            // Serial.println(g * intensity);
            // Serial.println(b);
            // Serial.println(" ");
            
            int _r = round(r *intensity);
            int _g = round(g *intensity);
            int _b = round(b *intensity);

            r = byte(_r);
            g = byte(_g);
            b = byte(_b);

            // Serial.println("   ");
            // Serial.println("  a ");
            // Serial.println("   ");
            // Serial.println(intensity);
            // Serial.println(r);
            // Serial.println(g);
            // Serial.println(b);
            // Serial.println(" ");

            return *this;
        }

        Color& rand(byte minR, byte maxR,
                  byte minG, byte maxG, 
                  byte minB, byte maxB){
            r = random(minR, maxR);
            g = random(minG, maxG);
            b = random(minB, maxB);
            return *this;    
        };

        Color& wheel(byte wheelPos){
            if (wheelPos < 85) {
                return setRGB(wheelPos * 3, 255 - wheelPos * 3, 0);
            } else if (wheelPos < 170) {
                wheelPos -= 85;
                return setRGB(255 - wheelPos * 3, 0, wheelPos * 3);
            } else {
                wheelPos -= 170;
                return setRGB(0, wheelPos * 3, 255 - wheelPos * 3);
            }
            return *this;
        };

    private:
};

//////////////////////////////////////////////////////////////
//////////////////////////// Vec2 ////////////////////////////
//////////////////////////////////////////////////////////////
// Vec2 class with 2d operations (if i need them)
//////////////////////////////////////////////////////////////
class Vec2{
    public:
        Vec2(){
            x = 0;
            y = 0;
        };

        Vec2(int _x, int _y){
            x = _x;
            y = _y;
        };

        int x;
        int y;
    private:

};

//////////////////////////////////////////////////////////////
//////////////////////////// PIXEL ////////////////////////////
//////////////////////////////////////////////////////////////
// a single LED dot with knowledge of its position
// defaults to off
// however, everything else will be handled by the parent (vine)
//////////////////////////////////////////////////////////////
class Pixel{
    public:
        Pixel(Vec2 _pos, Color _color){
            pos = _pos;
            color = _color;
            on = false;
        };
        
        Vec2 pos;
        Color color;
        bool on;

    private:

};

//////////////////////////////////////////////////////////////
//////////////////////////// VINE ////////////////////////////
//////////////////////////////////////////////////////////////
// LED strip in a pseudo 2D array shape
//////////////////////////////////////////////////////////////
class PixelVine{
    public:
        PixelVine(int _numPixels, int _dataPin, int _clockPin){

        };
        PixelVine(){
            numPixels = 0;
            dataPin = 0;
            clockPin = 0;
        };

        int numPixels;
        int dataPin;
        int clockPin;

        int rows;
        int columns;
        int missing;
        int stride;

        Adafruit_NeoPixel strip;

        void setup(Adafruit_NeoPixel _strip){
            strip = _strip;
            numPixels = strip.numPixels();
        };

        void makeGrid(int _rows, int _columns, int _missing){
            rows = _rows;
            columns = _columns;
            stride = columns;
            missing = _missing;
        };

        void randomBlink(int wait){
            int index = random(0, strip.numPixels());
            uint32_t tmpColor = strip.getPixelColor(index);

            strip.setPixelColor(index, Color(0, 0, 0).as24bit());
            strip.show();
            delay(wait);
            strip.setPixelColor(index, tmpColor);
            strip.show();    
        };

        void setAllRandom(){
            for (int i = 0; i < strip.numPixels(); i++){
                strip.setPixelColor(i, Color().rand(0, 255).as24bit());
            }
            strip.show();
        };

        void randomScatter(int wait){
            for (int col = 0; col < columns; col++){
                for(int row = 0; row < rows; row++){
                    lightAt(Color().rand(0, 255), row + random(0, 1), col + random(0, 1));
                    delay(wait + random(-20, 20));
                }
            }
        };

        void colorWipe(Color color, int wait){
            int i;

            for(i = 0; i < strip.numPixels(); i++){
                strip.setPixelColor(i, color.as24bit());
                strip.show();
                delay(wait);
            }
        };

        void rainbow(int wait){
            int i, j;

            for (j = 0; j < 256; j++) {   // 3 cycles of all 256 colors in the wheel
                for (i = 0; i < strip.numPixels(); i++) {
                    Color c = Color();
                    c.wheel( (i + j) % 255);
                    strip.setPixelColor(i, c.as24bit());
                }
                strip.show();   // write all the pixels out
                delay(wait);
            }
        };

        void rainbowCycle(int wait){
            int i, j;

            for (j = 0; j < 256 * 5; j++) {   // 5 cycles of all 25 colors in the wheel
                for (i = 0; i < strip.numPixels(); i++) {
                    Color thisColor = Color();
                    thisColor.wheel(((i * 256 / strip.numPixels()) + j) % 256);
                    // tricky math! we use each pixel as a fraction of the full 96-color wheel
                    // (thats the i / strip.numPixels() part)
                    // Then add in j which makes the colors go around per pixel
                    // the % 96 is to make the wheel cycle around
                    strip.setPixelColor(i, thisColor.as24bit());
                }
                strip.show();   // write all the pixels out
                delay(wait);
            }
        };

        void setAllColor(Color color, int varyDepth){
            for (int i = 0; i < strip.numPixels(); i++){
                Color newColor = Color(color.r, color.g, color.b);
                strip.setPixelColor(i, newColor.vary(varyDepth).as24bit());
            }
            strip.show(); 
        };

        void setColorAt(Color color, int row, int column){
            int index = at(row, column);
            strip.setPixelColor(index, strip.Color(color.r, 
                                                    color.g, 
                                                    color.b));
        }

        void lightAt(Color color, int row, int column){
            int index = at(row, column);
            // Serial.println(color.r);
            // Serial.println(color.g);
            // Serial.println(color.b);
            // Serial.println(" ");
            strip.setPixelColor(index, strip.Color(color.r, 
                                                    color.g, 
                                                    color.b));
            strip.show();
        };

        void lightAt(Color color, int index){
            // Serial.println(color.r);
            // Serial.println(color.g);
            // Serial.println(color.b);
            // Serial.println(" ");
            strip.setPixelColor(index, strip.Color(color.r, 
                                                    color.g, 
                                                    color.b));
            strip.show();
        };

        int at(int row, int column){
            int index;
            // Serial.println(row);
            // Serial.println(column);
            index = column + row * stride;
            index = clip(index, numPixels);

            // Serial.println(numPixels);
            // Serial.println(index);

            return index;
        }; //returns the REAL index of the pixel

    private:
        int clip(int in, int hi){
            int out = in;
            if (hi < in){
                out = hi;
            }

            return out;
        };

};

#endif