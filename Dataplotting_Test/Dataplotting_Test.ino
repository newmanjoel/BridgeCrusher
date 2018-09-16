const float pi = 3.14;
float count = 0.0;
char incomingByte;
float dx = 0.01;

#define LED 13
void setup() {
  pinMode(LED, OUTPUT);
  Serial.begin(115200);
  Serial.println("this is the setup");
}

void loop() {
  if ( count > pi){
    count = -pi;
  }
  if ( count < -pi){
    count = pi;
  }
  Serial.print("V:");
  Serial.println(cos(count));
  count += dx;
  if (Serial.available() > 0) {
      // read the incoming byte:
      incomingByte = Serial.read();
      Serial.println(incomingByte);
      if(incomingByte=='U'){
        analogWrite(LED,255);
        dx += 0.01;
      }
      if(incomingByte=='D'){
        analogWrite(LED,200);
        dx -= 0.01;
      }
      Serial.flush();
  }
                
  delay(100);
}
