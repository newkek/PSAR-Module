#ifndef _H_IBS_MANAGING_
#define _H_IBS_MANAGING_

#include <linux/cpu.h>
#include <linux/kernel.h>

#define IBS_OP_LOW_VALID_BIT           (1ULL<<18)      /* bit 18 */
#define IBS_OP_LOW_ENABLE              (1ULL<<17)      /* bit 17 */
#define APIC_EILVT_LVTOFF_IBS          1
#define APIC_EILVT0                    0x500

typedef struct {
  unsigned char IbsLdOp:1;
  unsigned char IbsStOp:1;
  unsigned char IbsDcL1tlbMiss:1;
  unsigned char IbsDcL2tlbMiss:1;
  unsigned char IbsDcL1tlbHit2M:1;
  unsigned char IbsDcL1tlbHit1G:1;
  unsigned char IbsDcL2tlbHit2M:1;
  unsigned char IbsDcMiss:1;
  unsigned char IbsDcMisAcc:1;
  unsigned char IbsDcLdBnkCon:1;
  unsigned char IbsDcStBnkCon:1;
  unsigned char IbsDcStToLdFwd:1;
  unsigned char IbsDcStToLdCan:1;
  unsigned char IbsDcWcMemAcc:1;
  unsigned char IbsDcUcMemAcc:1;
  unsigned char IbsDcLockedOp:1;
  unsigned char IbsDcMabHit:1;
  unsigned char IbsDcLinAddrValid:1;
  unsigned char IbsDcPhyAddrValid:1;
  unsigned char IbsDcL2tlbHit1G:1;
  unsigned short reserved:12;
  unsigned short IbsDcMissLat:16;
  unsigned short reserved2:16;
} IbsOpData3_t;

typedef struct {
  unsigned char NbIbsReqSrc:3;
  unsigned char reserved:1;
  unsigned char NbIbsReqDstProc:1;
  unsigned char NbIbsReqCacheHitSt:1;
  unsigned long reserved2:58;
} IbsOpData2_t;

typedef struct {
  unsigned short IbsCompToRetCtr:16;
  unsigned short IbsTagToRetCtr:16;
  unsigned char IbsOpBrnResync:1;
  unsigned char IbsOpMispReturn:1;
  unsigned char IbsOpReturn:1;
  unsigned char IbsOpBrnTaken:1;
  unsigned char IbsOpBrnMisp:1;
  unsigned char IbsOpBrnRet:1;
  unsigned long reserved:26;
} IbsOpData_t;

struct ibs_op_sample {
  union {
    struct {
      unsigned int ibs_op_rip_low;
      unsigned int ibs_op_rip_high;
      unsigned int ibs_op_data1_low;
      unsigned int ibs_op_data1_high;
      unsigned int ibs_op_data2_low;
      unsigned int ibs_op_data2_high;
      unsigned int ibs_op_data3_low;
      unsigned int ibs_op_data3_high;
      unsigned int ibs_dc_linear_low;
      unsigned int ibs_dc_linear_high;
      unsigned int ibs_dc_phys_low;
      unsigned int ibs_dc_phys_high;
    };
    struct {
      u64 ibs_op_rip;
      u64 ibs_op_data1;
      u64 ibs_op_data2;
      u64 ibs_op_data3;
      u64 ibs_dc_linear;
      u64 ibs_dc_phys;
    };
    struct {
      u64 rip;
      IbsOpData_t data1;
      IbsOpData2_t data2;
      IbsOpData3_t data3;
      u64 lin_addr;
      u64 phys_addr;
    };
  };
};

//static int handle_ibs_nmi(struct pt_regs* const regs);

void ibs_start(void);
void ibs_stop(void);
/* void my_shutdown_apic_wesh(void* args); */
/* void set_ibs_rate(void* args); */
/* static void apic_init_ibs_nmi_per_cpu (void * args); */
/* int ibs_cpu_notifier(struct notifier_block *b, unsigned long action, void *data); */

#endif /* _H_IBS_MANAGING_ */
