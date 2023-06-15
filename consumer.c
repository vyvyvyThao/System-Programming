#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <string.h>
#include <signal.h>

#define SHARED_MEMORY_NAME "ITEMS"

struct item {
    char name[32];
    int quantity;
};

struct data 
{
    struct item arr[10];
    int slots; 
};

struct data *products;
off_t size = sizeof(struct data);
int fd;

//ctrl-c: clean up the shared memory  
void sigint_handler(int sig) {
    // set the number of slots back to 0
    products->slots = 0;

    // unmap
    int status = munmap(products, size);

    // close the shared memory
    status = close(fd);
    if (status != 0) {
        printf("Couldn't close the shared memory...not much we can do about that.\n");
    } 

    // delete
    status = shm_unlink(SHARED_MEMORY_NAME);
	if (status != 0) {
		printf("couldn't delete the shared memory... not much we can do about that.\n");
	}
    exit(0);
}

int main() {
    //access shared memory
    int flags = O_RDWR;
    mode_t permission = S_IRUSR |S_IWUSR;
    fd = shm_open(SHARED_MEMORY_NAME, flags, permission);

    if (fd == -1) {
		printf("shm_open failed\n");
    }

    // set size 
    if (ftruncate(fd, size) != 0) {
        printf("%d\n", ftruncate(fd, size));
		printf("ftruncate failed... not much we can do... exitting.\n");
		exit(1);
	}

    // map the SHM
    products = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if (products == MAP_FAILED) {
		printf("mmap failed...not much we can do... exiting.\n");
		exit(1);
	}

    // handling ctrl-c 
    if (signal(SIGINT, sigint_handler) == SIG_ERR) {
        	exit(1);
	}

    // check if item is available
    int i;
    while (1) {
        if (products->slots == 0)
        {
            sleep(30);
        } else {
            for (int i = 0; i < 10; i++) {
                if (products->arr[i].quantity > 0) {
                    printf("Customer has taken %s out.\n", products->arr[i].name);
                    *products->arr[i].name = '\0';
                    products->arr[i].quantity = 0;
                    printf("Boxing item up and delivering...\n");
                    sleep(30);
                    printf("Delivered.\n");
                    products->slots -= 1;
                }
            }
        }
    }
}