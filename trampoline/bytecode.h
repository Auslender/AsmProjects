#ifndef TRAMPOLINE_BYTECODE_H
#define TRAMPOLINE_BYTECODE_H

typedef const char* _asm;

_asm mov_rsi_rdi = "\x48\x89\xfe",
     mov_rdx_rsi = "\x48\x89\xf2",
     mov_rcx_rdx = "\x48\x89\xd1",
     mov_r8_rcx =  "\x49\x89\xc8",
     mov_r9_r8 =   "\x4d\x89\xc1";


_asm mov_rdi_imm = "\x48\xbf",
     mov_rax_imm = "\x48\xb8";


_asm jmp_rax = "\xff\xe0";

_asm mov_r11_val_rsp = "\x4c\x8b\x1c\x24";
_asm push_r9 =       "\x41\x51";
_asm mov_rax_rsp =   "\x48\x89\xe0";
_asm add_rax_imm =   "\x48\x05";
_asm add_rsp_imm =   "\x48\x81\xc4";

_asm cmp_rax_rsp =   "\x48\x39\xe0";
_asm je =            "\x74";

_asm mov_rdi_val_rsp = "\x48\x8b\x3c\x24";
_asm mov_val_sub_rsp_8_rdi = "\x48\x89\x7c\x24\xf8";

_asm jmp = "\xeb";

_asm mov_val_rsp_r11 = "\x4c\x89\x1c\x24";
_asm sub_rsp_imm = "\x48\x81\xec";


_asm call_rax = "\xff\xd0";

_asm pop_r9 = "\x41\x59";
_asm mov_r11_val_add_rsp_stack_size = "\x4c\x8b\x9c\x24";

_asm ret = "\xc3";




#endif //TRAMPOLINE_BYTECODE_H
