/*
DRIVER SAFTEY SYSTEM- IOT SOLUTION
Serial1 is for SIM 808
Serial2 IS FOR HM 10
*/
//////////////////////////////////////////For SIM 808 Module
char myBuffer[64];
int myCount = 0;
String nmea ;
String latitude;
double lat;
double lon;
String longitude;
int DD;
float SS;
double LocDec;
double Location(double);
void BufferData(void);
void locUpdate(void);
//////////////////////////////////////////For HM 10
char c = ' ';
String result = "";
String reply;
float Fvalue = 0.00;
byte pulse = 0;
float minThresh = 0.750;
void widgetRx(void);
void Red(void);
void Green(void);
void Blue(void);
///////////////////////////////////////parking
#define lm1 PE_2
#define lm2 PE_1
#define rm1 PE_3
#define rm2 PF_1
void setup() {
  Serial.begin(9600);
  Serial1.begin(9600);
  delay(500);
  pinMode(RED_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(BLUE_LED, OUTPUT);
loop1:
  Serial2.begin(9600);
  result = "";
  reply = "";
  Serial2.write("AT");
  delay(500);
  for (int i = 0; i < 2; i++)
  {
    if (Serial2.available())
      c = Serial2.read();
    result += c ;
  }
  reply = result;
  if (reply == "OK") {
    Serial.println("GADGET  working");
  }
  else {
    Serial.println("GADGET not working");
    Blue();
    goto loop1;
  }
  Serial1.write("AT");
  delay(1000);
  Serial1.println("AT+CGPSPWR=1");
  delay(2000);
  pinMode(lm1, OUTPUT);
  pinMode(lm2, OUTPUT);
  pinMode(rm1, OUTPUT);
  pinMode(rm2, OUTPUT);
  pinMode(PC_7,OUTPUT);
}
void loop()
{
  widgetRx();
  if (pulse != 1 and pulse != 2 ) {
    Serial.print("Danger. Pulse Is: Low ");
    Red();
    digitalWrite(PC_7,HIGH);
    left();
    delay(700);
    right();
    delay(1000);
    stopv();
    locUpdate();
  }
  else if (pulse == 2) {
    Serial.println("Live Zero");
    Blue();
  }
  else {
    Green();
  }
}
void widgetRx() {
  //Function that requests and recivers the Pulse Voltage form Wearable
  result = "";
  reply = "";
  Serial2.write("AT+ADCB?");
  delay(150);
  for (int i = 0; i < 12; i++)
  {
    if (Serial2.available()) {
      c = Serial2.read();
      if (i > 7 and i < 12) {
        /*7 and 10 indicates the position of
          voltage character in the AT Command response*/
        result += c ;
      }
    }
    Fvalue = result.toFloat();
  }
  reply = result;
  Serial.println("Fvalue is");
  Serial.println(Fvalue);
  if (Fvalue < minThresh and Fvalue>0.00) {
    pulse = 0;
  }
  else if (Fvalue == 0.00) {
    pulse = 2;
  }
  else {
    pulse = 1;
  }
}
void locUpdate() {
  Serial1.println("AT+CPIN?");
  delay(1000);
  Serial1.println("AT+CREG?");
  delay(1000);
  Serial1.println("AT+CGATT?");
  delay(1000);
  Serial1.println("AT+CIPSHUT");
  delay(1000);
  Serial1.println("AT+CIPSTATUS");
  delay(2000);
  Serial1.println("AT+CIPMUX=0");
  delay(2000);
  Serial1.println("AT+CSTT=\"airtelgprs.com\"");
  delay(1000);
  Serial1.println("AT+CIICR");
  delay(3000);
  Serial1.println("AT+CIFSR");
  delay(2000);
  Serial1.println("AT+CGPSSTATUS?");
  delay(2000);
  BufferData();
  Serial1.println("AT+CGPSINF=0");
  delay(4000);
  while ( myCount < 63 ) {
    myBuffer[myCount] = Serial1.read();
    myCount++;
  }
  myBuffer[myCount] = 0;
  char *output;
  output = strtok(myBuffer, ":");
  int field = 0;
  while (output != NULL) {
    field++;
    output = strtok(NULL, ",");
    switch (field)
    {
      case 2:
        latitude = output;
        lat = latitude.toFloat();
        lat =  Location(lat);
      case 3:
        longitude = output;
        lon = longitude.toFloat();
        lon =  Location(lon);
    }
  }
  Serial1.println("AT+CIPSPRT=0");
  delay(3000);
  Serial1.println("AT+CIPSTART=\"TCP\",\"api.thingspeak.com\",\"80\"");
  delay(6000);
  Serial1.println("AT+CIPSEND");
  BufferData();
  delay(4000);
  Serial1.print("GET https://api.thingspeak.com/update?api_key=GY66LRVTCAR5SV43&field3=");
  Serial1.print(lat);
  Serial1.print("&field4=");
  Serial1.println(lon);
  delay(4000);
  Serial1.println((char)26);
  delay(5000);
  Serial1.println();
  Serial1.println("AT+CIPSHUT");
  delay(100);
}
double Location(double loc) {
  /*NMEA LOCATION EXTRACTOR */
  DD = int(loc / 100);
  SS = float(loc) - DD * 100;
  LocDec = DD + SS / 60;
  return LocDec;
}
/*INDICATOR FUNCTIONS*/
void Red() {
  digitalWrite(RED_LED, HIGH);
  delay(50);
  digitalWrite(RED_LED, LOW);
  delay(10);
}
void Green() {
  digitalWrite(GREEN_LED, HIGH);
  delay(20);
  digitalWrite(GREEN_LED, LOW);
  delay(20);
}
void Blue() {
  digitalWrite(BLUE_LED, HIGH);
  delay(20);
  digitalWrite(BLUE_LED, LOW);
  delay(20);
}
void forward()
{
  digitalWrite(lm1, HIGH);
  digitalWrite(lm2, LOW);
  digitalWrite(rm1, HIGH);
  digitalWrite(rm2, LOW);
}
void right()
{
  digitalWrite(lm1, LOW);
  digitalWrite(lm2, LOW);
  digitalWrite(rm1, HIGH);
  digitalWrite(rm2, LOW);
}
void left()
{
  digitalWrite(lm1, HIGH);
  digitalWrite(lm2, LOW);
  digitalWrite(rm1, LOW);
  digitalWrite(rm2, HIGH);
}
void stopv()
{
  digitalWrite(lm1, LOW);
  digitalWrite(lm2, LOW);
  digitalWrite(rm1, LOW);
  digitalWrite(rm2, LOW);
}
void BufferData()
{
  while (Serial1.available() != 0) {
    Serial.write(Serial1.read());
  }
}
