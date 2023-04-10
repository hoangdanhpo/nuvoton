void setup() {
  // put your setup code here, to run once:
   DDRB=B00000111;
}

void loop() {
  // put your main code here, to run repeatedly:
  /* Pin 8 - B0, 9 - B1, 10 - B2 */
  PORTB = B00000101; delay(10); // state 1
  PORTB = B00000001; delay(10); // state 2
  PORTB = B00000011; delay(10); // state 3
  PORTB = B00000010; delay(10); // state 4
  PORTB = B00000110; delay(10); // state 5
  PORTB = B00000100; delay(10); // state 6
}
