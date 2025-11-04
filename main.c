// ----------------------------------------------------------------------------
// PlayStation One Reset Mod
// ----------------------------------------------------------------------------
/*
 *   Arduino    PlayStation 1 (or One) Board
 *    5V/VCC <- Pad Port Pin 5 (3.5V)
 *       GND <- GND
 * (PD2) D2  <- Pad Port Pin 7 (CLK)
 * (PD3) D3  <- Pad Port Pin 6 (ATT/SS)
 * (PD4) D4  <- Pad Port Pin 1 (DAT)
 * (PD5) D5  -- x
 * (PD6) D6  -> RESET
 * (PD7) D7  -- x
 * (PB0) D8  -- x
 * (PB1) D9  -- x
 * (PB2) D10 -- x
 * (PB3) D11 -- x
 * (PB4) D12 -- x
 * (PC0) A0  -- x
 * (PC1) A1  -- x
 * (PC2) A2  -- x
 * (PC3) A3  -- x
 * (PC4) A4  -- x
 * (PC5) A5  -- x
 */
// ----------------------------------------------------------------------------
/*
 * SELECT + START + L1 + R1,
 * SELECT + START + L2 + R2 : Reset
 * SELECT + CROSS + L1 + R1,
 * SELECT + CROSS + L2 + R2 : Long Reset
 */
// ----------------------------------------------------------------------------
#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>
#include <string.h>
// ----------------------------------------------------------------------------
/* configuration */
//#define DEBUG

//#define LONG_RESET_ENABLED /* long reset function is enabled (for X-Station) */

#define USE_INTERRUPTS /* use interrupts instead of polling */

#if !defined(DEBUG)
#define REBOOT_DELAY 15
#else
#define REBOOT_DELAY 1
#endif
// ----------------------------------------------------------------------------
#define delay_us _delay_us
#define delay_ms _delay_ms
// ----------------------------------------------------------------------------
#define cbi(v, b) (v &= ~_BV(b))
#define sbi(v, b) (v |=  _BV(b))
// ----------------------------------------------------------------------------
#define LED_INIT sbi(DDRB,  DDB5)
#define LED_OFF  cbi(PORTB, DDB5)
#define LED_ON   sbi(PORTB, DDB5)
// ----------------------------------------------------------------------------
#define CLK (PIND & _BV(DDD2))
#define ATT (PIND & _BV(DDD3))
#define DAT_MASK _BV(DDD4)
// ----------------------------------------------------------------------------
#define max_dat_bytes 5
#define max_dat_nbit  (max_dat_bytes * 8)
// ----------------------------------------------------------------------------
// PlayStation controller buttons masks
#define BTN0_SELECT   (1 << 0)
#define BTN0_L3       (1 << 1)
#define BTN0_R3       (1 << 2)
#define BTN0_START    (1 << 3)
#define BTN0_UP       (1 << 4)
#define BTN0_RIGHT    (1 << 5)
#define BTN0_DOWN     (1 << 6)
#define BTN0_LEFT     (1 << 7)
#define BTN1_L2       (1 << 0)
#define BTN1_R2       (1 << 1)
#define BTN1_L1       (1 << 2)
#define BTN1_R1       (1 << 3)
#define BTN1_TRIANGLE (1 << 4)
#define BTN1_CIRCLE   (1 << 5)
#define BTN1_CROSS    (1 << 6)
#define BTN1_SQUARE   (1 << 7)
// ----------------------------------------------------------------------------
union dat_t
{
  uint8_t buf[max_dat_bytes];
  struct
  {
    uint8_t ff;
    uint8_t pid; // Peripheral ID (0x4x - Digital, 0x5x - Dual Analog, 0x6x - GunCon, 0x7x - Analogue)
    uint8_t sid; // Source ID (for controller = 0x5A)
    uint8_t btn0;
    uint8_t btn1;
  };
};
// ----------------------------------------------------------------------------
#if defined(USE_INTERRUPTS)
uint8_t bits[max_dat_nbit * 8];
volatile uint8_t nbits;
#else
uint8_t bits[max_dat_nbit];
uint8_t nbits;
#endif
// ----------------------------------------------------------------------------
void delay_s(uint8_t s)
{
  while (s--)
    delay_ms(1000);
}
// ----------------------------------------------------------------------------
void perform_short_reset(void)
{
  sbi(DDRD, DDD6);
  cbi(PORTD, DDD6);
  delay_ms(100);
  sbi(PORTD, DDD6);
  cbi(DDRD, DDD6);
}
// ----------------------------------------------------------------------------
void perform_long_reset(void)
{
  sbi(DDRD, DDD6);
  cbi(PORTD, DDD6);
  delay_s(2);
  sbi(PORTD, DDD6);
  cbi(DDRD, DDD6);
}
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
#if defined(USE_INTERRUPTS)
// ----------------------------------------------------------------------------
ISR(INT0_vect)
{
  bits[nbits++] = PIND;
}
// ----------------------------------------------------------------------------
ISR(INT1_vect)
{
  nbits = 0;
  /* enable interrupt for CLK */
  sbi(EIMSK, INT0);
}
// ----------------------------------------------------------------------------
#endif /* USE_INTERRUPTS */
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
#if defined(ARDUINO)
int main_routine(void)
#else
int main(void)
#endif
{
  PORTD = 0;
  DDRD  = 0;

  sbi(PORTD, DDD6);

  LED_INIT;
  LED_ON;
  delay_s(REBOOT_DELAY);
  LED_OFF;

  nbits = 0;

#if defined(USE_INTERRUPTS)
  /* interrupts: rising edge for CLK, falling edge for ATT/SS */
  EICRA |= (_BV(ISC00) | _BV(ISC01) | _BV(ISC11));
  /* enable interrupt for ATT/SS */
  sbi(EIMSK, INT1);
  /* enable interrupts */
  sei();
#else
  uint8_t clk0, clk1;
#endif

  uint8_t cnt_short = 0;
#if defined(LONG_RESET_ENABLED)
  uint8_t cnt_long  = 0;
#endif

  union dat_t dat;

  while (1)
  {
#if !defined(USE_INTERRUPTS)
    /* skipping the current data session */
    while (!ATT);
    // ----------------------------------------------------
    /* waiting for a new data session */
    while (ATT);
    // ----------------------------------------------------
    nbits = 0;
    clk0 = CLK;
    /* loop while ATT/SS is low */
    while (!ATT)
    {
      clk1 = CLK;
      if (!clk0 && clk1)
      {
        bits[nbits] = PIND;
        if (++nbits == max_dat_nbit)
          break;
      }
      clk0 = clk1;
    }
#endif
    // ----------------------------------------------------
    if (nbits < max_dat_nbit)
      continue;
    // ----------------------------------------------------
#if defined(USE_INTERRUPTS)
    /* disable interrupt for ATT/SS */
    cbi(EIMSK, INT1);
    /* disable interrupt for CLK */
    cbi(EIMSK, INT0);
#endif
    // ----------------------------------------------------
    memset(dat.buf, 0, max_dat_bytes);

    nbits = 0;

    for (uint8_t i = 0; i < max_dat_nbit; i++)
      if (bits[i] & DAT_MASK)
        dat.buf[i >> 3] |= _BV(i % 8);

    // ----------------------------------------------------
    if (dat.sid == 0x5A)
    {
      // 0x4x - Digital
      // 0x5x - Dual Analog
      // 0x6x - GunCon
      // 0x7x - Analogue
      if ( (dat.pid == 0x41) || (dat.pid == 0x73) || (dat.pid == 0x53) )
      {
        // btn0 bits: LEFT, DOWN, RIGHT, UP, START, R3, L3, SELECT
        // btn1 bits: SQUARE, CROSS, CIRCLE, TRIANGLE, R1, L1, R2, L2

        dat.btn0 = ~dat.btn0;
        dat.btn1 = ~dat.btn1;

        if ( (dat.btn0 == (BTN0_SELECT | BTN0_START)) &&
            ((dat.btn1 == (BTN1_L2 | BTN1_R2)) ||
             (dat.btn1 == (BTN1_L1 | BTN1_R1))) )
        {
          if (++cnt_short == 5)
          {
            LED_ON;
            perform_short_reset();
            delay_s(REBOOT_DELAY);
            LED_OFF;
            cnt_short = 0;
          }
#if defined(LONG_RESET_ENABLED)
          cnt_long = 0;
#endif
        }
#if defined(LONG_RESET_ENABLED)
        else if ( (dat.btn0 == BTN0_SELECT) &&
                 ((dat.btn1 == (BTN1_CROSS | BTN1_L2 | BTN1_R2)) ||
                  (dat.btn1 == (BTN1_CROSS | BTN1_L1 | BTN1_R1))) )
        {
          if (++cnt_long == 10)
          {
            LED_ON;
            perform_long_reset();
            delay_s(REBOOT_DELAY);
            LED_OFF;
            cnt_long = 0;
          }
          cnt_short = 0;
        }
#endif /* LONG_RESET_ENABLED */
        else
        {
          cnt_short = 0;
#if defined(LONG_RESET_ENABLED)
          cnt_long = 0;
#endif
        }

        /* sleep for a while */
        delay_ms(50);
      }
    }
    // ----------------------------------------------------
#if defined(USE_INTERRUPTS)
    /* enable interrupt for ATT/SS */
    sbi(EIMSK, INT1);
#endif
    // ----------------------------------------------------
  }

  return 0;
}
// ----------------------------------------------------------------------------
