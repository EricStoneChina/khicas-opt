/*
 * khicas_gb18030.cc - self-contained GB18030 charset switch for CASIOWIN.
 *
 * See khicas_gb18030.h for the rationale. Implemented as raw syscall thunks
 * (same pattern as libfxcg asm.h) with unique symbol names, so they never
 * collide with libfxcg's own _EnableGB18030/_DisableGB18030.
 */
#include "khicas_gb18030.h"

__asm__(
".text\n"
".align 2\n"
".global _khicas_enable_gb18030\n"
"_khicas_enable_gb18030:\n"
"  mov.l 1f, r2\n"
"  mov.l 2f, r0\n"
"  jmp @r2\n"
"  nop\n"
".align 2\n"
"1: .long 0x80020070\n"
"2: .long 0x01E0\n"
"\n"
".text\n"
".align 2\n"
".global _khicas_disable_gb18030\n"
"_khicas_disable_gb18030:\n"
"  mov.l 1f, r2\n"
"  mov.l 2f, r0\n"
"  jmp @r2\n"
"  nop\n"
".align 2\n"
"1: .long 0x80020070\n"
"2: .long 0x01DF\n"
"\n"
".text\n"
".align 2\n"
".global _khicas_process_print_chars\n"
"_khicas_process_print_chars:\n"
"  mov.l 1f, r2\n"
"  mov.l 2f, r0\n"
"  jmp @r2\n"
"  nop\n"
".align 2\n"
"1: .long 0x80020070\n"
"2: .long 0x1300\n"
);
