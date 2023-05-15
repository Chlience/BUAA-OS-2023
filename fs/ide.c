/*
 * operations on IDE disk.
 */

#include "serv.h"
#include <drivers/dev_disk.h>
#include <lib.h>
#include <mmu.h>

// Overview:
//  read data from IDE disk. First issue a read request through
//  disk register and then copy data from disk buffer
//  (512 bytes, a sector) to destination array.
//
// Parameters:
//  diskno: disk number.
//  secno: start sector number.
//  dst: destination for data read from IDE disk.
//  nsecs: the number of sectors to read.
//
// Post-Condition:
//  Panic if any error occurs. (you may want to use 'panic_on')
//
// Hint: Use syscalls to access device registers and buffers.
// Hint: Use the physical address and offsets defined in 'include/drivers/dev_disk.h':
//  'DEV_DISK_ADDRESS', 'DEV_DISK_ID', 'DEV_DISK_OFFSET', 'DEV_DISK_OPERATION_READ',
//  'DEV_DISK_START_OPERATION', 'DEV_DISK_STATUS', 'DEV_DISK_BUFFER'
void ide_read(u_int diskno, u_int secno, void *dst, u_int nsecs) {
	u_int begin = secno * BY2SECT;
	u_int end = begin + nsecs * BY2SECT;

	for (u_int off = 0; begin + off < end; off += BY2SECT) {
		uint32_t temp = diskno;
		/* Exercise 5.3: Your code here. (1/2) */
		panic_on(msyscall(SYS_write_dev, &temp, DEV_DISK_ADDRESS + DEV_DISK_ID, 4));
		temp = begin + off;
		panic_on(msyscall(SYS_write_dev, &temp , DEV_DISK_ADDRESS + DEV_DISK_OFFSET, 4));
		temp = 0;
		panic_on(msyscall(SYS_write_dev, &temp , DEV_DISK_ADDRESS + DEV_DISK_START_OPERATION, 4));
		panic_on(msyscall(SYS_read_dev, &temp , DEV_DISK_ADDRESS + DEV_DISK_STATUS, 4));
		panic_on(temp == 0);
		panic_on(msyscall(SYS_read_dev, dst + off, DEV_DISK_ADDRESS + DEV_DISK_BUFFER, BY2SECT));
	}
}

// Overview:
//  write data to IDE disk.
//
// Parameters:
//  diskno: disk number.
//  secno: start sector number.
//  src: the source data to write into IDE disk.
//  nsecs: the number of sectors to write.
//
// Post-Condition:
//  Panic if any error occurs.
//
// Hint: Use syscalls to access device registers and buffers.
// Hint: Use the physical address and offsets defined in 'include/drivers/dev_disk.h':
//  'DEV_DISK_ADDRESS', 'DEV_DISK_ID', 'DEV_DISK_OFFSET', 'DEV_DISK_BUFFER',
//  'DEV_DISK_OPERATION_WRITE', 'DEV_DISK_START_OPERATION', 'DEV_DISK_STATUS'
void ide_write(u_int diskno, u_int secno, void *src, u_int nsecs) {
	u_int begin = secno * BY2SECT;
	u_int end = begin + nsecs * BY2SECT;

	for (u_int off = 0; begin + off < end; off += BY2SECT) {
		uint32_t temp = diskno;
		/* Exercise 5.3: Your code here. (2/2) */
		panic_on(msyscall(SYS_write_dev, &temp, DEV_DISK_ADDRESS + DEV_DISK_ID, 4));
		temp = begin + off;
		panic_on(msyscall(SYS_write_dev, &temp , DEV_DISK_ADDRESS + DEV_DISK_OFFSET, 4));
		panic_on(msyscall(SYS_write_dev, src + off, DEV_DISK_ADDRESS + DEV_DISK_BUFFER, BY2SECT));
		temp = 1;
		panic_on(msyscall(SYS_write_dev, &temp , DEV_DISK_ADDRESS + DEV_DISK_START_OPERATION, 4));
		panic_on(msyscall(SYS_read_dev, &temp , DEV_DISK_ADDRESS + DEV_DISK_STATUS, 4));
		panic_on(temp == 0);
	}
}

int ssd_map[32];
int ssd_state[32]; // 0：可写，1：不可写
int ssd_erase_time[32];
int n = 32;

void ssd_init() {
	for (int i = 0; i < n; ++ i) {
		ssd_map[i] = -1;
		ssd_state[i] = 0;
		ssd_erase_time[i] = 0;
	}
}

int ssd_read(u_int logic_no, void *dst) {
	if (ssd_map[logic_no] == -1) {
		return -1;
	}
	u_int physical_no = ssd_map[logic_no];
	ide_read(0, physical_no, dst, 1);
	return 0;
}

void ssd_erase(u_int logic_no) {
	if (ssd_map[logic_no] == -1) {
		return;
	}
	u_int physical_no = ssd_map[logic_no];
	for(int i = 0; i < 512 / 4; ++ i) {
		temp_512[i] = 0;
	}
	ide_write(0, physical_no, temp_512, 1);
	ssd_map[logic_no] = -1;
	ssd_state[physical_no] = 1;
	ssd_erase_time[physical_no]++;
}

int temp_512[512 / 4];

u_int alloc_ssd() {
	u_int min_erase_no = -1;
	for (int i = 0; i < n; ++ i) {
		if (ssd_state[i] == 0) {
			if (min_erase_no == -1 || ssd_erase_time[i] < ssd_erase_time[min_erase_no]) {
				min_erase_no = i;
			}
		}
	}
	if (min_erase_no == - 1) {
		debugf("no unused ssd block!\n");
	}
	if (ssd_erase_time[min_erase_no] >= 5) {
		u_int used_min_erase_no = - 1;
		for (int i = 0; i < n; ++ i) {
			if (ssd_state[i] == 1) {
				if (used_min_erase_no == -1 || ssd_erase_time[i] < ssd_erase_time[used_min_erase_no]) {
					used_min_erase_no = i;
				}
			}
		}
		if (used_min_erase_no == - 1) {
			debugf("no used ssd block!\n");
		}
		ide_read(0, used_min_erase_no, temp_512, 1);
		ide_write(0, min_erase_no, temp_512, 1);
		ssd_state[min_erase_no] = 1;
		u_int used_logic_no = - 1;
		for (int i = 0; i < 32; ++ i) {
			if (ssd_map[i] == used_min_erase_no) {
				used_logic_no = i;
				break;
			}
		}
		ssd_erase(used_logic_no);
		min_erase_no = used_min_erase_no;
	}
	return min_erase_no;
}

void ssd_write(u_int logic_no, void *src) {
	if (ssd_map[logic_no] != -1) {
		ssd_erase(logic_no);
	}
	u_int physical_no = alloc_ssd();
	ssd_map[logic_no] = physical_no;
	ide_write(0, physical_no, src, 1);
	ssd_state[physical_no] = 1;
}
