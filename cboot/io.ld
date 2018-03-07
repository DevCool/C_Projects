SECTIONS
{
  . = 0x1000;

  .cseg :
  {
    *(.text);
  }

  .dseg :
  {
    *(.rodata);
  }

  /DISCARD/ :
  {
    *(.eh_frame);
  }
}
