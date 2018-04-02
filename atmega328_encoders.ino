volatile word lastRead;

volatile int32_t counts[6] = {0};

const int QEM [16] = {0,-1,1,0,1,0,0,-1,-1,0,0,1,0,1,-1,0};

void setup() {
  // set all encoder pins to input
  for (int i = 0; i < 12; i++)
    pinMode(i, INPUT);

  // set up interrupts
  PCIFR |= (1 << PCIF0); // clear PCINT[7:0] flag
  PCIFR |= (1 << PCIF2); // clear PCINT[23:16] flag
  
  PCICR |= (1 << PCIE0); // enable PCINT[7:0]
  PCICR |= (1 << PCIE2); // enable PCINT[23:16]
  
  PCMSK0 = 0b00001111; // mask to enable PCINT on (D8-D11)
  PCMSK2 = 0b11111111; // enable PCINT on (D0-D7)

  sei(); // enable global interrupts to be safe
}

// pass both ISRs to updateCounts()
ISR(PCINT0_vect) { updateCounts(); }
ISR(PCINT2_vect) { updateCounts(); }

void loop() {

}

// shared ISR for PCINT0 and PCINT1
void updateCounts()
{
  // quickly read all pins
  word newRead = PIND | ( (PINB & 0x0F) << 8 );
  
  // mask out pins that have not changed
  word change = newRead ^ lastRead;

  for (int i = 0; i < 6; ++i)
  {
    // update count based on old and new readings
    counts[i] += QEM [ ( (lastRead >> (2 * i) ) & 0b11) * 4 + (newRead >> (2 * i) ) ];
  }

  // update lastReading
  lastRead = newRead;
}

