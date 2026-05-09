/*
 * fpu.h
 * (C) 2026 Guo Fu
 */
#ifndef _FPU_H_
#define _FPU_H_

#include <datatype.h>

/*
 * 这里全部使用AT&T语法
 * 这里，我使用的基本都是寄存器间接寻址
 */

#pragma pack(push, 1)
typedef struct fpu_x87
{
    // 环境寄存器
    uint16_t control_word;
    uint16_t status_word;
    uint16_t tag_word;
    uint32_t fip;
    uint16_t fcs;
    uint32_t fdp;
    uint16_t fds;
    // 数据寄存器
    uint8_t st_0[10];
    uint8_t st_1[10];
    uint8_t st_2[10];
    uint8_t st_3[10];
    uint8_t st_4[10];
    uint8_t st_5[10];
    uint8_t st_6[10];
    uint8_t st_7[10];
} fpu_x87_t;
#pragma pack(pop)

void judge_fpu();
void fpu_init();
void _fnsave(virt_addr addr);
void _flds(virt_addr addr);
void _fldl(virt_addr addr);
void _fldt(virt_addr addr);
void _faddp();
void _fadd_sti_st0(int8_t st_i);
void _fadd_st0_sti(int8_t st_i);
void _fadds_mem(virt_addr addr);
void _faddl_mem(virt_addr addr);
void _fiadds_mem(virt_addr addr);
void _fiaddl_mem(virt_addr addr);
void _fsubp();
void _fsubrp();
void _fsub_sti_st0(int8_t st_i);
void _fsub_st0_sti(int8_t st_i);
void _fsubs_mem(virt_addr addr);
void _fsubrs_mem(virt_addr addr);
void _fsubrl_mem(virt_addr addr);
void _fsubl_mem(virt_addr addr);
void _fisubs_mem(virt_addr addr);
void _fisubrs_mem(virt_addr addr);
void _fisubl_mem(virt_addr addr);
void _fisubrl_mem(virt_addr addr);
void _fmulp();
void _fmul_sti_st0(int8_t st_i);
void _fmul_st0_sti(int8_t st_i);
void _fmuls_mem(virt_addr addr);
void _fmull_mem(virt_addr addr);
void _fimuls_mem(virt_addr addr);
void _fimull_mem(virt_addr addr);
void _fdivp();
void _fdiv_sti_st0(int8_t st_i);
void _fdiv_st0_sti(int8_t st_i);
void _fdivs_mem(virt_addr addr);
void _fdivrs_mem(virt_addr addr);
void _fdivl_mem(virt_addr addr);
void _fdivrl_mem(virt_addr addr);
void _fidivs_mem(virt_addr addr);
void _fidivrs_mem(virt_addr addr);
void _fidivl_mem(virt_addr addr);
void _fidivrl_mem(virt_addr addr);
void _fcom();
void _fcomp();
void _fcompp();
void _fcoms_mem(virt_addr addr);
void _fcoml_mem(virt_addr addr);
void _ficoms_mem(virt_addr addr);
void _ficoml_mem(virt_addr addr);
void _ftst();
void _fsts(virt_addr addr);
void _fstl(virt_addr addr);
void _fstps(virt_addr addr);
void _fstpl(virt_addr addr);
void _fstpt(virt_addr addr);
void _fld1();
void _fldz();
void _fldpi();
void _fld2e();
void _fldln2();
void _fsqrt();
void _fabs();
void _fchs();
void _fsin();
void _fcos();

#endif
