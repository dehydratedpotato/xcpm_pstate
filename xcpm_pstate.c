
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/fcntl.h>

#define XCPM_GET_PSTATE_CTRS 0x2000580D
#define XCPM_GET_PSTATE_TABLE 0x20005801

typedef struct pstate {
    uint32_t pstate_id;
    uint32_t nominal_frequency;
    uint32_t unknown;
    uint32_t reserved;
} pstate;

typedef struct pstate_table {
    uint32_t high_frequency_modes_turbo[2];
    uint32_t high_frequency_modes_nonturbo[2];
    uint32_t max_efficent_frequency;
    uint32_t state_count;

    pstate states[64];
} pstate;

int main(int argc, const char * argv[]) {
    if (getuid() != 0) {
        printf("This tool requires root\n");
        return -1;
    }

    int32_t fd;
    pstate_table *table;
    uint32_t pstatecnt;
    uint64_t* ctr_a;
    uint64_t* ctr_b;
    
    double sumdiff = 0;
    double freq = 0;
        
    // open xcpm
    fd = open("/dev/xcpm", 0);
    if(fd == -1) perror("read");
    
    // get pstate table
    table = malloc(sizeof(pstate_table));
    ioctl(fd, XCPM_GET_PSTATE_TABLE, table, 0);
    
    pstatecnt = table->state_count;

    // sample ctrs
    ctr_a = malloc(sizeof(uint64_t) * pstatecnt);
    ctr_b = malloc(sizeof(uint64_t) * pstatecnt);
    
    ioctl(fd, XCPM_GET_PSTATE_CTRS, ctr_a, 0);
    sleep(1); // update interval
    ioctl(fd, XCPM_GET_PSTATE_CTRS, ctr_b, 0);
    
    printf("Max Efficient State: %u MHz\n", table->max_efficent_frequency);
    printf("Non-Turbo States: %u-%u MHz\n", table->high_frequency_modes_nonturbo[0], table->high_frequency_modes_nonturbo[1]);
    printf("Turbo States: %u-%u MHz\n\n", table->high_frequency_modes_turbo[0], table->high_frequency_modes_turbo[1]);
    printf("Requested Distribution: ");

    // format to distribution
    for (int i = pstatecnt; i--;) {
        sumdiff += ctr_b[i] - ctr_a[i];
    }
    
    for (int i = 0; i < pstatecnt; i++) {
        double res = (double)(ctr_b[i] - ctr_a[i]) / sumdiff;
        double state = (double)table->states[i].nominal_frequency;
        
        freq += state * res;
        
        printf("%.fMHz: %.2f%%  ", state, res * 100);
    }
    
    printf("\n\n");
    printf("Requested Package Frequency: %.2f MHz\n", freq);
    
    close(fd);

    return 0;
}
