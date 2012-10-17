{
  Quick and dirty SPI bus sniffer, used to capture the traces you see here.
  Micah Elizabeth Scott - beth@scanlime.org
}

CON
  _clkmode = xtal1 + pll16x
  _xinfreq = 5_000_000

  BUFSIZE = 256

  SCLK  = $08
  SO    = $10
  SI    = $20
  CSN   = $40
  
OBJ
  serial : "FullDuplexSerial"

VAR
  long  buffer[BUFSIZE]
  byte  writePtr
  byte  readPtr

PUB main | w, r, value
  serial.start(31, 30, 0, 115200)
  cognew(@entry, @buffer)

  repeat
    w := writePtr
    r := readPtr
    if r <> w
      value := buffer[r]

      if value & $FF0000
        ' End of transaction
        serial.tx(13)
        serial.tx(10)
      else
        ' Normal byte
        serial.hex(value, 4)
        serial.tx(32)

      readPtr := r + 1

DAT
              org
entry
              mov       p_write, par
              add       p_write, c_wOff
              rdbyte    v_write, p_write

word_loop

              mov       v_out, #0               ' Fill buffer with all zeroes

              add       v_write, #1             ' Update write pointer
              mov       p_out, v_write          ' Calculate actual address of next word
              and       p_out, #$FF
              shl       p_out, #2
              add       p_out, par

bit_loop

              waitpeq   c_0, #(CSN | SCLK)      ' Make sure clock and CSN are low first
              waitpne   c_0, #(CSN | SCLK)      ' Wait for either CSN or CLK high
              mov       sample, ina             ' Sample all input pins

              and       sample, #CSN nr,wz      ' End of transaction?
  if_nz       neg       v_out, #1               '   Send (-1)
  if_nz       jmp       #commit

              shl       v_out, #1               ' Make room for another bit
              and       v_out, c_FEFEFEFE
              or        v_out, c_01000000       ' Add count bit

              and       sample, #SI nr,wz       ' Sample DI/DO
  if_nz       or        v_out, #$0100
              and       sample, #SO nr,wz
  if_nz       or        v_out, #$0001

              rcl       v_out, #1 nr,wc
  if_nc       jmp       #bit_loop
              
commit
              wrlong    v_out, p_out            ' Write and commit word
              wrbyte    v_write, p_write
              jmp       #word_loop

c_0           long      0
c_FEFEFEFE    long      $FEFEFEFE
c_01000000    long      $01000000
c_wOff        long      BUFSIZE * 4

p_write       res       1
v_write       res       1
p_out         res       1
v_out         res       1
sample        res       1         

              fit
