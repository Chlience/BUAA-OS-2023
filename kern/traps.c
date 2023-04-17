#include <env.h>
#include <pmap.h>
#include <printk.h>
#include <trap.h>

extern void handle_int(void);
extern void handle_tlb(void);
extern void handle_sys(void);
extern void handle_mod(void);
extern void handle_reserved(void);
extern void handle_ov(void);

void (*exception_handlers[32])(void) = {
    [0 ... 31] = handle_reserved,
    [0] = handle_int,
    [2 ... 3] = handle_tlb,
    [12] = handle_ov,
#if !defined(LAB) || LAB >= 4
    [1] = handle_mod,
    [8] = handle_sys,
#endif
};

/* Overview:
 *   The fallback handler when an unknown exception code is encountered.
 *   'genex.S' wraps this function in 'handle_reserved'.
 */
void do_reserved(struct Trapframe *tf) {
	print_tf(tf);
	panic("Unknown ExcCode %2d", (tf->cp0_cause >> 2) & 0x1f);
}

#define OPTION(instr) ((u_int)instr >> 26)
#define SPECIAL(instr) ((u_int)instr & (u_int)0b111111)
#define IS_ADD(instr) ((OPTION(instr) == (u_int)0) && (SPECIAL(instr) == (u_int)0b100000))
#define IS_SUB(instr) ((OPTION(instr) == (u_int)0) && (SPECIAL(instr) == (u_int)0b100010))
#define IS_ADDI(instr) (OPTION(instr) == (u_int)0b001000) 
#define rs(instr) (((u_int)instr >> 21) & ((u_int)0b11111))
#define rt(instr) (((u_int)instr >> 16) & ((u_int)0b11111))
#define rd(instr) (((u_int)instr >> 11) & ((u_int)0b11111))
#define imm(instr) ((u_int)instr & ((u_int)0xffff))

void do_ov(struct Trapframe *tf) {
    Pte *pte;
    u_int va = tf->cp0_epc;
    page_lookup(cur_pgdir, va, &pte);
    u_int pa = PTE_ADDR(*pte) | (va & 0xfff);
    u_int *instr = KADDR(pa);
    if (IS_ADD(*instr)) {
        *instr = (rs(*instr) << 21) | (rt(*instr) << 16) | (rd(*instr) << 11) | (u_int)0b100001;
        printk("add ov handled\n");
    } else if (IS_SUB(*instr)) {
        *instr = (rs(*instr) << 21) | (rt(*instr) << 16) | (rd(*instr) << 11) | (u_int)0b100011;
        printk("sub ov handled\n");
    } else if (IS_ADDI(*instr)) {
        tf->regs[rt(*instr)] = (((u_int)tf->regs[rs(*instr)]) / 2) + (((u_int)imm(*instr)) / 2);
        tf->cp0_epc = tf->cp0_epc + 4;
        printk("addi ov handled\n");
    }
    curenv->env_ov_cnt++;
}
