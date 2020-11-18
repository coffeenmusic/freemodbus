/*
 * FreeModbus Libary: MSP430 Port
 * Copyright (C) 2006 Christian Walter <wolti@sil.at>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * File: $Id$
 */

/* ----------------------- Platform includes --------------------------------*/
#include "port.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"


/* ----------------------- Defines ------------------------------------------*/
#define U0_CHAR                 ( 0x10 )        /* Data 0:7-bits / 1:8-bits */

#define DEBUG_PERFORMANCE       ( 1 )

#if DEBUG_PERFORMANCE == 1
#define DEBUG_PIN_RX            ( 0 )
#define DEBUG_PIN_TX            ( 6 )
#define DEBUG_PORT_DIR          ( P1DIR )
#define DEBUG_PORT_OUT          ( P1OUT )
#define DEBUG_INIT( )           \
  do \
  { \
    DEBUG_PORT_DIR |= ( 1 << DEBUG_PIN_RX ) | ( 1 << DEBUG_PIN_TX ); \
    DEBUG_PORT_OUT &= ~( ( 1 << DEBUG_PIN_RX ) | ( 1 << DEBUG_PIN_TX ) ); \
  } while( 0 ); 
#define DEBUG_TOGGLE_RX( ) DEBUG_PORT_OUT ^= ( 1 << DEBUG_PIN_RX )
#define DEBUG_TOGGLE_TX( ) DEBUG_PORT_OUT ^= ( 1 << DEBUG_PIN_TX )

#else

#define DEBUG_INIT( )
#define DEBUG_TOGGLE_RX( )
#define DEBUG_TOGGLE_TX( )
#endif

/* ----------------------- Static variables ---------------------------------*/
UCHAR           ucGIEWasEnabled = FALSE;
UCHAR           ucCriticalNesting = 0x00;

/* ----------------------- Start implementation -----------------------------*/
void
vMBPortSerialEnable( BOOL xRxEnable, BOOL xTxEnable )
{
    ENTER_CRITICAL_SECTION(  );
    if( xRxEnable )
    {
        IE2 |= UCA0RXIE;
    }
    else
    {
        IE2 &= ~UCA0RXIE;
    }
    if( xTxEnable )
    {
        IE2 |= UCA0TXIE;
        IFG2 |= UCA0TXIFG;
    }
    else
    {
        IE2 &= ~UCA0TXIE;
    }
    EXIT_CRITICAL_SECTION(  );
}

BOOL
xMBPortSerialInit( UCHAR ucPort, ULONG ulBaudRate, UCHAR ucDataBits, eMBParity eParity )
{
    BOOL            bInitialized = TRUE;
    USHORT          UCAxCTL0 = 0;
    float           NN = (float)SMCLK/ulBaudRate;

    switch ( eParity )
    {
    case MB_PAR_NONE:
        break;
    case MB_PAR_ODD:
        UCAxCTL0 |= UCPEN;
        break;
    case MB_PAR_EVEN:
        UCAxCTL0 |= UCPEN | UCPAR;
        break;
    }
    switch ( ucDataBits )
    {
    case 8:
        break;
    case 7:
        UCAxCTL0 |= UC7BIT;
        break;
    default:
        bInitialized = FALSE;
    }
    if( bInitialized )
    {
        ENTER_CRITICAL_SECTION(  );
        /* Reset USCI */
        UCA0CTL1 |= UCSWRST;
        /* Initialize all UART registers */
        UCA0CTL0 = UCAxCTL0;
        /* SSELx = 11 = SMCLK. Use only if PLL is synchronized ! */
        UCA0CTL1 |= UCSSEL1;
        UCA0CTL1 |= UCRXEIE;

        /* Configure USCI Baudrate Registers. */
        if(NN > 16){
            UCA0MCTL |= UCOS16; // Enable Oversampling. Needed when N >= 16. N=BRCLK/Baud
            float Nd16 = NN/16;
            UCA0BR0 = ( (int)Nd16 & 0xFF );
            UCA0BR1 = ( (int)Nd16 >> 8 )*256;
            USHORT UCBRFx = (USHORT) ((float)(Nd16 - (int)Nd16)*16);
            UCA0MCTL |= UCBRFx << 4 & 0b11110000;
        }
        else
        {
            UCA0BR0 = ( (int)NN & 0xFF );
            UCA0BR1 = ( (int)NN >> 8 )*256;
            USHORT UCBRSx = (USHORT) ((float)(NN - (int)NN)*8);
            UCA0MCTL |= UCBRSx << 1 & 0b00001110;
        }
//        UCA0MCTL |= UCOS16;                     // Enable Oversampling. Needed when N >= 16. N=BRCLK/Baud
//        UCA0BR0 = 26;                            // Prescaler
//        UCA0BR1 = 0;
//        UCA0MCTL |= UCBRF0;    // First Stage Modulator bits [7-4] = 01

        /* Clear reset flag. */
        UCA0CTL1 &= ~UCSWRST;

        /* USCI UART TXD/RXD */
        P1SEL |= BIT2 + BIT1;   // Assign UART pins to USCI_A0
        P1SEL2|= BIT2 + BIT1;   // Assign UART pins to USCI_A0

        EXIT_CRITICAL_SECTION(  );

        DEBUG_INIT( );
    }
    return bInitialized;
}

BOOL
xMBPortSerialPutByte( CHAR ucByte )
{
    UCA0TXBUF = ucByte;
    return TRUE;
}

BOOL
xMBPortSerialGetByte( CHAR * pucByte )
{
    *pucByte = UCA0RXBUF;
    return TRUE;
}

#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=USCIAB0RX_VECTOR
__interrupt void prvvMBSerialRXIRQHandler(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(USCIAB0RX_VECTOR))) prvvMBSerialRXIRQHandler (void)
#else
#error Compiler not supported!
#endif
{
    DEBUG_TOGGLE_RX( );
    pxMBFrameCBByteReceived(  );
}

#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=USCIAB0TX_VECTOR
__interrupt void prvvMBSerialTXIRQHandler(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(USCIAB0TX_VECTOR))) prvvMBSerialTXIRQHandler (void)
#else
#error Compiler not supported!
#endif
{
    DEBUG_TOGGLE_TX( );
    pxMBFrameCBTransmitterEmpty(  );
}

void
EnterCriticalSection( void )
{
    USHORT usOldSR;
    if( ucCriticalNesting == 0 )
    {
#if defined (__GNUC__)
        usOldSR = READ_SR;
        _DINT( );
#else
        usOldSR = __get_SR_register();
        _DINT( );
#endif
        ucGIEWasEnabled = usOldSR & GIE ? TRUE : FALSE;
    }
    ucCriticalNesting++;
}

void
ExitCriticalSection( void )
{
    ucCriticalNesting--;
    if( ucCriticalNesting == 0 )
    {
        if( ucGIEWasEnabled )
        {
            _EINT(  );
        }
    }
}
