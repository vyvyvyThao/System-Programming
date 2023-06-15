#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <signal.h>

#define SHARED_MEMORY_NAME "ITEMS"
#define FALSE 0 
#define TRUE 1

struct item {
    char name[32];
    int quantity;
};

struct data 
{
    struct item arr[10];
    int slots; 
};

// create an array of 10 structs containing item name and quantity
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
    // create shared memory 
    // shm_open
    int flags = O_RDWR | O_CREAT;
    mode_t permission = S_IRUSR |S_IWUSR;

    int SHM_creator_status = TRUE;

	// Create the shared memory. If already exist, connect to it
	printf("Trying to create shared memory.\n"); 
    fd = shm_open(SHARED_MEMORY_NAME, flags, permission);

    // error handling if failed to open
	if (fd == -1) {
		printf("shm_open failed\n");
		if (errno == EEXIST){
			SHM_creator_status = FALSE;
			printf("SHM already exist...trying to connect to it now\n");
			fd = shm_open(SHARED_MEMORY_NAME, flags, permission);
			if (fd == -1) {
				printf("failed to open shared memory...something's wrong...exiting.\n");
				exit(1);
			}
		} else {
			printf("something's wrong...exiting.\n");
			exit(1);
		}
	}

    // set size 
    if (ftruncate(fd, size) != 0) {
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

    // while the slot is empty, ask the user for name and quantity
    int i;
    for (i = 0; i < 10; i++)
    {   
        if (*products->arr[i].name == '\0') {
            printf("Enter item name: ");
            scanf(" %s", products->arr[i].name); 

            printf("Enter quantity to produce: ");
            scanf("%d", &products->arr[i].quantity);
            products->slots ++;
        }      
    }
}