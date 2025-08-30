/* Encoder Library - TwoKnobs Example
 * http://www.pjrc.com/teensy/td_libs_Encoder.html
 *
 * This example code is in the public domain.
 */

#include <Encoder.h>

// Change these pin numbers to the pins connected to your encoder.
//   Best Performance: both pins have interrupt capability
//   Good Performance: only the first pin has interrupt capability
//   Low Performance:  neither pin has interrupt capability
Encoder knobLeft(31,30); //(5, 6);
Encoder knobRight(36,37);//(7, 8);
//   avoid using pins with LEDs attached

void setup() {
  //Serial8.begin(9600);
  Serial8.begin(1000000);
  Serial8.println("TwoKnobs Encoder Test:");
}

long positionLeft  = -999;
long positionRight = -999;

void loop() {
  long newLeft, newRight;
  newLeft = knobLeft.read();
  newRight = knobRight.read();
  if (newLeft != positionLeft || newRight != positionRight) {
    Serial8.print("Left = ");
    Serial8.print(newLeft);
    Serial8.print(", Right = ");
    Serial8.print(newRight);
    Serial8.println();
    positionLeft = newLeft;
    positionRight = newRight;
  }
  // if a character is sent from the Serial8 monitor,
  // reset both back to zero.
  if (Serial8.available()) {
    Serial8.read();
    Serial8.println("Reset both knobs to zero");
    knobLeft.write(0);
    knobRight.write(0);
  }
}
