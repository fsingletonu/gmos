#ifndef _APIC_H_
#define _APIC_H_

#include <datatype.h>
#include <init/idt.h>

#define LOCAL_APIC_ID_OFFSET 0x020
#define LOCAL_APIC_VERSION_OFFSET 0x030
#define LOCAL_APIC_TPR_OFFSET 0x080
#define LOCAL_APIC_APR_OFFSET 0x090
#define LOCAL_APIC_PPR_OFFSET 0x0A0
#define LOCAL_APIC_EOI_OFFSET 0x0B0
#define LOCAL_APIC_LDR_OFFSET 0x0D0
#define LOCAL_APIC_DFR_OFFSET 0x0E0
#define LOCAL_APIC_SVR_OFFSET 0x0F0
#define LOCAL_APIC_ISR_BASE 0x100
#define LOCAL_APIC_TMR_BASE 0x180
#define LOCAL_APIC_IRR_BASE 0x200
#define LOCAL_APIC_ESR_OFFSET 0x280
#define LOCAL_APIC_ICR_LOW_OFFSET 0x300
#define LOCAL_APIC_ICR_HIGH_OFFSET 0x310
#define LOCAL_APIC_LVT_TIMER_OFFSET 0x320
#define LOCAL_APIC_LVT_THERMAL_OFFSET 0x330
#define LOCAL_APIC_LVT_PERF_OFFSET 0x340
#define LOCAL_APIC_LVT_LINT0_OFFSET 0x350
#define LOCAL_APIC_LVT_LINT1_OFFSET 0x360
#define LOCAL_APIC_LVT_ERROR_OFFSET 0x370
#define LOCAL_APIC_TIMER_INIT_OFFSET 0x380
#define LOCAL_APIC_TIMER_CURR_OFFSET 0x390
#define LOCAL_APIC_TIMER_DCR_OFFSET 0x3E0

// 这里的reg都传指针
#define apic_barrier() __sync_synchronize()
#define apic_read(reg) (*(reg))
#define apic_write(reg, val) (*(reg) = (val))

typedef volatile uint32_t apic_reg_t;

#pragma pack(push, 1)
typedef struct local_apic
{
    volatile uint32_t reserved[8];
    union apic_reg_id
    {
        struct
        {
            uint32_t reserved : 24;
            uint32_t x1_apic_id : 8;
        } x1_apic;
        apic_reg_t x2_apic_id;
    } id;
    apic_reg_t version;
    volatile uint32_t reserved2[4];
    apic_reg_t tpr;
    apic_reg_t apr;
    apic_reg_t ppr;
    apic_reg_t eoi;
    apic_reg_t ldr;
    apic_reg_t dfr;
    apic_reg_t svr;
    apic_reg_t isr[8];
    apic_reg_t tmr[8];
    apic_reg_t irr[8];
    apic_reg_t esr;
    volatile uint32_t reserved3[28];
    apic_reg_t icr_low;
    apic_reg_t icr_high;
    apic_reg_t lvt_timer;
    apic_reg_t lvt_thermal;
    apic_reg_t lvt_perf;
    apic_reg_t lvt_lint0;
    apic_reg_t lvt_lint1;
    apic_reg_t lvt_error;
    apic_reg_t timer_initial;
    apic_reg_t timer_current;
    volatile uint32_t reserved4[16];
    apic_reg_t timer_divide;
} local_apic_t;
#pragma pack(pop)

void apic_init();
void apic_eoi();

#endif
