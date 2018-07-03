// Including Library for 1.8 TFT DispLay
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>

//// TFT Display Pin For Arduino
#define TFT_CS  10
#define TFT_RST 17
#define TFT_DC  2

// Creating Object of TFT Display
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

// Color Code For Tft Display
#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

// Variable For TFT Display
int rect1x = 3;
int rect1y = 5;
int rect2y = 75;
int rect3y = 37;
int rect4y = 93;
int recwidth = 122;
int recheight = 20;

//Buttons:
const int start = 3;
const int compare = 12;

//TSC3200 Sensor Pinmap
const int S0 = 4;
const int S1 = 5;
const int S2 = 6;
const int S3 = 7;
const int Out = 8;

// Variables
float red = 0.00;
float green = 0.00;
float blue = 0.00;

float pRED = 0.00;
float pGREEN = 0.00;
float pBLUE = 0.00;

String color;

//Debouncing :
long debouncing_time = 500;
long compare_debouncing_time = 700;
volatile unsigned long last_micros;
volatile unsigned long compare_last_micros;

//DEBUG:
// #define TEST

void setup()
{
	Serial.begin(9600);
	//Pin Config:
	Serial.println("Configuring Pin...");
	PinConfiguration();
	//CONFIGURE DISPLAY:
	Serial.println("Setting up display....");
	DisplaySetup();
	Serial.println("Finished setup!");
}

void loop() {	
	#ifdef TEST
	ColorConditionCheck();
	IntensityDebug();
	Serial.println();
	delay(300);
	#endif

	if(digitalRead(start) == LOW) {
  	if( (long)(micros() - last_micros) >= debouncing_time * 1000 ) {
  		Serial.println("Button Pressed");

  		tft.fillRect(rect1x+1, rect2y+1, recwidth -2, recheight -2, CYAN); //RESULT

  		ColorConditionCheck();
  		DetectColor();


  		tft.fillRect(rect1x , rect3y + 1 , recwidth - 2, recheight + 1, RED); //STATUS
  		tft.setCursor(30, 45);
	    tft.setTextSize(0.5);
	    tft.setTextColor(BLACK);
	    tft.println("SCANNING......");

	    tft.setCursor(33, 76+5);
			tft.setTextSize(0.5);
			tft.setTextColor(BLACK);
			tft.println("PROCESSING..");

	    delay(500);

	    tft.fillRect(rect1x , rect3y + 1 , recwidth - 2, recheight + 1, GREEN); //STATUS
  		tft.setCursor(10, 45);
	    tft.setTextSize(0.5);
	    tft.setTextColor(BLACK);
	    tft.println("Scanning Complete!");

	    delay(300);

  		tft.fillRect(rect1x+1, rect2y+1, recwidth -2, recheight -2, CYAN); //RESULT
  		tft.setCursor(6, 76+5);
			tft.setTextSize(0.3);
			tft.setTextColor(BLACK);
			tft.println("Color:" + color);

  		last_micros = micros();
  		} else {
				Serial.println("Wait");
			}

	  delay(300);
	  tft.fillRect(rect1x , rect3y + 1 , recwidth - 2, recheight + 1, CYAN); //STATUS
	  tft.setCursor(17, 45);
	  tft.setTextSize(0.6);
	  tft.setTextColor(BLACK);
	  tft.println("Waiting For Scan");
	}
	if(digitalRead(compare) == LOW) {
		if((long)(micros() - compare_last_micros) >= compare_debouncing_time * 1000) {
			Serial.println("Compare button pressed");

			tft.fillRect(rect1x , rect3y + 1 , recwidth - 2, recheight + 1, RED); //STATUS
  		tft.setCursor(22, 45);
	    tft.setTextSize(0.5);
	    tft.setTextColor(BLACK);
	    tft.println("COMPARING......");

	    delay(500);

	    tft.fillRect(rect1x , rect3y + 1 , recwidth - 2, recheight + 1, GREEN); //STATUS
  		tft.setCursor(8, 45);
	    tft.setTextSize(0.5);
	    tft.setTextColor(BLACK);
	    tft.println("Comparing Complete!");
			comparison();

			// delay(300);
			delay(300);
	    tft.fillRect(rect1x , rect3y + 1 , recwidth - 2, recheight + 1, CYAN); //STATUS
	    tft.setCursor(17, 45);
	    tft.setTextSize(0.6);
	    tft.setTextColor(BLACK);
	    tft.println("Waiting For Scan");

			compare_last_micros = micros();
		}
	}
}

void ColorConditionCheck() {
	digitalWrite(S2, LOW);
	digitalWrite(S3, LOW);

	//count OUT, pRed, RED
	red = pulseIn(Out, digitalRead(Out) == HIGH ? LOW : HIGH);
	digitalWrite(S3, HIGH);

	//count OUT, pBLUE, BLUE
	blue = pulseIn(Out, digitalRead(Out) == HIGH ? LOW : HIGH);
	digitalWrite(S2, HIGH);

	//count OUT, pGreen, GREEN
	green = pulseIn(Out, digitalRead(Out) == HIGH ? LOW : HIGH);
}

void DetectColor() {
	if (red < blue && red < green && red > 15FDS) {
		IntensityDebug();
	  Serial.println(" - (Red Color)");
	  color = "RED";
		} else if (blue < red && blue < green) {
			IntensityDebug();
		  Serial.println(" - (Blue Color)");
		  color = "BLUE";
		} else if (green < red && green < blue) {
			IntensityDebug();
		  Serial.println(" - (Green Color)");
		  color = "GREEN";
		} else if (red <= 11 && (red < blue && red < green)) {
			IntensityDebug();
		  Serial.println(" - (Yellow Color)");
		  color = "YELLOW";
		} else if ( (red >=12 && red <= 14)   && (red < blue && red < green) ) {
			IntensityDebug();
		  Serial.println(" - (Orange Color)");
		  color = "ORANGE";
		}
		else {
	  	Serial.println("");
		}
		delay(300);	
}

void IntensityDebug() {
	Serial.print("R Intensity:");
	Serial.print(red, DEC);
	Serial.print(" G Intensity: ");
	Serial.print(green, DEC);
	Serial.print(" B Intensity : ");
	Serial.print(blue, DEC);
}

void comparison() {
	float dRED = 0.00;
	float dGREEN = 0.00;
	float dBLUE = 0.00;

	dRED = (((red/255) * 100.00) - ((pRED/255)*100.00));
	dGREEN = (((green/255) * 100.00) - ((pGREEN/255) * 100.00));
	dBLUE = (((blue/255) * 100.00) - ((pBLUE/255) * 100.00));

	// Serial.print("Deviation value: RED ");
	// Serial.print(dRED);
	// Serial.print(" GREEN ");
	// Serial.print(dGREEN);
	// Serial.print(" BLUE ");
	// Serial.println(dBLUE);

	tft.fillRect(rect1x+1, rect4y+1, recwidth -2, recheight * 2, CYAN); //RESULT
	tft.setCursor(6, 94);
	tft.setTextSize(0.3);
	tft.setTextColor(BLACK);
	tft.println("Deviation Value:");
	tft.setCursor(6, 103+2);
	tft.setTextSize(0);
	tft.setTextColor(BLACK);
	tft.println("RED " + String(dRED)); 
	tft.setCursor(6, 112+ 3);
	tft.println("GREEN " + String(dGREEN)); 
	tft.setCursor(6, 121 + 4); 
	tft.println("BLUE " + String(dBLUE));

	pRED = red;
	pGREEN = green;
	pBLUE = blue;
}

void PinConfiguration() {

	pinMode(S0, OUTPUT);
	pinMode(S1, OUTPUT);
	pinMode(S2, OUTPUT);
	pinMode(S3, OUTPUT);
	pinMode(Out, INPUT);
	pinMode(start, INPUT);

	digitalWrite(S0, HIGH);
	digitalWrite(S1, HIGH);
	digitalWrite(start, HIGH);
	digitalWrite(compare, HIGH);
}

void DisplaySetup() {
	// Diaplay Initialization and Creating BLock
	tft.initR(INITR_BLACKTAB);
	tft.setRotation(4);
	tft.fillScreen(ST7735_CYAN);

	//TOP 
	tft.drawRect(rect1x - 1 , rect1y - 1, recwidth + 2, recheight, BLACK); //HEADER
	tft.drawRect(rect1x, rect1y, recwidth, recheight, BLACK); //HEADER

	tft.setTextSize(1);
	tft.setTextColor(BLACK);
	tft.setCursor(20, 10);
	tft.println("COLOR DETECTION");
	delay(1000);

	tft.setCursor(3, 27);
	tft.setTextSize(1);
	tft.setTextColor(BLACK);
	tft.println("STATUS");

	tft.drawRect(rect1x - 1 , rect3y, recwidth, recheight + 3, BLACK); //STATUS
	tft.setCursor(17, 45);
	tft.setTextSize(0.6);
	tft.setTextColor(BLACK);
	tft.println("Waiting For Scan");

	tft.drawRect(rect1x, rect2y , recwidth, recheight*3, BLACK); //RESULT
	tft.setCursor(3, 65);
	tft.setTextSize(1);
	tft.setTextColor(BLACK);
	tft.println("RESULT");
}