#include "ompi_config.h"
#include "fbtl_ucx.h"

#include <unistd.h>
#include <time.h> 
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/uio.h>
#if HAVE_AIO_H
#include <aio.h>
#endif

#include "mpi.h"
#include "ompi/constants.h"
#include "ompi/mca/fbtl/fbtl.h"

struct ucx_BB {
    int orig_fd;
    int BB_fd;
    struct iovec *iov;
    int iov_count;
    off_t offset;
    char orig_filename[256];
    char BB_filename[256];
	size_t iov_total_length;
    int rank;
};

int random_num() 
{ 
    int num;
    num = (rand() % (99999999 - 10000000 + 1)) + 10000000; 
	return num;
} 

int ucx_BB_open(struct ucx_BB BB){
    char *path = NULL;
    int fd, random_int;

    path = getenv("BB_PATH");  // export BB_PATH=/home/mlaufer/test/BB_PATH/
	//path = "./BB_PATH/";
    random_int = random_num();
    sprintf(BB.BB_filename, "%srank%d_%d", path, BB.rank, random_int);
	fd = open(BB.BB_filename, O_RDWR | O_CREAT, 0666);
    return fd;
}

ssize_t ucx_bb_pwritev(int fd, struct iovec *iovec, int count, off_t offset, const char *f_filename, size_t iov_total_length, int rank) {
    ssize_t ret_code;
    struct ucx_BB BB;
	off_t total_meta_offset;
    
    BB.iov = iovec;
    BB.iov_count = count;
    BB.offset = offset;
	BB.iov_total_length = iov_total_length;
    BB.orig_fd = fd;
    BB.rank = rank;
	strcpy(BB.orig_filename, f_filename);

	// Create metadata iovec
	struct iovec meta_iov[3];
	meta_iov[0].iov_base = &BB.orig_filename;
    meta_iov[0].iov_len = sizeof(BB.orig_filename);
	meta_iov[1].iov_base = &BB.iov_total_length;
    meta_iov[1].iov_len = sizeof(size_t);
	meta_iov[2].iov_base = &BB.offset;
    meta_iov[2].iov_len = sizeof(off_t);
	total_meta_offset = sizeof(BB.orig_filename) + sizeof(size_t) + sizeof(off_t);

    BB.BB_fd = ucx_BB_open(BB); // error check
	ret_code = writev(BB.BB_fd, meta_iov, 3);
    ret_code = pwritev(BB.BB_fd, BB.iov, BB.iov_count, total_meta_offset); 
	close(BB.BB_fd);  
	return ret_code;
}

ssize_t ucx_bb_pwrite(int fd, void *buf, size_t count, off_t offset, const char *f_filename, int rank) {
	ssize_t ret_code;
    struct iovec iov[1];
	iov[0].iov_base = buf;
    iov[0].iov_len = count;
	ret_code = ucx_bb_pwritev(fd, iov, 1, offset, f_filename, count, rank);
	return ret_code;
}

ssize_t ucx_bb_writev(int fd, struct iovec *iovec, int count, const char f_filename[], size_t iov_total_length, int rank) {
	ssize_t ret_code;
    ret_code = ucx_bb_pwritev(fd, iovec, count, 0, f_filename, iov_total_length, rank); //pass 0 offset
	return ret_code;
}
