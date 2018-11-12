

#define NUM_DISPLAYS 2

#define INTENSITYMIN 0

// MAX7219 registers
#define MAXREG_DECODEMODE 0x09
#define MAXREG_INTENSITY  0x0a
#define MAXREG_SCANLIMIT  0x0b
#define MAXREG_SHUTDOWN   0x0c
#define MAXREG_DISPTEST   0x0f

const uint32_t numeros[10] =
{
  0x001f111f,
  0x00081f00,
  0x0017151d,
  0x0015151f,
  0x001c041f,
  0x001d1517,
  0x001f1517,
  0x00101718,
  0x001f151f,
  0x001c141f
};

//  number font is 3x5 pixels, being:
// [01][02][03]
// [04][05][06]
// [07][08][09]
// [10][11][12]
// [13][14][15]

const uint8_t numbers3x5[10][15] = 
{
  {1,1,1, 1,0,1, 1,0,1, 1,0,1, 1,1,1}, // 0
  {0,1,0, 1,1,0, 0,1,0, 0,1,0, 0,1,0}, // 1
  {1,1,1, 0,0,1, 1,1,1, 1,0,0, 1,1,1}, // 2
  {1,1,1, 0,0,1, 1,1,1, 0,0,1, 1,1,1}, // 3
  {1,0,1, 1,0,1, 1,1,1, 0,0,1, 0,0,1}, // 4
  {1,1,1, 1,0,0, 1,1,1, 0,0,1, 1,1,1}, // 5
  {1,1,1, 1,0,0, 1,1,1, 1,0,1, 1,1,1}, // 6
  {1,1,1, 0,0,1, 0,1,0, 0,1,0, 0,1,0}, // 7
  {1,1,1, 1,0,1, 1,1,1, 1,0,1, 1,1,1}, // 8
  {1,1,1, 1,0,1, 1,1,1, 0,0,1, 0,0,1}, // 9
};



byte disp_buffer[NUM_DISPLAYS * 8][8]; // [x][y]
uint64_t lowlevel_buffer[NUM_DISPLAYS];


// function that sets the same register and value to all cascated displays
void setRegistry(byte reg, byte value)
{
  digitalWrite(DISPLAY_CS, LOW);

  for (int i = 0 ; i < NUM_DISPLAYS ; i++)
  {
    putByte(reg);   // specify register
    putByte(value); // send data
  }

  digitalWrite(DISPLAY_CS, HIGH);
}


void putByte(byte data)
{
  byte i = 8;
  byte mask;
  while (i > 0)
  {
    mask = 0x01 << (i - 1);        // get bitmask
    digitalWrite( DISPLAY_CLK, LOW);   // tick
    if (data & mask)               // choose bit
      digitalWrite(DISPLAY_DIN, HIGH); // send 1
    else
      digitalWrite(DISPLAY_DIN, LOW);  // send 0
    digitalWrite(DISPLAY_CLK, HIGH);   // tock
    --i;                           // move to lesser bit
  }
}


void write_buffer_to_display()
{
  write_display_buffer_to_lowlevel_buffer();
  write_lowlevel_buffer_to_display();
}

void write_display_buffer_to_lowlevel_buffer()
{
  // is here where we take care to convert the display buffer to the low level buffer, including positoning and rotation
  for (int line = 0 ; line < 8 ; line ++)
  {
    for (int col = 0 ; col < (NUM_DISPLAYS * 8) ; col++)
    {
      int display_number = col / 8;
      int display_col = col % 8;

      if (disp_buffer[col][line] == 0)
      {
        lowlevel_buffer[display_number] &= ~ ((uint64_t)(0x80 >> display_col) << (line * 8));
      }
      else
      {
        lowlevel_buffer[display_number] |= ((uint64_t)(0x80 >> display_col) << (line * 8));
      }
    }
  }
}

void write_lowlevel_buffer_to_display()
{
  for (int line_reg = 0 ; line_reg < 8 ; line_reg++)
  {
    digitalWrite(DISPLAY_CS, LOW);
    for (int disp = 0 ; disp < NUM_DISPLAYS ; disp++)
    {
      putByte((byte) line_reg + 1);   // specify register
      putByte((byte) (lowlevel_buffer[disp] >> 8 * line_reg)); // send data
    }
    digitalWrite(DISPLAY_CS, HIGH);
  }
}

void display_write_number_to_buffer(int posx, int posy, uint8_t number)
{
  unsigned int x,y;
  if (number>9) return;

  for (int i = 0 ; i<15 ; i++)
  {
    x = posx + i%3;
    y = posy + i/3;
    if (( x < (NUM_DISPLAYS*8)) && (y<8)) disp_buffer[x][y] = numbers3x5[number][i];
  }
}

void display_clear_buffer()
{
   for (int line = 0 ; line < 8 ; line ++)
  {
    for (int col = 0 ; col < (NUM_DISPLAYS * 8) ; col++)
    {
      disp_buffer[col][line] = 0;
    }
  }
}

void display_change_number(int posx, int posy, uint8_t oldnumber, uint8_t newnumber)
{
  for (int t=0 ; t<9 ; t++)
  {
    display_clear_buffer();
    display_write_number_to_buffer(posx,posy-t,oldnumber);
    display_write_number_to_buffer(posx,posy-t+8,newnumber);
    write_buffer_to_display();
    delay(50);
  }
  
}

void display_init()
{
  pinMode(DISPLAY_DIN, OUTPUT);
  pinMode(DISPLAY_CLK, OUTPUT);
  pinMode(DISPLAY_CS, OUTPUT);

  // initialization of the MAX7219
  setRegistry(MAXREG_SCANLIMIT, 0x07);
  setRegistry(MAXREG_DECODEMODE, 0x00);  // using an led matrix (not digits)
  setRegistry(MAXREG_SHUTDOWN, 0x01);    // not in shutdown mode
  setRegistry(MAXREG_DISPTEST, 0x00);    // no display test
  setRegistry(MAXREG_INTENSITY, 0x0f & INTENSITYMIN);

//  lowlevel_buffer[1] = 0xff017d45555d417f;
//  lowlevel_buffer[0] = 0xfe82baaaa2be80ff;

  write_lowlevel_buffer_to_display();
}
