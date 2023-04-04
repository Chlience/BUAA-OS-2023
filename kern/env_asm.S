#include <asm/asm.h>
#include <mmu.h>
#include <trap.h>

.text
LEAF(env_pop_tf)
.set reorder
.set at
	sll     a1, a1, 6
	mtc0    a1, CP0_ENTRYHI
	move    sp, a0
	j       ret_from_exception
END(env_pop_tf)

LEAF(enable_irq)
	/**
	 * STATUS_CU0:
	 * set 1 to be able to use some nominally-
	 * privileged instructions in user mode (this is rarely if ever done).
	 * The CPU control instructions encoded as ‘‘co-processor 0’’ type
	 * are always usable in kernel mode, regardless of the setting of this
	 * bit
	 * see IDTR30xx Document
	*/
	/**
	 * STATUS_IM4:
	 * enable IM4(rtc)
	 * 启用外部中断
	 * see https://gavare.se/gxemul/gxemul-stable/doc/experiments.html#expdevices
	*/
	/**
	 * STATUS_IEc:
	 * set 1 for user mode
	 * set 0 for kernel mode
	*/
	li      t0, (STATUS_CU0 | STATUS_IM4 | STATUS_IEc)
	/* 写入 CP0 STATUS 寄存器 */
	mtc0    t0, CP0_STATUS
	/* 结束函数 */
	jr      ra
END(enable_irq)
