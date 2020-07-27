#include <errno.h>
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/iofunc.h>
#include <sys/dispatch.h>
#include <process.h>

void* Server_function(void* notuse);
int io_read (resmgr_context_t *ctp, io_read_t *msg, RESMGR_OCB_T *ocb);
int io_write (resmgr_context_t *ctp, io_write_t *msg, RESMGR_OCB_T *ocb);

int ResourceManager_buffer[1024];

int main(int argc, char *argv[]) {
	printf("Server process is ready, pid = %d\n", getpid());

	pthread_attr_t attr;
	int Thread_ID, result = 0;
	int i = 0;
	for(i=0; i < sizeof(ResourceManager_buffer)/sizeof(int); i++){
		ResourceManager_buffer[i] = i + 1;
	}

	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
	pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);
	pthread_attr_setschedpolicy(&attr, SCHED_FIFO);
	attr.__param.__sched_priority = 21;
	pthread_create(&Thread_ID, &attr, Server_function, NULL);

	sleep(3);

	result = spawnl(P_NOWAIT, "/tmp/Z__ResourseManager_Client", "Z__Client_NameOpen", NULL);
	printf("result = %d\n", result);

	pthread_join(Thread_ID, NULL);

	return EXIT_SUCCESS;
}

void* Server_function(void* notuse){
	resmgr_connect_funcs_t 	connect_funcs;
	resmgr_io_funcs_t		io_funcs;
	iofunc_attr_t			attr;
	resmgr_attr_t        	resmgr_attr;
	dispatch_t           	*dpp;
	dispatch_context_t   	*ctp;
	int                  	id;

	if((dpp = dispatch_create()) == NULL){
		printf("Unable to allocate dispatch handle\n");
		return NULL;
	}

	memset(&resmgr_attr, 0, sizeof(resmgr_attr));
	resmgr_attr.nparts_max = 1;
	resmgr_attr.msg_max_size = 2048;

	iofunc_func_init(_RESMGR_CONNECT_NFUNCS, &connect_funcs, _RESMGR_IO_NFUNCS, &io_funcs);
	io_funcs.read = io_read;
	io_funcs.write = io_write;

	iofunc_attr_init(&attr, S_IFNAM | 0666, 0, 0);
	attr.nbytes = sizeof(ResourceManager_buffer);

	id = resmgr_attach(dpp, &resmgr_attr, "dev/sample", _FTYPE_ANY, 0, &connect_funcs, &io_funcs, &attr);
	if(id == -1){
		printf("Unable to attach name\n");
		return NULL;
	}
	ctp = dispatch_context_alloc(dpp);

	while(1){
		if((ctp = dispatch_block(ctp)) == NULL){
			printf("block error\n");
			return NULL;
		}

		dispatch_handler(ctp);
	}
}

int io_read (resmgr_context_t *ctp, io_read_t *msg, RESMGR_OCB_T *ocb){
	int 	nleft;
	int 	nbytes;
	int 	nparts;
	int 	status;

	if((status = iofunc_read_verify(ctp, msg, ocb, NULL)) != EOK){
		return (status);
	}

	if((msg->i.xtype & _IO_XTYPE_MASK) != _IO_XTYPE_NONE){
		return (ENOSYS);
	}

	nleft = ocb->attr->nbytes - ocb->offset;
	nbytes = min(msg->i.nbytes, nleft);

	if(nbytes > 0){
		SETIOV(ctp->iov, ((void *)ResourceManager_buffer) + ocb->offset, nbytes);

		_IO_SET_READ_NBYTES(ctp, nbytes);

		ocb->offset += nbytes;

		nparts = 1;
	}else{
		_IO_SET_READ_NBYTES(ctp, 0);

		nparts = 0;
	}

	if(msg->i.nbytes > 0)
		ocb->attr->flags |= IOFUNC_ATTR_ATIME;

	return (_RESMGR_NPARTS(nparts));
}

int io_write (resmgr_context_t *ctp, io_write_t *msg, RESMGR_OCB_T *ocb){
	int status;
	int* buff;

	if((status = iofunc_write_verify(ctp, msg, ocb, NULL)) != EOK){
		return (status);
	}

	if((msg->i.xtype & _IO_XTYPE_MASK) != _IO_XTYPE_NONE){
		return (ENOSYS);
	}

	_IO_SET_WRITE_NBYTES(ctp, msg->i.nbytes);

	buff = (int*)malloc(msg->i.nbytes);

	if(buff == NULL)
		return ENOMEM;

	resmgr_msgread(ctp, buff, msg->i.nbytes, sizeof(msg->i));

	memcpy((void*)ResourceManager_buffer + ocb->offset, (void*)buff, msg->i.nbytes);

	printf("Received %d bytes data is %d\n", msg->i.nbytes, *((int *)((void*)ResourceManager_buffer + ocb->offset)));

	free(buff);

	if (msg->i.nbytes > 0)
		ocb->attr->flags |= IOFUNC_ATTR_MTIME | IOFUNC_ATTR_CTIME;

	return (_RESMGR_NPARTS (0));
}


