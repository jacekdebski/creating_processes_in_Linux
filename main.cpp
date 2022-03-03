#include <unistd.h>
#include <iostream>
#include <signal.h>
#include <sched.h>
#include <string.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <errno.h>

#define STACK_SIZE 1024*1024

int foo(void *argv) {
    std::cout << "PID in foo (child process): " << getpid() << ", (char*)argv: " << (char *) argv << std::endl;
    exit(EXIT_SUCCESS);
}

void clone_func(int argc, char **argv) {
    for (int i = 1; i < argc; i++) {
		int clone_res = 0;
        void *ptr_stack = mmap(NULL, STACK_SIZE, PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS | MAP_STACK, -1,
                               0);
		if(ptr_stack == MAP_FAILED){
			std::cout << "Error " << errno << " occured in mmap function" << std::endl;
        	exit(EXIT_FAILURE);
		}
		clone_res = clone(foo, (char *) ptr_stack + STACK_SIZE, SIGCHLD, *(argv + i));
		if (clone_res == -1){
			std::cout << "Error " << errno << " occured in clone function" << std::endl;
        	exit(EXIT_FAILURE);
		}
    }
}

void fork_func(int argc, char **argv) {
    for (int i = 1; i < argc; i++) {
        pid_t pid;
        pid = fork();
		if (pid == -1){
			std::cout << "Error " << errno << " occured in fork function" << std::endl;
        	exit(EXIT_FAILURE);
		}
        if (pid == 0) {
			std::cout << "Child " << i << " PID: " << getpid() << ", *(argv + " << i << "): " << *(argv + i) << std::endl;
            exit(EXIT_SUCCESS);
        }
    }
}

int main(int argc, char **argv) {
    std::cout << "Start main" << std::endl;
    std::cout << "Parent PID: " << getpid() << std::endl;
	if (argc < 2) {
        std::cout << "No arguments" << std::endl;
        exit(EXIT_SUCCESS);
    } else {
        if (strcmp(*(argv + 1), "c") == 0) {
            clone_func(argc, argv);
        } else {
            fork_func(argc, argv);
        }
        wait(NULL);
    }
	std::cout << "End main" << std::endl;
    exit(EXIT_SUCCESS);
}