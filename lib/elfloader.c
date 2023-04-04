#include <elf.h>
#include <pmap.h>

const Elf32_Ehdr *elf_from(const void *binary, size_t size) {
	const Elf32_Ehdr *ehdr = (const Elf32_Ehdr *)binary;
	if (size >= sizeof(Elf32_Ehdr) && ehdr->e_ident[EI_MAG0] == ELFMAG0 &&
	    ehdr->e_ident[EI_MAG1] == ELFMAG1 && ehdr->e_ident[EI_MAG2] == ELFMAG2 &&
	    ehdr->e_ident[EI_MAG3] == ELFMAG3 && ehdr->e_type == 2) {
		return ehdr;
	}
	return NULL;
}

/* Overview:
 *   load an elf format binary file. Map all section
 * at correct virtual address.
 *
 * Pre-Condition:
 *   `binary` can't be NULL and `size` is the size of binary.
 *
 * Post-Condition:
 *   Return 0 if success. Otherwise return < 0.
 *   If success, the entry point of `binary` will be stored in `start`
 */
int elf_load_seg(Elf32_Phdr *ph, const void *bin, elf_mapper_t map_page, void *data) {
	u_long va = ph->p_vaddr;
	size_t bin_size = ph->p_filesz;
	size_t sgsize = ph->p_memsz;
	u_int perm = PTE_V;
	if (ph->p_flags & PF_W) {
		perm |= PTE_D;
	}

	int r;
	size_t i;

	/* 加载前端未对齐部分 */
	u_long offset = va - ROUNDDOWN(va, BY2PG);
	/* offset: va 到页头的距离 */
	if (offset != 0) {
		/* map_page 为回调函数 */
		if ((r = map_page(data, va, offset, perm, bin, MIN(bin_size, BY2PG - offset))) !=
		    0) {
			return r;
		}
	}

	/* Step 1: load all content of bin into memory. */
	/** 
	 * 加载剩余部分
	 * 如果 offset == 0，则说明已然对齐，从头开始
	 * 如果 offset != 0，则说明未对齐，从未对齐部分末尾 MIN(bin_size, BY2PG - offset) 开始
	 * */
	for (i = offset ? MIN(bin_size, BY2PG - offset) : 0; i < bin_size; i += BY2PG) {
		/* 注意每次 copy 的长度不超过 BY2PG */
		if ((r = map_page(data, va + i, 0, perm, bin + i, MIN(bin_size - i, BY2PG))) != 0) {
			return r;
		}
	}

	/* Step 2: alloc pages to reach `sgsize` when `bin_size` < `sgsize`. */
	/** 
	 * 将 seg 剩余部分用零填充
	 * */
	while (i < sgsize) {
		if ((r = map_page(data, va + i, 0, perm, NULL, MIN(bin_size - i, BY2PG))) != 0) {
			return r;
		}
		i += BY2PG;
	}
	return 0;
}
