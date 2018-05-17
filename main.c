#include <stdint.h>
#include <stdbool.h>

#include <msp430fr2433.h>

bool g_bCurrentLed = false;
bool g_bPeriodFlag = false;

int main( void )
{
  WDTCTL = WDTPW | WDTHOLD;  /** stop watchdog timer */
  PM5CTL0 &= ~LOCKLPM5;      /** for FRAM devices */

  P1DIR |= BIT0;
  P1DIR |= BIT1;

  P1OUT &= ~BIT0;
  P1OUT |= BIT1;

  /** Timer0  Setup
   *  TASSEL_2 -> use SMCLK;
   *  MC_1     -> continous to TACCR0;
   *  ID_3     -> prescaler = 8;
   *  TACLR    -> clean counter TAR; */
  TA0CTL = TASSEL_2 | MC_1 | ID_3 | TACLR;
  TA0CCR0 = 0x4000;
  TA0CCTL0 = CCIE;

  /** P2.3 - Interrupt rising edge (from button)*/
  P2DIR &= ~BIT3; /** set as input */
  P2OUT |= BIT3;
  P2REN |= BIT3;  /** pull-down is ON */
  P2IES &= ~BIT3; /** rising front */
  P2IFG &= ~BIT3; /** IFG cleared */
  P2IE  |= BIT3;  /** enable interrupt */

  /** P2.7 - Interrupt rising edge (from button) */
  P2DIR &= ~BIT7; /** set as input */
  P2OUT |= BIT7;
  P2REN |= BIT7;  /** pull-down is ON */
  P2IES &= ~BIT7; /** rising front */
  P2IFG &= ~BIT7; /** IFG cleared */
  P2IE  |= BIT7;  /** enable interrupt */

  __enable_interrupt();

  while (true) { }
}

#pragma vector = PORT2_VECTOR
__interrupt void Action(void)
{
  if (P2IFG & BIT3)
  {
    g_bCurrentLed = !g_bCurrentLed;
    P2IFG &= ~BIT3;
  }

  if (P2IFG & BIT7)
  {
    if (g_bPeriodFlag)
    {
      TA0CCR0 = 0x8000;
      g_bPeriodFlag = false;
    }
    else
    {
      TA0CCR0 = 0x2000;
      g_bPeriodFlag = true;
    }

    P2IFG &= ~BIT7;
  }
}

#pragma vector = TIMER0_A0_VECTOR
__interrupt void Timer0_Toggle_Led( void )
{
  if(g_bCurrentLed)
  {
    P1OUT ^= BIT0;
    P1OUT &= ~BIT1;
  }
  else
  {
    P1OUT &= ~BIT0;
    P1OUT ^= BIT1;
  }
}
