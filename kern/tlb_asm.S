#include <asm/asm.h>

LEAF(tlb_out)
.set noreorder /* 禁止重排 */
	mfc0    t0, CP0_ENTRYHI 	/* 读 CP0_ENTRYHI */
	mtc0    a0, CP0_ENTRYHI 	/* 存 CP0_ENTRYHI */
	nop
	/* Step 1: Use 'tlbp' to probe TLB entry */
	/* Exercise 2.8: Your code here. (1/2) */
	tlbp						/* 根据 CP0_ENTRYHI */

	nop
	/* Step 2: Fetch the probe result from CP0.Index */
	mfc0    t1, CP0_INDEX		/* 读 CP0_INDEX */
.set reorder
	bltz    t1, NO_SUCH_ENTRY	/* 小于 0 时不存在该表项 */
.set noreorder
	mtc0    zero, CP0_ENTRYHI	/* 写入 0 */
	mtc0    zero, CP0_ENTRYLO0	/* 写入 0 */
	nop
	/* Step 3: Use 'tlbwi' to write CP0.EntryHi/Lo into TLB at CP0.Index */
	/* Exercise 2.8: Your code here. (2/2) */
	tlbwi						/* 用 0 覆盖原表项 */
.set reorder

NO_SUCH_ENTRY:
	mtc0    t0, CP0_ENTRYHI		/* 写回 CP0_ENTRYHI */
	j       ra					/* return */
END(tlb_out)

.data
tlb_refill_ra:
.word 0
.text
NESTED(do_tlb_refill, 0, zero)
	mfc0    a0, CP0_BADVADDR
	mfc0    a1, CP0_ENTRYHI
	srl     a1, a1, 6
	andi    a1, a1, 0b111111
	sw      ra, tlb_refill_ra
	jal     _do_tlb_refill
	lw      ra, tlb_refill_ra
	mtc0    v0, CP0_ENTRYLO0
	/* See <IDT R30xx Family Software Reference Manual> Chapter 6-8
	nop
	/* Hint: use 'tlbwr' to write CP0.EntryHi/Lo into a random tlb entry. */
	/* Exercise 2.10: Your code here. */

	jr      ra
END(do_tlb_refill)
