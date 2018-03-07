#ifndef _16BITS_H_
#define _16BITS_H_

__asm__ (".code16gcc");
__asm__ ("jmpl $0x0000, $main");

#endif
