/*Locomotion of Yom-Yom and Pom-Pom.
  Author: Maxim Zakirov 
  Date Created: A long time ago in a galaxy far far away...
  */
 #include <NewPing.h>

 #define ECHO 8
 #define TRIG 9

 #define TRIGGER_PIN  12
 #define ECHO_PIN     11
 #define MAX_DISTANCE 200

   int Motor1 = 5; //various variables for different things (couldn't be turned into a define statement)
   int Motor2 = 6;
   int DIRECTION_1 = 4;
   int DIRECTION_2 = 7;
   int distance_cm;
//   int uS;
   int led1 = 1;
   int led2 = 2;
   int Irsignal1 = A0;
   int Irsignal2 = A1;
   
 void setup() {
   NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE); //For the sonar sensor.
   pinMode(Motor1, OUTPUT); //Motor Pins setup for output.
   pinMode(Motor2, OUTPUT);
   pinMode(DIRECTION_1, OUTPUT);
   pinMode(DIRECTION_2, OUTPUT);
 
   Serial.begin(115200); //LEDs, Reflectance sensors setup here.
   pinMode(led1, OUTPUT); 
   pinMode(led2, OUTPUT);
   pinMode(Irsignal1, INPUT);
   pinMode(Irsignal2, INPUT);

//   attachInterrupt(A0, FORWARDS, FALLING); //Experimental interrupt system.
//   attachInterrupt(A1, FORWARDS, FALLING);
//   attachInterrupt(A0, STOP, RISING);
//   attachInterrupt(A1, STOP, RISING);
 }

 // FUNCTIONS : What the main loop calls for movement.
 
 void FORWARD() { 
   digitalWrite(DIRECTION_1, HIGH);
   digitalWrite(DIRECTION_2, HIGH);
   digitalWrite(Motor1, HIGH);
   digitalWrite(Motor2, HIGH);
 }
 
 void BACKWARD() {
   digitalWrite(Motor1, HIGH);
   digitalWrite(Motor2, HIGH);
   digitalWrite(DIRECTION_1, LOW);
   digitalWrite(DIRECTION_2, LOW);
 }

 void TURN_LEFT() {
   digitalWrite(Motor1, LOW);
   digitalWrite(Motor2, HIGH);
   digitalWrite(DIRECTION_1, LOW);
   digitalWrite(DIRECTION_2, HIGH);
 }

 void TURN_RIGHT() {
  digitalWrite(DIRECTION_1, HIGH);
  digitalWrite(DIRECTION_2, LOW);
  digitalWrite(Motor1, HIGH);
  digitalWrite(Motor2, LOW);
 }
 
 void STOP() {
   digitalWrite(Motor1, LOW);
   digitalWrite(Motor2, LOW);
   digitalWrite(DIRECTION_1, LOW);
   digitalWrite(DIRECTION_2, LOW);
 }

// MAIN LOGIC LOOP, checks for obstacles.
 void ALGORITHM() {
  
  analogRead(Irsignal1);
  analogRead(Irsignal2);
  delay(50);
  distance_cm = sonar.ping_cm();
//  distance_cm = uS / US_ROUNDTRIP_CM;
 
 if (distance_cm < 10 && distance_cm != 0) {
    digitalWrite(led1, HIGH);
 }else{
    digitalWrite(led1, LOW);
 }
 
 if (distance_cm < 30 && distance_cm >= 10 && distance_cm != 0) {
   digitalWrite(led2, HIGH);
 }else{
   digitalWrite(led2, LOW);
 }
  Serial.print("\t");
  Serial.print("Ping: ");
  Serial.print(distance_cm);
  Serial.println("cm");
  Serial.print("\t");
  Serial.print(analogRead(Irsignal1));
  Serial.print("\t");
  Serial.print(analogRead(Irsignal2));
 }

 //MAIN LOOP. With sample code.
 void loop() {
    
    FORWARD();
    delay(7500);
    STOP();
    TURN_RIGHT();
    delay(500);
    STOP();
    delay(7500);
    FORWARD();
    delay(7500);
    TURN_LEFT();
    delay(500);
    FORWARD();
    delay(7500);
    STOP(); 
    delay(10000);
    while(1) {}
 }
 
