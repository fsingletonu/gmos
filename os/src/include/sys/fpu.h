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

typedef struct fpu_st
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
} _packed fpu_st_t;
fpu_st_t fpu_sys_st;

void judge_fpu();
void fpu_init();
void _fnsave(phys_addr_t addr);
void _flds(phys_addr_t addr);
void _fldl(phys_addr_t addr);
void _fldt(phys_addr_t addr);
void _faddp();
void _fadd_sti_st0(int8_t st_i);
void _fadd_st0_sti(int8_t st_i);
void _fadds_mem(phys_addr_t addr);
void _faddl_mem(phys_addr_t addr);
void _fiadds_mem(phys_addr_t addr);
void _fiaddl_mem(phys_addr_t addr);
void _fsubp();
void _fsubrp();
void _fsub_sti_st0(int8_t st_i);
void _fsub_st0_sti(int8_t st_i);
void _fsubs_mem(phys_addr_t addr);
void _fsubrs_mem(phys_addr_t addr);
void _fsubrl_mem(phys_addr_t addr);
void _fsubl_mem(phys_addr_t addr);
void _fisubs_mem(phys_addr_t addr);
void _fisubrs_mem(phys_addr_t addr);
void _fisubl_mem(phys_addr_t addr);
void _fisubrl_mem(phys_addr_t addr);
void _fmulp();
void _fmul_sti_st0(int8_t st_i);
void _fmul_st0_sti(int8_t st_i);
void _fmuls_mem(phys_addr_t addr);
void _fmull_mem(phys_addr_t addr);
void _fimuls_mem(phys_addr_t addr);
void _fimull_mem(phys_addr_t addr);
void _fdivp();
void _fdiv_sti_st0(int8_t st_i);
void _fdiv_st0_sti(int8_t st_i);
void _fdivs_mem(phys_addr_t addr);
void _fdivrs_mem(phys_addr_t addr);
void _fdivl_mem(phys_addr_t addr);
void _fdivrl_mem(phys_addr_t addr);
void _fidivs_mem(phys_addr_t addr);
void _fidivrs_mem(phys_addr_t addr);
void _fidivl_mem(phys_addr_t addr);
void _fidivrl_mem(phys_addr_t addr);
void _fcom();
void _fcomp();
void _fcompp();
void _fcoms_mem(phys_addr_t addr);
void _fcoml_mem(phys_addr_t addr);
void _ficoms_mem(phys_addr_t addr);
void _ficoml_mem(phys_addr_t addr);
void _ftst();
void _fsts(phys_addr_t addr);
void _fstl(phys_addr_t addr);
void _fstps(phys_addr_t addr);
void _fstpl(phys_addr_t addr);
void _fstpt(phys_addr_t addr);
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
