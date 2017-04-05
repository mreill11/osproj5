/*
Main program for the virtual memory project.
Make all of your modifications to this file.
You may add or rearrange any code or data as you need.
The header files page_table.h and disk.h explain
how to use the page table and disk interfaces.
*/

#include "page_table.h"
#include "disk.h"
#include "program.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define NOT_FOUND -1

struct disk *disk;
int numPageFaults = 0;
int numDiskWrite = 0;
int numDiskRead = 0;
int runMode;            // RAND, FIFO, or CUSTOM
int *ptArr;
int count = 0;

int search(int start, int end, int k) {
    int i;
    for (i = start; i <= end; i++) {
        if (ptArr[i] == k)
            return i;
    }
    return NOT_FOUND;
}


void page_fault_handler( struct page_table *pt, int page )
{
    int numPages = page_table_get_npages(pt);
    int numFrames = page_table_get_nframes(pt);
    char *pmem = page_table_get_physmem(pt);

    if (numFrames >= numPages) {
        printf("Page fault on page #%d\n", page);
        page_table_set_entry(pt, page, page, PROT_READ|PROT_WRITE);
        numPageFaults++;
        numDiskWrite = 0;
        numDiskRead = 0;
    } else if (runMode == 1) {
        numPageFaults++;

        int val = search(0, numFrames - 1, page);
        int tmp = lrand48() % numFrames;

        if (val > -1) {
            page_table_set_entry(pt, page, val, PROT_READ|PROT_WRITE);
            numPageFaults--;
        } else if (count < numFrames) {
            while (ptArr[tmp] != -1) {
                tmp = lrand48() % numFrames;
                numPageFaults++;
            }

            page_table_set_entry(pt, page, tmp, PROT_READ);
            disk_read(disk, page, &pmem[tmp * PAGE_SIZE]);

            numDiskRead++;
            ptArr[tmp] = page;
            count++;
        } else {
            disk_write(disk, ptArr[tmp], &pmem[tmp * PAGE_SIZE]);
            disk_read(disk, page, &pmem[tmp * PAGE_SIZE]);

            numDiskWrite++;
            numDiskRead++;

            page_table_set_entry(pt, page, tmp, PROT_READ);
            ptArr[tmp] = page;
        }
        page_table_print(pt);

    } else if (runMode == 2) {
        numPageFaults++;
        int val = search(0, numFrames - 1, page);

        if (val > -1) {
            page_table_set_entry(pt, page, val, PROT_READ|PROT_WRITE);
            count--;
            numPageFaults--;
        } else if (ptArr[count] == -1) {
            page_table_set_entry(pt, page, count, PROT_READ);
            disk_read(disk, page, &pmem[count * PAGE_SIZE]);
            numDiskRead++;
        } else {
            disk_write(disk, ptArr[count], &pmem[count * PAGE_SIZE]);
            disk_read(disk, page, &pmem[count * PAGE_SIZE]);
            numDiskWrite++;
            numDiskRead++;
            page_table_set_entry(pt, page, count, PROT_READ);
        }

        ptArr[count] = page;
        count = (count + 1) % numFrames;
        page_table_print(pt);
        
    }

    //printf("page fault on page #%d\n",page);
    //exit(1);
}

int main( int argc, char *argv[] )
{
    if(argc!=5) {
        printf("use: virtmem <npages> <nframes> <rand|fifo|lru|custom> <sort|scan|focus>\n");
        return 1;
    }

    int npages = atoi(argv[1]);
    int nframes = atoi(argv[2]);
    const char *program = argv[4];

    // Parse run mode
    if ((strcmp(argv[3], "rand")) == 0)
        runMode = 1;
    else if ((strcmp(argv[3], "fifo")) == 0)
        runMode = 2;
    else
        runMode = 3;

    ptArr = (int *) malloc(nframes * sizeof(int));
    int loop;
    for (loop = 0; loop < nframes; loop++)      // populate with -1
        ptArr[loop] = -1;

    disk = disk_open("myvirtualdisk",npages);
    if(!disk) {
        fprintf(stderr,"couldn't create virtual disk: %s\n",strerror(errno));
        return 1;
    }


    struct page_table *pt = page_table_create( npages, nframes, page_fault_handler );
    if(!pt) {
        fprintf(stderr,"couldn't create page table: %s\n",strerror(errno));
        return 1;
    }

    char *virtmem = page_table_get_virtmem(pt);

    //char *physmem = page_table_get_physmem(pt);

    if(!strcmp(program,"sort")) {
        sort_program(virtmem,npages*PAGE_SIZE);

    } else if(!strcmp(program,"scan")) {
        scan_program(virtmem,npages*PAGE_SIZE);

    } else if(!strcmp(program,"focus")) {
        focus_program(virtmem,npages*PAGE_SIZE);

    } else {
        fprintf(stderr,"unknown program: %s\n",argv[3]);
        return 1;
    }

    page_table_delete(pt);
    disk_close(disk);

    return 0;
}