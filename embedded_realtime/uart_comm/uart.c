/**
 * Lab 6 - Simple PC Application
 * 
 * Board UART Application <uart.c>
 * By: Yousef Yassin June 10, 2021
 **/

#include "msp.h"

/* GPIO pin macros */
#define RED_LED 	((uint8_t)BIT0)	// P1.0
#define GREEN_LED 	((uint8_t)BIT1) // P2.1, Using rgb as green (0,1,0) only
#define SWITCH1 	((uint8_t)BIT1)	// P1.1
#define SWITCH2 	((uint8_t)BIT4)	// P1.4

/* Output pin set/clear macros */
#define RED_ON 		P1OUT |= RED_LED
#define RED_OFF		P1OUT &= ~RED_LED
#define GREEN_ON	P2OUT |= GREEN_LED
#define GREEN_OFF	P2OUT &= ~GREEN_LED

/* Debounce delay */
#define DELAY 		(uint16_t)40000

/* State machine input enum */
typedef enum {
	NEXT_STATE,		// 0
	PREVIOUS_STATE,	// 1
	GET_STATE		// 2 (only used for client initialization)
} Input_t;

/* State machine state enum */
typedef enum {
	OFF_STATE,		// 0
	RED_STATE,		// 1
	GREEN_STATE,	// 2
	BOTH_STATE		// 3
} State_t;

/* Forward function protoype declarations */
void setup(void);
void state_controller(Input_t input);
void UART0_init(void);
void UART0_Rx_IE(void);
void UART0_putchar(char c);

/**
 * UART0 Receiver ISR. 
 * @brief ISR triggered when the RX buffer is no longer
 * empty. Update the state machine based on its
 * contents.
 **/
void EUSCIA0_IRQHandler(void) {
	// Cast the buffer character to a input type for clarity
	// Input error handling happens client side
	volatile const Input_t input_encoding = (Input_t)(EUSCI_A0->RXBUF - '0'); // Char -> Int mapping is offset by '0'
	EUSCI_A0->IFG &= ~(uint16_t)(BIT0);	// Clear the interrupt flag
	state_controller(input_encoding);	// Update the state machine
	}

/**
 * PORT1 ISR. 
 * @brief ISR triggered when either switch 1
 * or 2 is pressed. Switch 1 updates the state machine
 * to the next state, switch 2 to the previous one.
 **/
void PORT1_IRQHandler(void) {
	volatile uint16_t i = DELAY;
	// Debounce
	while (i--) {}

	// Check which switch was pressed
	if (P1IFG & SWITCH1) {
		P1IFG &= ~SWITCH1;					// Clear the interrupt flag
		if (P1IN & SWITCH1) return;			// Debounce failed, false press
		state_controller(NEXT_STATE);		// If pressed, update state
		return;
	}

	else if (P1IFG & SWITCH2) {
		P1IFG &= ~SWITCH2;					// Clear the interrupt flag
		if (P1IN & SWITCH2) return;			// Debounce failed, false press
		state_controller(PREVIOUS_STATE);	// If pressed, update state
		return;
	}
}

/* The main thread. */
int main(void) {
	setup(); // Setup ports/pins/interrupts.

	// Idle until interrupt
    while (1) {
		__WFI();
    }

	return 0;
}

/**
 * State machine controller
 * @brief Controller for the board state machine. 
 * Handles transition to next state based on 
 * specified input.
 * 
 * @param input Input_t, the state machine input.
 **/
void state_controller(const Input_t input) {
	// Store the current state
	static volatile State_t state = OFF_STATE;
	// Output state message char to client. Offset
	// by State_t int value to get correct encoding.
	static const volatile uint8_t RX_OUT_OFFSET = (uint8_t)'0';

	// The client application was just started,
	// send our current state.
	if (input == GET_STATE) {
		UART0_putchar(RX_OUT_OFFSET + state);
		return;
	}

	// Transition to next state
	switch(state) {
		case OFF_STATE:
			// Off -> Red
			if (input == NEXT_STATE) {
				state = RED_STATE;
				RED_ON;
			// Off -> Both
			} else if (input == PREVIOUS_STATE) {
				state = BOTH_STATE;
				RED_ON;
				GREEN_ON;
			}
			// These could all be elses, but else if for clarity.
			break;

		case RED_STATE:
			// Red -> Green
			if (input == NEXT_STATE) {
				state = GREEN_STATE;
				GREEN_ON;
				RED_OFF;
			// Red -> Off
			} else if (input == PREVIOUS_STATE) {
				state = OFF_STATE;
				RED_OFF;
			}
			break;

		case GREEN_STATE:
			// Green -> Both
			if (input == NEXT_STATE) {
				state = BOTH_STATE;
				RED_ON;
			// Green -> Red
			} else if (input == PREVIOUS_STATE) {
				state = RED_STATE;
				RED_ON;
				GREEN_OFF;
			}
			break;

		case BOTH_STATE:
			// Both -> Off
			if (input == NEXT_STATE) {
				state = OFF_STATE;
				RED_OFF;
				GREEN_OFF;
			// Both -> Green
			} else if (input == PREVIOUS_STATE) {
				state = GREEN_STATE;
				RED_OFF;
			}
			break;

		default:
			return;
	}

	// Communicate the state update to the client
	UART0_putchar(RX_OUT_OFFSET + state);
}

/* UART0 is connected to virtual COM port through the USB debug connection */

/**
 * UART0 Init
 * @brief Configures UART communication
 * from the board to a baudrate of 115200, 1 stop bit,
 * no parity and 8 bit data
 **/ 
void UART0_init(void) {
	// b0->1 Reset enable for config, b(76)->(01) to select SMCLK = 3 MHz 
    EUSCI_A0->CTLW0 |= (uint16_t)(BIT0 | BIT7);		
	// bB - one stop, bC - 8 bit data, bF - no parity
    EUSCI_A0->CTLW0 &= ~(uint16_t)(BITB | BITC | BITF);	
	EUSCI_A0->MCTLW &= ~(uint16_t)BIT0;     // Disable oversampling
	EUSCI_A0->BRW = (uint16_t)26;           // BRW = f / baud => (3M / 115200) = 26
    P1->SEL0 |= (uint16_t)(BIT2 | BIT3);    // P1.3, P1.2 (SEL0, SEL1)->(1,0) for UART
    P1->SEL1 &= ~(uint16_t)(BIT2 | BIT3);
    EUSCI_A0->CTLW0 &= ~(uint16_t)BIT0;     // Reset disable, release UART for operation.
}

/**
 * UART0 Rx Interrupt Init
 * @brief Configures interrupt
 * for UART receive event.
 **/ 
void UART0_Rx_IE(void) {
	// Clear UCRXIFG interrupt flag
	EUSCI_A0->IFG &= ~(uint16_t)(BIT0);
	// Enable UCRXIE interrupt request
	EUSCI_A0->IE |= (uint16_t)BIT0;

	// Configure NVIC for UART Rx interrupt
	NVIC_ClearPendingIRQ(EUSCIA0_IRQn);
	NVIC_SetPriority(EUSCIA0_IRQn, 2);
	NVIC_EnableIRQ(EUSCIA0_IRQn);
}

/**
 * UART0 Transmitter
 * @brief Transmits a character
 * (byte) through UART.
 * 
 * @param c char, the character to transmit.
 **/ 
void UART0_putchar(char c) {
	// Wait for UCTXCPTIFG for complete transmit
    while (!(EUSCI_A0->IFG&(uint16_t)BIT1)) {} 
    // Send char
	EUSCI_A0->TXBUF = c;             
}

/* Setup board hardware and interrupts */
void setup(void) {
	// Turn off the watchdog timer
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;

	// Configure GPIO
	P1SEL0 &= ~(RED_LED | SWITCH1 | SWITCH2);
	P1SEL1 &= ~(RED_LED | SWITCH1 | SWITCH2);
	P2SEL0 &= ~GREEN_LED;
	P2SEL1 &= ~GREEN_LED;

	// Configure inputs active-low, pull-up
	P1DIR &= ~(SWITCH1 | SWITCH2);
	P1REN |= (SWITCH1 | SWITCH2);
	P1OUT |= (SWITCH1 | SWITCH2);

	// Configure outputs active-high
	P1DIR |= RED_LED;
	P2DIR |= GREEN_LED;
	P1OUT &= ~RED_LED;
	P2OUT &= ~GREEN_LED;

	// Configure UART
	UART0_init();
	// Configure UART IRQ
	UART0_Rx_IE();

	// Configure switch interrupts: high to low
	P1IES |= (SWITCH1 | SWITCH2);
	P1IFG &= ~(SWITCH1 | SWITCH2);
	P1IE |= (SWITCH1 | SWITCH2);

	// Configure NVIC for switch interrupts
	NVIC_ClearPendingIRQ(PORT1_IRQn);
	NVIC_SetPriority(PORT1_IRQn, 2);
	NVIC_EnableIRQ(PORT1_IRQn);
	
	// Enable interrupts globally in cpu
	__asm("CPSIE I");
}
