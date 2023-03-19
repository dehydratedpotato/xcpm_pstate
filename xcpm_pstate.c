//
//  xcpm_pstate.c
//  xcpm_pstate
//
//  Created by BitesPotatoBacks on 11/23/22.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/fcntl.h>

#define XCPM_GET_PSTATE_CTRS 0x2000580D
#define XCPM_GET_PSTATE_TABLE 0x20005801

typedef struct {
    uint32_t pstate_id;
    uint32_t nominal_frequency;
    uint32_t unknown;
    uint32_t reserved;
} pstate_t;

typedef struct {
    uint32_t high_frequency_modes_turbo[2];
    uint32_t high_frequency_modes_nonturbo[2];
    uint32_t max_efficent_frequency;
    uint32_t state_count;

    pstate_t states[64];
} pstate_table_t;

int main(int argc, const char * argv[]) {
    if (getuid() != 0) {
        printf("This tool requires root\n");
        return -1;
    }
    
    uint64_t tableptr, ctrptr_a, ctrptr_b = 0;
        
    // open xcpm
    int32_t fd = open("/dev/xcpm", 0);
    if(fd == -1) perror("read");
    
    // get pstate table
    ioctl(fd, XCPM_GET_PSTATE_TABLE, &tableptr, 0);
    pstate_table_t* table = (pstate_table_t *)malloc(sizeof(pstate_table_t));
    memcpy(table, &tableptr, sizeof(pstate_table_t));
    
    uint32_t count = table->state_count;
    
    printf("Max Efficient State: %u MHz\n", table->max_efficent_frequency);
    printf("Non-Turbo States: %u-%u MHz\n", table->high_frequency_modes_nonturbo[0], table->high_frequency_modes_nonturbo[1]);
    printf("Turbo States: %u-%u MHz\n\n", table->high_frequency_modes_turbo[0], table->high_frequency_modes_turbo[1]);

    // sample ctrs
    ioctl(fd, XCPM_GET_PSTATE_CTRS, &ctrptr_a, 0);
    uint64_t counters_a[count];
    memcpy(counters_a, &ctrptr_a, sizeof(uint64_t) * count);

    sleep(1); // update interval
    
    ioctl(fd, XCPM_GET_PSTATE_CTRS, &ctrptr_b, 0);
    uint64_t counters_b[count];
    memcpy(counters_b, &ctrptr_b, sizeof(uint64_t) * count);

    // format to distribution
    
    double sumdiff = 0;

    for (int i = 0; i < count; i++) {
        sumdiff += counters_b[i] - counters_a[i];
    }
    
    double freq = 0;
    
    for (int i = 0; i < count; i++) {
        double res = (double)(counters_b[i] - counters_a[i]) / sumdiff;
        double state = (double)table->states[i].nominal_frequency;
        
        freq += state * res;
        
        printf("%.fMHz: %.2f%%  ", state, res * 100);
    }
    
    printf("\n\n");
    printf("Package Frequency: %.2f MHz\n", freq);
    
    close(fd);

    return 0;
}
