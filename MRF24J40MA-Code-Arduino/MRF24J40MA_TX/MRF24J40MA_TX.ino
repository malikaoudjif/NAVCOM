/*************************************************************************************************

* Title:       Mrf24j40ShieldExample01

* Autors:       Ingg. Tommaso Giusto e Alessandro Giusto

* Date:         26/06/2014

* Email:        tommro@libero.it

*************************************************************************************************/

 

/*************************************************************************************************

* Library include

*************************************************************************************************/

#include <SPI.h>

#include <mrf24j.h>

 

/*************************************************************************************************

* Definition pin I/O Arduino

*************************************************************************************************/

// Pin reset radio module

const int pinResetMRF = 6;

// Pin chip select radio module

const int pinCSMRF = 10;

// Pin interrupt radio module

const int pinIntMRF = 2;

// Pin relays MRF shield

const int pinShieldRele = 8;

// Pin input MRF shield

const int pinShieldInput = 9;

 

// PAN ID wireless

const word PANWireless = 0x1235;

 

// this node address

const word thisNodeAddress = 0x6001;

//const word thisNodeAddress = 0x6002;

 

// destination node wireless

const word destNodeAddress = 0x6002;

//const word destNodeAddress = 0x6001;

 

/*************************************************************************************************

* Variables

*************************************************************************************************/

 

// object Mrf24j shield

Mrf24j mrfShield(pinResetMRF, pinCSMRF, pinIntMRF);

 

// last state of input

int lastStatoInput;

 

/*************************************************************************************************

* source code

*************************************************************************************************/

 

// board init

void setup() {

// init  I/O

pinMode(pinShieldRele, OUTPUT);

pinMode(pinShieldInput, INPUT);

// relay disable

digitalWrite(pinShieldRele, LOW);

// read current input state

lastStatoInput = LOW;

 

// init serial port

Serial.begin(9600);

 

// Reset Mrf24j shield

mrfShield.reset();

// init Mrf24j shield

mrfShield.init();

 

// set PAN ID

mrfShield.set_pan(PANWireless);

// set sender node address

mrfShield.address16_write(thisNodeAddress);

 

// uncomment if you want to receive any packet on this channel

//mrfShield.set_promiscuous(true);

 

// uncomment if you want to enable PA/LNA external control

//mrfShield.set_palna(true);

 

// uncomment if you want to buffer all PHY Payload

//mrfShield.set_bufferPHY(true);

 

// attach interrupt handler function when i receive a “change” interrupt

// 0 -> matches Arduino pin 2 (INT0 for Arduino ATmega8/168/328)

attachInterrupt(0, MRFInterruptRoutine, CHANGE);

interrupts();

}

 

// function to manage MRF interrupt

void MRFInterruptRoutine() {

//sends control to interrupt routine on object Mrf24j shield

mrfShield.interrupt_handler();

}

 

// main program

void loop() {

int statoInput;

unsigned long currentMillis;

char serialBuffer[105];

int index;

int flag;

 

// call the handler Rx/Tx

mrfShield.check_flags(&handleRx, &handleTx);

 

// read input state

statoInput = digitalRead(pinShieldInput);

 

// if input state is changed:

if (statoInput != lastStatoInput) {

// sends the state (communication protocol)

if (statoInput == HIGH)

mrfShield.send16(destNodeAddress, "INPUT0");

else

mrfShield.send16(destNodeAddress, "INPUT1");

 

// update last input state

lastStatoInput = statoInput;

}

 

// if i received data on serial input

if (Serial.available() > 0) {

// Init command buffer

serialBuffer[0] = 'S';

serialBuffer[1] = 'E';

serialBuffer[2] = 'R';

serialBuffer[3] = 'I';

serialBuffer[4] = 'A';

index = 5;

// store the received chars

for (;;) {

// if new char available

if (Serial.available() > 0) {

// queue the char

serialBuffer[index] = (char) (Serial.read());

index++;

// flag the received char

flag = 0;

}

// if new char not available

else {

// if i didn’t receive a new char

if (flag == 1)

// end cycle to store received chars

break;

 

// set “char not received” flag

flag = 1;

// wait to receive next char

delay(5);

}

}

// add end of string

serialBuffer[index] = (char) ('\0');

// send command

mrfShield.send16(destNodeAddress, serialBuffer);

}

}

 

// Handler executed when radio receives data

void handleRx() {

int i;

 

// if the command is valid

if (mrfShield.rx_datalength() > 5) {

// if received an INPUT command

if ((mrfShield.get_rxinfo()->rx_data[0] == 'I') &&

(mrfShield.get_rxinfo()->rx_data[1] == 'N') &&

(mrfShield.get_rxinfo()->rx_data[2] == 'P') &&

(mrfShield.get_rxinfo()->rx_data[3] == 'U') &&

(mrfShield.get_rxinfo()->rx_data[4] == 'T')) {

// set relay

if (mrfShield.get_rxinfo()->rx_data[5] == '0')

digitalWrite(pinShieldRele, LOW);

else

digitalWrite(pinShieldRele, HIGH);

}

// if received command SERIA

else if ((mrfShield.get_rxinfo()->rx_data[0] == 'S') &&

(mrfShield.get_rxinfo()->rx_data[1] == 'E') &&

(mrfShield.get_rxinfo()->rx_data[2] == 'R') &&

(mrfShield.get_rxinfo()->rx_data[3] == 'I') &&

(mrfShield.get_rxinfo()->rx_data[4] == 'A')) {

// send data on the serial interface

for (int i = 5; i < mrfShield.rx_datalength(); i++)

Serial.write(mrfShield.get_rxinfo()->rx_data[i]);

}

}

}

// Handler to transmit data on radio

void handleTx() {

}
