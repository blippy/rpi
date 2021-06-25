/* Inspired from
https://smist08.wordpress.com/2021/04/16/assembly-language-on-the-raspberry-pi-pico/
*/

.syntax unified
.cpu cortex-m0plus
.thumb
/*
@ .syntax unified
 .fpu softvfp 
@ .thumb
*/


.section .boot_loader, "a"

.byte 0x00, 0xb5, 0x2f, 0x4b, 0x21, 0x20, 0x58, 0x60, 0x98, 0x68, 0x02, 0x21, 0x88, 0x43, 0x98, 0x60
.byte 0xd8, 0x60, 0x18, 0x61, 0x58, 0x61, 0x2b, 0x4b, 0x00, 0x21, 0x99, 0x60, 0x02, 0x21, 0x59, 0x61
.byte 0x01, 0x21, 0xf0, 0x22, 0x99, 0x50, 0x28, 0x49, 0x19, 0x60, 0x01, 0x21, 0x99, 0x60, 0x35, 0x20
.byte 0x00, 0xf0, 0x3e, 0xf8, 0x02, 0x22, 0x90, 0x42, 0x14, 0xd0, 0x06, 0x21, 0x19, 0x66, 0x00, 0xf0
.byte 0x2e, 0xf8, 0x19, 0x6e, 0x01, 0x21, 0x19, 0x66, 0x00, 0x20, 0x18, 0x66, 0x1a, 0x66, 0x00, 0xf0
.byte 0x26, 0xf8, 0x19, 0x6e, 0x19, 0x6e, 0x19, 0x6e, 0x05, 0x20, 0x00, 0xf0, 0x29, 0xf8, 0x01, 0x21
.byte 0x08, 0x42, 0xf9, 0xd1, 0x00, 0x21, 0x99, 0x60, 0x18, 0x49, 0x19, 0x60, 0x00, 0x21, 0x59, 0x60
.byte 0x17, 0x49, 0x18, 0x48, 0x01, 0x60, 0x01, 0x21, 0x99, 0x60, 0xeb, 0x21, 0x19, 0x66, 0xa0, 0x21
.byte 0x19, 0x66, 0x00, 0xf0, 0x0c, 0xf8, 0x00, 0x21, 0x99, 0x60, 0x13, 0x49, 0x11, 0x48, 0x01, 0x60
.byte 0x01, 0x21, 0x99, 0x60, 0x01, 0xbc, 0x00, 0x28, 0x00, 0xd1, 0x10, 0x48, 0x00, 0x47, 0x03, 0xb5
.byte 0x99, 0x6a, 0x04, 0x20, 0x01, 0x42, 0xfb, 0xd0, 0x01, 0x20, 0x01, 0x42, 0xf8, 0xd1, 0x03, 0xbd
.byte 0x02, 0xb5, 0x18, 0x66, 0x18, 0x66, 0xff, 0xf7, 0xf2, 0xff, 0x18, 0x6e, 0x18, 0x6e, 0x02, 0xbd
.byte 0x00, 0x00, 0x02, 0x40, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x07, 0x00, 0x00, 0x03, 0x5f, 0x00
.byte 0x21, 0x22, 0x00, 0x00, 0xf4, 0x00, 0x00, 0x18, 0x22, 0x20, 0x00, 0xa0, 0x01, 0x01, 0x00, 0x10
.byte 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
.byte 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3e, 0x27, 0x2a, 0x60



.section .vectors, "ax"
.align 2

.global __vectors
__vectors:
.word __StackTop
.word _reset_handler






.section .text
_reset_handler:
	bl main


/*
.thumb_func
.global main_asm
.align 4
main_asm:
BL main  
*/

.data
.align 4
