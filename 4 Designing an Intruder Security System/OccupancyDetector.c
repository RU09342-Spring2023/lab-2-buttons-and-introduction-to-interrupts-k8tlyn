/*
 * OccupancyDetector.c
 *
 *  Created on: Jan 31, 2023
 *      Author: Kaitlyn Pounds
 */

#include <msp430.h>

int main() {

    WDTCTL = WDTPW | WDTHOLD;               // Stop watchdog timer

    PM5CTL0 &= ~LOCKLPM5;


       P1OUT &= ~BIT0;                         // Clear P1.0 output latch for a defined power-on state *red
       P1DIR |= BIT0;                          // Set P1.0 to output direction

       P6OUT &= ~BIT6;                          //same thing^^ but for the green one
       P6DIR |= BIT6;

        P2REN |= BIT3;                          // P2.3 pull-up register enable
        P2OUT |= BIT3;                          // Configure P2.3 as pulled-up
        P2DIR &= ~BIT3;                         //set P2.3 as input

       // P2IES &= ~BIT3;                         // P2.3 Low --> High edge      //which edge do you want the interrupt to go off at? rising edge
     //   P2IE |= BIT3;                           // P2.3 interrupt enabled

        P4REN |= BIT1;                          //enable register 4.1
        P4OUT |= BIT1;                          //Configure P4.1 as pulled up

      //  P4IES &= ~BIT1;
     //   P4IE |= BIT1;

        int timer = 0;
        int timer2 = 0;
        int state;
       const int armed = 0;
       const int warning = 1;
       const int alert = 2;


        while(1)
            {

            state = armed;

            switch(state){
               case armed:         //armed: BLINK GREEN LED (once every 3 seconds)
                     if(P2IN & BIT3){                //if button is not pushed
                         P1OUT &= ~BIT0;               //disable red LED
                         P6OUT ^= BIT6;                //toggle green LED
                         __delay_cycles(1000000);      // Delay for 1000000*(1/MCLK)=1s
                         P6OUT &= ~BIT6;               //green LED off
                         __delay_cycles(2500000);      // Delay for 2500000*(1/MCLK)=2.5s
                     }
                     else{
                         P6OUT &= ~BIT6;                //disable green LED
                         state = warning;               //move to warning state
                     }

               case warning:              //warning: BLINK RED LED (on for 500 ms and off for 500s)
                   while(!(P2IN & BIT3) && (timer < 10)){                        //if button is being pressed
                           P1OUT ^= BIT0;                //toggle red LED
                           __delay_cycles(500000);             // Delay for 500000*(1/MCLK)=.5s
                           P1OUT &= ~BIT0;               //red LED off
                           __delay_cycles(500000);             // Delay for 500000*(1/MCLK)=.5s
                           timer++;                      //for 10 second count down
                       }

                    if((P2IN & BIT3) && (timer < 10))
                       state = armed;                    //if button is released state will return to armed
                    else if (!(P2IN & BIT3) && (timer == 10))
                       state = alert;

               case alert:                    //alert: RED LED ON, IF BUTTON RELEASED IN FIRST 5 SECONDS STATE = ARMED
                   if (!(P2IN & BIT3)){
                       P1OUT ^= BIT0;                     //toggle red LED
                       while(timer2 < 5){                     //during first 5 seconds in alert state
                           __delay_cycles(1000000);          // Delay for 1000000*(1/MCLK)=1s
                           timer2++;
                       }
                   }
                   if((P2IN & BIT3) && (timer2 < 5)){      //if button is released and 15 seconds hasn't passed
                          state = armed;
                   }
                   else
                          state = alert;
                  if(!(P4IN & BIT1) && (timer2 == 5)){                    //if rst button is pushed
                      state = armed;
                  }
            }
        }
}
     /*   // Port 2 interrupt service routine
 #pragma vector=PORT2_VECTOR
     __interrupt void Port_2(void)
        {
            P2IFG &= ~BIT3;                         // Clear P1.3 IFG
            ToggleEnable ^= 0x01;                   // Enable if the toggle should be active
        }

}



 * The system when turned on needs to blink the Green LED once every 3 seconds to show it is armed.
 *
When the occupancy sensor detects someone, it will output a Logic 1, and your system needs to move into a "Warning" state,
where the Green LED stops blinking, and the Red LED Blinks once per second (500ms on, 500ms off).

If the occupancy detector still shows someone there after 10 seconds, your system should indicate this with the RED Led
constantly staying on, and move into the ALERT state.

If the occupancy detector before the 15 seconds goes back to a 0, indicating the room is now empty, then the system should
go back to the armed state.

When in the ALERT State, the only way for the system to go back to the Armed state is to press the P4.1 Button.
 *
 *
 * */
