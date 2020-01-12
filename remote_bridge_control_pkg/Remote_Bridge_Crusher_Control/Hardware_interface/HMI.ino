#define BLUE_OUTPUT 9
#define BLUE_INPUT 13
#define BLUE_ANALOG A2
#define YELLOW_OUTPUT 10
#define YELLOW_INPUT 14
#define YELLOW_ANALOG A1
#define WHITE_OUTPUT 11
#define WHITE_INPUT 15
#define WHITE_ANALOG A0


void setup() {
  Serial.begin(9600);
  
  // put your setup code here, to run once:
  pinMode(BLUE_INPUT, INPUT);
  pinMode(YELLOW_INPUT, INPUT);
  pinMode(WHITE_INPUT, INPUT);

  pinMode(BLUE_OUTPUT, OUTPUT);
  pinMode(YELLOW_OUTPUT, OUTPUT);
  pinMode(WHITE_OUTPUT, OUTPUT);  

  


}

void loop() {
  // put your main code here, to run repeatedly:

}
