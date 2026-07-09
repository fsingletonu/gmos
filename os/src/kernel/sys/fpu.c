#include <sys/fpu.h>

static inline void _fninit()
{
    asm volatile("fninit");
}

static inline void _fnstsw()
{
    asm volatile("fnstsw %0"
                 : "=m"(fpu_sys_st.status_word)
                 :
                 : "cc");
}

static inline void _fnstcw()
{
    asm volatile("fnstcw %0"
                 : "=m"(fpu_sys_st.control_word)
                 :
                 : "cc");
}

static inline void _fldcw()
{
    uint16_t cw_write = 0x103f;
    asm volatile("fldcw %0"
                 : "=m"(cw_write)
                 :
                 : "cc");
}

void judge_fpu()
{
    uint16_t keep = 0;
    _fninit();
    _fnstsw();
    _fnstcw();
    keep = fpu_sys_st.control_word;
    _fldcw();
    _fnstcw();
}

void fpu_init()
{
    uint32_t cr4 = 0;
    asm volatile("mov %%cr4,%0" : "=r"(cr4)::);
    cr4 |= ((1 << 9) | (1 << 10));
    asm volatile("mov %0,%%cr4" ::"r"(cr4) :);
    _fninit();
}

void _fnsave(phys_addr_t addr)
{
    asm volatile("fnsave (%0)"
                 :
                 : "r"(addr)
                 : "memory");
}

void _flds(phys_addr_t addr)
{
    asm volatile("flds (%0)"
                 :
                 : "r"(addr)
                 : "st");
}

void _fldl(phys_addr_t addr)
{
    asm volatile("fldl (%0)"
                 :
                 : "r"(addr)
                 : "st");
}

void _fldt(phys_addr_t addr)
{
    asm volatile("fldt (%0)"
                 :
                 : "r"(addr)
                 : "st");
}

void _faddp()
{
    asm volatile("faddp"
                 :
                 :
                 : "st");
}

void _fadd_sti_st0(int8_t st_i)
{
    if (st_i < 0 || st_i > 7)
        return;
    switch (st_i)
    {
    case 0:
        asm volatile("fadd %%st(0),%%st(0)" ::: "st");
        break;

    case 1:
        asm volatile("fadd %%st(1),%%st(0)" ::: "st");
        break;

    case 2:
        asm volatile("fadd %%st(2),%%st(0)" ::: "st");
        break;

    case 3:
        asm volatile("fadd %%st(3),%%st(0)" ::: "st");
        break;

    case 4:
        asm volatile("fadd %%st(4),%%st(0)" ::: "st");
        break;

    case 5:
        asm volatile("fadd %%st(5),%%st(0)" ::: "st");
        break;

    case 6:
        asm volatile("fadd %%st(6),%%st(0)" ::: "st");
        break;

    case 7:
        asm volatile("fadd %%st(7),%%st(0)" ::: "st");
        break;

    default:
        break;
    }
}

void _fadd_st0_sti(int8_t st_i)
{
    if (st_i < 0 || st_i > 7)
        return;
    switch (st_i)
    {
    case 0:
        asm volatile("fadd %%st(0),%%st(0)" ::: "st");
        break;

    case 1:
        asm volatile("fadd %%st(0),%%st(1)" ::: "st");
        break;

    case 2:
        asm volatile("fadd %%st(0),%%st(2)" ::: "st");
        break;

    case 3:
        asm volatile("fadd %%st(0),%%st(3)" ::: "st");
        break;

    case 4:
        asm volatile("fadd %%st(0),%%st(4)" ::: "st");
        break;

    case 5:
        asm volatile("fadd %%st(0),%%st(5)" ::: "st");
        break;

    case 6:
        asm volatile("fadd %%st(0),%%st(6)" ::: "st");
        break;

    case 7:
        asm volatile("fadd %%st(0),%%st(7)" ::: "st");
        break;

    default:
        break;
    }
}

void _fadds_mem(phys_addr_t addr)
{
    asm volatile("fadds (%0)"
                 :
                 : "r"(addr)
                 : "st");
}

void _faddl_mem(phys_addr_t addr)
{
    asm volatile("faddl (%0)"
                 :
                 : "r"(addr)
                 : "st");
}

void _fiadds_mem(phys_addr_t addr)
{
    asm volatile("fiadds (%0)"
                 :
                 : "r"(addr)
                 : "st");
}

void _fiaddl_mem(phys_addr_t addr)
{
    asm volatile("fiaddl (%0)"
                 :
                 : "r"(addr)
                 : "st");
}

void _fsubp()
{
    asm volatile("fsubp"
                 :
                 :
                 : "st");
}

void _fsubrp()
{
    asm volatile("fsubrp"
                 :
                 :
                 : "st");
}

void _fsub_sti_st0(int8_t st_i)
{
    if (st_i < 0 || st_i > 7)
        return;
    switch (st_i)
    {
    case 0:
        asm volatile("fsub %%st(0),%%st(0)" ::: "st");
        break;

    case 1:
        asm volatile("fsub %%st(1),%%st(0)" ::: "st");
        break;

    case 2:
        asm volatile("fsub %%st(2),%%st(0)" ::: "st");
        break;

    case 3:
        asm volatile("fsub %%st(3),%%st(0)" ::: "st");
        break;

    case 4:
        asm volatile("fsub %%st(4),%%st(0)" ::: "st");
        break;

    case 5:
        asm volatile("fsub %%st(5),%%st(0)" ::: "st");
        break;

    case 6:
        asm volatile("fsub %%st(6),%%st(0)" ::: "st");
        break;

    case 7:
        asm volatile("fsub %%st(7),%%st(0)" ::: "st");
        break;

    default:
        break;
    }
}

void _fsub_st0_sti(int8_t st_i)
{
    if (st_i < 0 || st_i > 7)
        return;
    switch (st_i)
    {
    case 0:
        asm volatile("fsub %%st(0),%%st(0)" ::: "st");
        break;

    case 1:
        asm volatile("fsub %%st(0),%%st(1)" ::: "st");
        break;

    case 2:
        asm volatile("fsub %%st(0),%%st(2)" ::: "st");
        break;

    case 3:
        asm volatile("fsub %%st(0),%%st(3)" ::: "st");
        break;

    case 4:
        asm volatile("fsub %%st(0),%%st(4)" ::: "st");
        break;

    case 5:
        asm volatile("fsub %%st(0),%%st(5)" ::: "st");
        break;

    case 6:
        asm volatile("fsub %%st(0),%%st(6)" ::: "st");
        break;

    case 7:
        asm volatile("fsub %%st(0),%%st(7)" ::: "st");
        break;

    default:
        break;
    }
}

void _fsubs_mem(phys_addr_t addr)
{
    asm volatile("fsubs (%0)"
                 :
                 : "r"(addr)
                 : "st");
}

void _fsubrs_mem(phys_addr_t addr)
{
    asm volatile("fsubrs (%0)"
                 :
                 : "r"(addr)
                 : "st");
}

void _fsubl_mem(phys_addr_t addr)
{
    asm volatile("fsubl (%0)"
                 :
                 : "r"(addr)
                 : "st");
}

void _fsubrl_mem(phys_addr_t addr)
{
    asm volatile("fsubrl (%0)"
                 :
                 : "r"(addr)
                 : "st");
}

void _fisubs_mem(phys_addr_t addr)
{
    asm volatile("fisubs (%0)"
                 :
                 : "r"(addr)
                 : "st");
}

void _fisubrs_mem(phys_addr_t addr)
{
    asm volatile("fisubrs (%0)"
                 :
                 : "r"(addr)
                 : "st");
}

void _fisubl_mem(phys_addr_t addr)
{
    asm volatile("fisubl (%0)"
                 :
                 : "r"(addr)
                 : "st");
}

void _fisubrl_mem(phys_addr_t addr)
{
    asm volatile("fisubrl (%0)"
                 :
                 : "r"(addr)
                 : "st");
}

void _fmulp()
{
    asm volatile("fmulp"
                 :
                 :
                 : "st");
}

void _fmul_sti_st0(int8_t st_i)
{
    if (st_i < 0 || st_i > 7)
        return;
    switch (st_i)
    {
    case 0:
        asm volatile("fmul %%st(0),%%st(0)" ::: "st");
        break;

    case 1:
        asm volatile("fmul %%st(1),%%st(0)" ::: "st");
        break;

    case 2:
        asm volatile("fmul %%st(2),%%st(0)" ::: "st");
        break;

    case 3:
        asm volatile("fmul %%st(3),%%st(0)" ::: "st");
        break;

    case 4:
        asm volatile("fmul %%st(4),%%st(0)" ::: "st");
        break;

    case 5:
        asm volatile("fmul %%st(5),%%st(0)" ::: "st");
        break;

    case 6:
        asm volatile("fmul %%st(6),%%st(0)" ::: "st");
        break;

    case 7:
        asm volatile("fmul %%st(7),%%st(0)" ::: "st");
        break;

    default:
        break;
    }
}

void _fmul_st0_sti(int8_t st_i)
{
    if (st_i < 0 || st_i > 7)
        return;
    switch (st_i)
    {
    case 0:
        asm volatile("fmul %%st(0),%%st(0)" ::: "st");
        break;

    case 1:
        asm volatile("fmul %%st(0),%%st(1)" ::: "st");
        break;

    case 2:
        asm volatile("fmul %%st(0),%%st(2)" ::: "st");
        break;

    case 3:
        asm volatile("fmul %%st(0),%%st(3)" ::: "st");
        break;

    case 4:
        asm volatile("fmul %%st(0),%%st(4)" ::: "st");
        break;

    case 5:
        asm volatile("fmul %%st(0),%%st(5)" ::: "st");
        break;

    case 6:
        asm volatile("fmul %%st(0),%%st(6)" ::: "st");
        break;

    case 7:
        asm volatile("fmul %%st(0),%%st(7)" ::: "st");
        break;

    default:
        break;
    }
}

void _fmuls_mem(phys_addr_t addr)
{
    asm volatile("fmuls (%0)"
                 :
                 : "r"(addr)
                 : "st");
}

void _fmull_mem(phys_addr_t addr)
{
    asm volatile("fmull (%0)"
                 :
                 : "r"(addr)
                 : "st");
}

void _fimuls_mem(phys_addr_t addr)
{
    asm volatile("fimuls (%0)"
                 :
                 : "r"(addr)
                 : "st");
}

void _fimull_mem(phys_addr_t addr)
{
    asm volatile("fimull (%0)"
                 :
                 : "r"(addr)
                 : "st");
}

void _fdivp()
{
    asm volatile("fdivp"
                 :
                 :
                 : "st");
}

void _fdiv_sti_st0(int8_t st_i)
{
    if (st_i < 0 || st_i > 7)
        return;
    switch (st_i)
    {
    case 0:
        asm volatile("fdiv %%st(0),%%st(0)" ::: "st");
        break;

    case 1:
        asm volatile("fdiv %%st(1),%%st(0)" ::: "st");
        break;

    case 2:
        asm volatile("fdiv %%st(2),%%st(0)" ::: "st");
        break;

    case 3:
        asm volatile("fdiv %%st(3),%%st(0)" ::: "st");
        break;

    case 4:
        asm volatile("fdiv %%st(4),%%st(0)" ::: "st");
        break;

    case 5:
        asm volatile("fdiv %%st(5),%%st(0)" ::: "st");
        break;

    case 6:
        asm volatile("fdiv %%st(6),%%st(0)" ::: "st");
        break;

    case 7:
        asm volatile("fdiv %%st(7),%%st(0)" ::: "st");
        break;

    default:
        break;
    }
}

void _fdiv_st0_sti(int8_t st_i)
{
    if (st_i < 0 || st_i > 7)
        return;
    switch (st_i)
    {
    case 0:
        asm volatile("fdiv %%st(0),%%st(0)" ::: "st");
        break;

    case 1:
        asm volatile("fdiv %%st(0),%%st(1)" ::: "st");
        break;

    case 2:
        asm volatile("fdiv %%st(0),%%st(2)" ::: "st");
        break;

    case 3:
        asm volatile("fdiv %%st(0),%%st(3)" ::: "st");
        break;

    case 4:
        asm volatile("fdiv %%st(0),%%st(4)" ::: "st");
        break;

    case 5:
        asm volatile("fdiv %%st(0),%%st(5)" ::: "st");
        break;

    case 6:
        asm volatile("fdiv %%st(0),%%st(6)" ::: "st");
        break;

    case 7:
        asm volatile("fdiv %%st(0),%%st(7)" ::: "st");
        break;

    default:
        break;
    }
}

void _fdivs_mem(phys_addr_t addr)
{
    asm volatile("fdivs (%0)"
                 :
                 : "r"(addr)
                 : "st");
}

void _fdivrs_mem(phys_addr_t addr)
{
    asm volatile("fdivrs (%0)"
                 :
                 : "r"(addr)
                 : "st");
}

void _fdivl_mem(phys_addr_t addr)
{
    asm volatile("fdivl (%0)"
                 :
                 : "r"(addr)
                 : "st");
}

void _fdivrl_mem(phys_addr_t addr)
{
    asm volatile("fdivrl (%0)"
                 :
                 : "r"(addr)
                 : "st");
}

void _fidivs_mem(phys_addr_t addr)
{
    asm volatile("fidivs (%0)"
                 :
                 : "r"(addr)
                 : "st");
}

void _fidivrs_mem(phys_addr_t addr)
{
    asm volatile("fidivrs (%0)"
                 :
                 : "r"(addr)
                 : "st");
}

void _fidivl_mem(phys_addr_t addr)
{
    asm volatile("fidivl (%0)"
                 :
                 : "r"(addr)
                 : "st");
}

void _fidivrl_mem(phys_addr_t addr)
{
    asm volatile("fidivrl (%0)"
                 :
                 : "r"(addr)
                 : "st");
}

void _fcom()
{
    asm volatile("fcom"
                 :
                 :
                 : "cc", "st");
}

void _fcomp()
{
    asm volatile("fcomp"
                 :
                 :
                 : "cc", "st");
}

void _fcompp()
{
    asm volatile("fcompp"
                 :
                 :
                 : "cc", "st");
}

void _fcoms_mem(phys_addr_t addr)
{
    asm volatile("fcoms (%0)"
                 :
                 : "r"(addr)
                 : "cc", "st");
}

void _fcoml_mem(phys_addr_t addr)
{
    asm volatile("fcoml (%0)"
                 :
                 : "r"(addr)
                 : "cc", "st");
}

void _ficoms_mem(phys_addr_t addr)
{
    asm volatile("ficoms (%0)"
                 :
                 : "r"(addr)
                 : "cc", "st");
}

void _ficoml_mem(phys_addr_t addr)
{
    asm volatile("ficoml (%0)"
                 :
                 : "r"(addr)
                 : "cc", "st");
}

void _ftst()
{
    asm volatile("ftst" ::: "cc", "st");
}

void _fsts(phys_addr_t addr)
{
    asm volatile("fsts (%0)"
                 :
                 : "r"(addr)
                 : "st");
}

void _fstl(phys_addr_t addr)
{
    asm volatile("fstl (%0)"
                 :
                 : "r"(addr)
                 : "st");
}

void _fstps(phys_addr_t addr)
{
    asm volatile("fstps (%0)"
                 :
                 : "r"(addr)
                 : "st");
}

void _fstpl(phys_addr_t addr)
{
    asm volatile("fstpl (%0)"
                 :
                 : "r"(addr)
                 : "st");
}

void _fstpt(phys_addr_t addr)
{
    asm volatile("fstpt (%0)"
                 :
                 : "r"(addr)
                 : "st");
}

void _fld1()
{
    asm volatile("fld1" ::: "st");
}

void _fldz()
{
    asm volatile("fldz" ::: "st");
}

void _fldpi()
{
    asm volatile("fldpi" ::: "st");
}

void _fldl2e()
{
    asm volatile("fldl2e" ::: "st");
}

void _fldln2()
{
    asm volatile("fldln2" ::: "st");
}

void _fsqrt()
{
    asm volatile("fsqrt" ::: "st");
}

void _fabs()
{
    asm volatile("fabs" ::: "st");
}

void _fchs()
{
    asm volatile("fchs" ::: "st");
}

void _fsin()
{
    asm volatile("fsin" ::: "st");
}

void _fcos()
{
    asm volatile("fcos" ::: "st");
}
