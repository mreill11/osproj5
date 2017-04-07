import os
import matplotlib.pyplot as plt

frame_numbers = [1, 10, 20, 30, 40, 50, 60, 70, 80, 90, 100]

def graph_program(prog_name):
    rand_page_faults = []
    rand_disk_reads = []
    rand_disk_writes = []
    fifo_page_faults = []
    fifo_disk_reads = []
    fifo_disk_writes = []
    custom_page_faults = []
    custom_disk_reads = []
    custom_disk_writes = []
    for num in frame_numbers:
        rand_command = "./virtmem 100 {} rand {}".format(str(num), prog_name)
        fifo_command = "./virtmem 100 {} fifo {}".format(str(num), prog_name)
        custom_command = "./virtmem 100 {} custom {}".format(str(num), prog_name)

        stream = os.popen(rand_command)
        for line in stream:
            if 'Page' in line:
                rand_page_faults.append(int(line.rstrip().split(' ')[-1]))
            elif 'Disk Reads' in line:
                rand_disk_reads.append(int(line.rstrip().split(' ')[-1]))
            elif 'Disk Writes' in line:
                rand_disk_writes.append(int(line.rstrip().split(' ')[-1]))
            else:
                continue

        stream = os.popen(fifo_command)
        for line in stream:
            if 'Page' in line:
                fifo_page_faults.append(int(line.rstrip().split(' ')[-1]))
            elif 'Disk Reads' in line:
                fifo_disk_reads.append(int(line.rstrip().split(' ')[-1]))
            elif 'Disk Writes' in line:
                fifo_disk_writes.append(int(line.rstrip().split(' ')[-1]))
            else:
                continue

        stream = os.popen(custom_command)
        for line in stream:
            if 'Page' in line:
                custom_page_faults.append(int(line.rstrip().split(' ')[-1]))
            elif 'Disk Reads' in line:
                custom_disk_reads.append(int(line.rstrip().split(' ')[-1]))
            elif 'Disk Writes' in line:
                custom_disk_writes.append(int(line.rstrip().split(' ')[-1]))
            else:
                continue

    # plot random
    plt.plot(frame_numbers, rand_page_faults, 'r--', label='Page Faults')
    plt.plot(frame_numbers, rand_disk_reads, 'bs', label='Disk Reads')
    plt.plot(frame_numbers, rand_disk_writes, 'g^', label='Disk Writes')
    plt.legend(loc='upper left')
    plt.title('rand page fault handler run on {} program'.format(prog_name))
    plt.xlabel('frame number')
    plt.ylabel('disk operations and page faults')
    plt.show()

    # plot fifo
    plt.plot(frame_numbers, fifo_page_faults, 'r--', label='Page Faults')
    plt.plot(frame_numbers, fifo_disk_reads, 'bs', label='Disk Reads')
    plt.plot(frame_numbers, fifo_disk_writes, 'g^', label='Disk Writes')
    plt.legend(loc='upper left')
    plt.title('fifo page fault handler run on {} program'.format(prog_name))
    plt.xlabel('frame number')
    plt.ylabel('disk operations and page faults')
    plt.show()

    # plot custom
    plt.plot(frame_numbers, custom_page_faults, 'r--', label='Page Faults')
    plt.plot(frame_numbers, custom_disk_reads, 'bs', label='Disk Reads')
    plt.plot(frame_numbers, custom_disk_writes, 'g^', label='Disk Writes')
    plt.legend(loc='upper left')
    plt.title('custom page fault handler run on {} program'.format(prog_name))
    plt.xlabel('frame number')
    plt.ylabel('disk operations and page faults')
    plt.show()

    print "rand_page_faults {}".format(rand_page_faults)
    print "rand_disk_reads {}".format(rand_disk_reads)
    print "rand_disk_writes {}".format(rand_disk_writes)

    print "fifo_page_faults {}".format(fifo_page_faults)
    print "fifo_disk_reads {}".format(fifo_disk_reads)
    print "fifo_disk_writes {}".format(fifo_disk_writes)

    print "custom_page_faults {}".format(custom_page_faults)
    print "custom_disk_reads {}".format(custom_disk_reads)
    print "custom_disk_writes {}".format(custom_disk_writes)


graph_program('sort')
graph_program('scan')
graph_program('focus')