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
const String PHONE = "Enter_Your_Phone_Number";

#define RELAY_1 26
#define RELAY_2 27

String smsStatus,senderNumber,receivedDate,msg;
boolean isReply = false;

void setup() {
  pinMode(RELAY_1, OUTPUT); //Relay 1
  pinMode(RELAY_2, OUTPUT); //Relay 2

  smsStatus = ""; //READ, UNREAD
  senderNumber="";
  receivedDate="";
  msg="";
  
  // initialize serial port (Serial Monitor)
  Serial.begin(9600);

  //initialize serial port (SIM800L)
  Serial2.begin(9600);

  delay(7000);

  Serial2.println("AT");  //Test AT Command
  delay(500);
  Serial2.println("AT+CMGF=1"); //SMS Text Mode
  delay(500);

}

void loop() {
  //MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
  //read from SIM800L port, send to Serial Monitor
  while (Serial2.available()) {
    parseData(Serial2.readString());
  }
  //MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
  //MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
  //used for sending AT Commands to sim800l for degub
  while(Serial.available())  {
    Serial2.println(Serial.readString());
  }
  //MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM

}//main loop ends


//NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN
void parseData(String buff){
  Serial.println(buff);

  unsigned int len, index;
  //_____________________________________________________
  //Remove sent "AT Command" from the response string.
  index = buff.indexOf("\r");
  buff.remove(0, index+2);
  buff.trim();
  //_____________________________________________________
  
  //_____________________________________________________
  if(buff != "OK"){
    index = buff.indexOf(":");
    String cmd = buff.substring(0, index);
    cmd.trim();
    
    buff.remove(0, index+2);
    
    if(cmd == "+CMTI"){
      //get newly arrived memory location and store it in temp
      index = buff.indexOf(",");
      String temp = buff.substring(index+1, buff.length()); 
      temp = "AT+CMGR=" + temp + "\r"; 
      //get the message stored at memory location "temp"
      Serial2.println(temp); 
    }
    else if(cmd == "+CMGR"){
      extractSms(buff);
      Serial.println(smsStatus);
      Serial.println(senderNumber);
      Serial.println(receivedDate);
      Serial.println(msg);
      
      if(senderNumber == PHONE){
        doAction();
      }
    }
  }
  //_____________________________________________________
  else{
  //The result of AT Command is "OK"
  }
  //_____________________________________________________
}
//NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN

//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
void extractSms(String buff){
   unsigned int index;
   
    index = buff.indexOf(",");
    smsStatus = buff.substring(1, index-1); 
    buff.remove(0, index+2);
    
    senderNumber = buff.substring(0, 13);
    buff.remove(0,19);
   
    receivedDate = buff.substring(0, 20);
    buff.remove(0,buff.indexOf("\r"));
    buff.trim();
    
    index =buff.indexOf("\n\r");
    buff = buff.substring(0, index);
    buff.trim();
    msg = buff;
    buff = "";
    msg.toLowerCase();
}
//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM

//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
void doAction(){
  if(msg == "relay1 off"){  
    digitalWrite(RELAY_1, LOW);
    Reply("Relay 1 has been OFF");
  }
  else if(msg == "relay1 on"){
    digitalWrite(RELAY_1, HIGH);
    Reply("Relay 1 has been ON");
  }
  else if(msg == "relay2 off"){
    digitalWrite(RELAY_2, LOW);
    Reply("Relay 2 has been OFF");
  }
  else if(msg == "relay2 on"){
    digitalWrite(RELAY_2, HIGH);
    Reply("Relay 2 has been ON");
  }

  
  smsStatus = "";
  senderNumber="";
  receivedDate="";
  msg="";  
}
//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM

//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
void Reply(String text)
{
    Serial2.print("AT+CMGF=1\r");
    delay(1000);
    Serial2.print("AT+CMGS=\""+PHONE+"\"\r");
    delay(1000);
    Serial2.print(text);
    delay(100);
    Serial2.write(0x1A); //ascii code for ctrl-26 //Serial2.println((char)26); //ascii code for ctrl-26
    delay(1000);
    Serial.println("SMS Sent Successfully.");
}
//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
