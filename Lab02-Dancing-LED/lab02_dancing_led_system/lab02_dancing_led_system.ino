// Lab 02: Dancing LED System
// Bicol University Polangui Campus - BSCpE
// Author: Gian Carlo D. Trilles


void setup(){
//Inititalize pins 2 through 9 as OUTPUT using a loop
   for (int pin = 2; pin <=9; pin++){
     pinMode(pin, OUTPUT);
}
}
void loop(){
//Pattern 1: The “Chase” (Left to Right)
    for(int pin = 2; pin<= 9; pin++){
      digitalWrite(pin,HIGH); // Turn LED on
      delay(100);
      digitalWrite(pin,LOW); // Turn LED off 
}
//Pattern 2: The “Return” (Right to Left)
    for(int pin = 9; pin >=2; pin--){
      digitalWrite(pin,HIGH);
      delay(100);
      digitalWrite(pin,LOW);
}
}
