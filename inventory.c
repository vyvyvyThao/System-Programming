#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>

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

int main() {
    struct data *product;

    //access shared memory
    int flags = O_RDWR;
    mode_t permission = S_IRUSR |S_IWUSR;
    off_t size = sizeof(struct data); 
    int fd = shm_open(SHARED_MEMORY_NAME, flags, permission);

    struct item items_arr[10];

    // set size 
    if (ftruncate(fd, size) != 0) {
        printf("%d\n", ftruncate(fd, size));
		printf("ftruncate failed... not much we can do... exitting.\n");
		exit(1);
	}

    // map the SHM
    product = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if (product == MAP_FAILED) {
		printf("mmap failed...not much we can do... exiting.\n");
		exit(1);
	}

    // infinite loop for 2 options: (1) display (2) exit
    // edit here ==> array
    int option;
    while (1) 
    {
        printf("Enter an option:\n1: display current inventory\n2: exit\n");
        scanf("%d", &option);
        if (option == 1) {
            int i;
            for (i = 0; i < product->slots; i++) {
                printf("%d. Item name: %s\n", i+1, product->arr[i].name);
                printf("   Quantity: %d\n", product->arr[i].quantity);
            }
    
        } else if (option == 2) {
            exit(0);
        }
    }
}