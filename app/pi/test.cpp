char incomingByte;
boolean sw = false;
int LED_PIN = 12;

void setup(){
    Serial.begin(57600);
    pinMode(LED_PIN, OUTPUT);
    Serial.println("====ready====");
}

void loop(){

  if(Serial.available() > 0){

      incomingByte = Serial.read();
      incomingByte = (int)incomingByte;
      Serial.println("val from  pi  = ");
      Serial.println(incomingByte);

      if(incomingByte == '1'){
          sw = !sw;
          if(sw == true){
              digitalWrite(LED_PIN, HIGH);
          }else{
              digitalWrite(LED_PIN, LOW);
          }
      }else{
          Serial.println("not 1");
      }
    }else{
      //Serial.println("==== Serial not available====");
    }
}