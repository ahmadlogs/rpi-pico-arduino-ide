/*
   Created 28 Apr 2021
   by Ahmad Logs
   Multiple Serial test For Raspberry Pi Pico

  MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
  Modification is Core Files
  MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
  1. go to %appdata%
  2. AppData\Local\Arduino15\packages\arduino\hardware\mbed_rp2040\2.0.0\variants\RASPBERRY_PI_PICO
  3. Open pins_arduino.h file in text editor
  4. Add the following code at the end of file

  ///////////////////////////////////////////////
  //Added by AhmadLogs
  #define SERIAL2_TX      (8u)
  #define SERIAL2_RX      (9u)

  #define SERIAL_PORT_HARDWARE1       Serial2
  #define SERIAL_PORT_HARDWARE_OPEN1  Serial2
  //////////////////////////////////////////////

  5. Find following line from pins_arduino.h
   #define SERIAL_HOWMANY    1
   and change it to
   #define SERIAL_HOWMANY    2

*/

//SIM800L Rx to PI PICO Pin8
//SIM800L Tx to PI PICO Pin9

//your phone number with country code
const String PHONE = "ENTER_YOUR_PHONE_NUMBER_HERE";

void setup() {
  // initialize serial port (Serial Monitor)
  Serial.begin(9600);

  //initialize serial port (SIM800L)
  Serial2.begin(9600);

  delay(7000);

  Serial2.println("AT");  //Test AT Command
  delay(500);
  Serial2.println("AT+CMGF=1"); //SMS Text Mode

}

void loop() {
  //MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
  //read from SIM800L port, send to Serial Monitor
  while (Serial2.available()) {
    String buff = Serial2.readString();
    Serial.println(buff);
  }
  //MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
  //read from Serial Monitor, send to SIM800L port
  while (Serial.available()) {
    String buff = Serial.readString();
    String isSendSms = buff.substring(0,5);
    isSendSms.toLowerCase();
    if(isSendSms == "send:")
      {send_sms(buff);}
    else
      {Serial2.println(buff);}
  }
  //MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
}

void send_sms(String text)
{
    //Remove first five letters from string
    text.remove(0,5);
    Serial2.print("AT+CMGF=1\r");
    delay(1000);
    Serial2.print("AT+CMGS=\""+PHONE+"\"\r");
    delay(1000);
    Serial2.print(text);
    delay(100);
    Serial2.write(0x1A); //ascii code for ctrl-26 //sim800.println((char)26); //ascii code for ctrl-26
    delay(1000);
    Serial.println("SMS Sent Successfully.");
}
