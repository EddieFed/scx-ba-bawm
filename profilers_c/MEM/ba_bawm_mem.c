#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <bpf/libbpf.h>
#include "ba_bawm_mem.skel.h"

static volatile sig_atomic_t exiting = 0;

void handle_signal(int sig) {
    exiting = 1;
}

int main(int argc, char **argv) {

    struct ba_bawm_mem_skel_bpf *skel;
    int err;

    signal(SIGINT, handle_signal);
    signal(SIGTERM, handle_signal);

    libbpf_set_strict_mode(LIBBPF_STRICT_ALL);

    skel = ba_bawm_mem_skel_bpf__open_and_load();
    if (!skel) {
        fprintf(stderr, "Failed to open and load BPF skeleton\n");
        return 1;
    }

    err = ba_bawm_mem_skel_bpf__attach(skel);
    if (err) {
        fprintf(stderr, "Failed to attach BPF program\n");
        goto cleanup;
    }

    printf("Program loaded and attached. Press Ctrl-C to exit.\n");

    while (!exiting) {
        sleep(1);
    }

    cleanup:
        ba_bawm_mem_skel_bpf__destroy(skel);
        return err < 0 ? -err : 0;
}