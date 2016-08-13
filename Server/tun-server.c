#include<stdio.h>
#include<string.h>    //strlen
#include<sys/socket.h>
#include<arpa/inet.h> //inet_addr
#include<unistd.h>    //write

#include <fcntl.h>  /* O_RDWR */
#include <stdlib.h> /* exit(), malloc(), free() */
#include <sys/ioctl.h> /* ioctl() */

/* includes for struct ifreq, etc */
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#include <linux/if.h>
#include <linux/if_tun.h>

#define ARR_SIZE  100
#define ICMP_SIZE  84

int tun_open(char *devname)
{
    struct ifreq ifr;
    int fd, err;

    if ( (fd = open("/dev/net/tun", O_RDWR)) == -1 ) {
        perror("open /dev/net/tun");
        exit(1);
    }

    memset(&ifr, 0, sizeof(ifr));
    ifr.ifr_flags = (IFF_TUN | IFF_NO_PI);
    strncpy(ifr.ifr_name, devname, IFNAMSIZ);

    /* ioctl will use if_name as the name of TUN
    * interface to open: "tun0", etc. */
    if ( (err = ioctl(fd, TUNSETIFF, (void *) &ifr)) == -1 ) {
        perror("ioctl TUNSETIFF");close(fd);exit(1);
    }

    /* After the ioctl call the fd is "connected" to tun device
    * specified
    * by devname */

    return fd;
}

 
int main(int argc , char *argv[])
{
    int fd, nbytes;
    char *buf;
    char c;
    int shmid;
    key_t key = 5678;


    if ((shmid = shmget(key, ARR_SIZE, IPC_CREAT | 0666)) < 0) {
        perror("shmget");
        exit(1);
    }

    if ((buf = shmat(shmid, NULL, 0)) == (char *) -1) {
        perror("shmat");
        exit(1);
    }

    // opening first TUN interface
    fd = tun_open("asa0") ;
    printf("Device asa1 opened\n");

    while(1)
    {
	while(*(buf+ICMP_SIZE) != 1);
    	// writing to the TUN file descriptor
    	nbytes = write(fd, buf, ARR_SIZE);
//        printf("Wrote %d bytes to TUN file descriptor\n", nbytes);

	nbytes = 0;

    	// read from the TUN file descriptor
	nbytes = read(fd, buf, ARR_SIZE);
        printf("Read %d bytes from asa1\n", nbytes);


	*(buf+ICMP_SIZE) = '2';
    }
    return 0;
}
