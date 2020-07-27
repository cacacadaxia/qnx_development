/* R E S O U R C E   M A N A G E R S  - THE "NULL DEVICE" */

/* Project Name: "example_resmgr_devnull" */

/* What is a Resource Manager under QNX Neutrino?
 * A resource manager is a superset of a device driver. The QNX
 * resource manager framework is used to create the POSIX
 * interface (open, read, write, etc.) for any resource you
 * can think of. Imagine you are coding a device driver for
 * a device that reads credit cards. Your application would then
 * be able to just use open() to access the card, read() to retrieve
 * data from the card, and write() to store data on the card.
 * If later your hardware changes, and you don't need to read
 * credit cards any more but are reading serial data from some
 * field bus instead, you replace/update the resource manager
 * while your application still just uses open(), read() and write()!
 */

/*
 * This example contains a ready-to-run resource manager. It
 * represents the "null device": Everything works - you can
 * read from it and get zero bytes. And you can write to it,
 * and the write succeeds - the data is written into nowhere.
 * (Note that our device is called /dev/Null instead of
 * /dev/null; this is because /dev/null already exists and is
 * part of Neutrino).
 *
 * Start it as:
 *    example_resmgr_devnull -v &
 * If your target is running Photon, open a separate terminal
 * window for the following tests. Else all will happen on
 * your console.
 * Test the new device by trying:
 *   cat /dev/Null
 * (You will read from the device, and cat will
 * return at once because 0 bytes are returned from this resmgr).
 *
 *   echo hallo > /dev/Null
 * (You will write to the device; it
 * will work and the string will be printed on the resmgr side).
 * Sending whole files into this null device will also work;
 * to make this work, the more complex code in the write handler
 * is actually needed.
 *
 */

/* More information:
 * Please read the Writing a Resource Manager guide or the
 * Resource Managers chapter in Getting Started with QNX Neutrino.
 */


#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/iofunc.h>
#include <sys/dispatch.h>
#include <sys/neutrino.h>
#include <sys/resmgr.h>

void options (int argc, char **argv);

/* A resource manager mainly consists of callbacks for POSIX
 * functions a client could call. In the example, we have
 * callbacks for the open(), read() and write() calls. More are
 * possible. If we don't supply own functions (e.g. for stat(),
 * seek(), etc.), the resource manager framework will use default
 * system functions, which in most cases return with an error
 * code to indicate that this resource manager doesn't support
 * this function.*/

/* These prototypes are needed since we are using their names
 * in main(). */

int io_open (resmgr_context_t *ctp, io_open_t  *msg, RESMGR_HANDLE_T *handle, void *extra);
int io_read (resmgr_context_t *ctp, io_read_t  *msg, RESMGR_OCB_T *ocb);
int io_write(resmgr_context_t *ctp, io_write_t *msg, RESMGR_OCB_T *ocb);

/*
 * Our connect and I/O functions - we supply two tables
 * which will be filled with pointers to callback functions
 * for each POSIX function. The connect functions are all
 * functions that take a path, e.g. open(), while the I/O
 * functions are those functions that are used with a file
 * descriptor (fd), e.g. read().
 */

resmgr_connect_funcs_t  connect_funcs;
resmgr_io_funcs_t       io_funcs;

/*
 * Our dispatch, resource manager, and iofunc variables
 * are declared here. These are some small administrative things
 * for our resource manager.
 */

dispatch_t              *dpp;
resmgr_attr_t           rattr;
dispatch_context_t      *ctp;
iofunc_attr_t           ioattr;

char    *progname = "null";
int     optv;                               // -v for verbose operation

int main (int argc, char **argv)
{
	int pathID;

	printf ("%s:  starting...\n", progname);

	/* Check for command line options (-v) */
	options (argc, argv);

	/* Allocate and initialize a dispatch structure for use
	 * by our main loop. This is for the resource manager
	 * framework to use. It will receive messages for us,
	 * analyze the message type integer and call the matching
	 * handler callback function (i.e. io_open, io_read, etc.) */
	dpp = dispatch_create ();
	if (dpp == NULL) {
		fprintf (stderr, "%s:  couldn't dispatch_create: %s\n",
				progname, strerror (errno));
		exit (1);
	}

	/* Set up the resource manager attributes structure. We'll
	 * use this as a way of passing information to
	 * resmgr_attach(). The attributes are used to specify
	 * the maximum message length to be received at once,
	 * and the number of message fragments (iov's) that
	 * are possible for the reply.
	 * For now, we'll just use defaults by setting the
	 * attribute structure to zeroes. */
	memset (&rattr, 0, sizeof (rattr));

	/* Now, let's intialize the tables of connect functions and
	 * I/O functions to their defaults (system fallback
	 * routines) and then override the defaults with the
	 * functions that we are providing. */
	iofunc_func_init (_RESMGR_CONNECT_NFUNCS, &connect_funcs,
			_RESMGR_IO_NFUNCS, &io_funcs);
	/* Now we override the default function pointers with
	 * some of our own coded functions: */
	connect_funcs.open = io_open;
	io_funcs.read = io_read;
	io_funcs.write = io_write;

	/* Initialize the device attributes for the particular
	 * device name we are going to register. It consists of
	 * permissions, type of device, owner and group ID */
	iofunc_attr_init (&ioattr, S_IFCHR | 0666, NULL, NULL);

	/* Next we call resmgr_attach() to register our device name
	 * with the process manager, and also to let it know about
	 * our connect and I/O functions. */
	pathID = resmgr_attach (dpp, &rattr, "/dev/Null",
			_FTYPE_ANY, 0, &connect_funcs, &io_funcs, &ioattr);
	if (pathID == -1) {
		fprintf (stderr, "%s:  couldn't attach pathname: %s\n",
				progname, strerror (errno));
		exit (1);
	}

	/* Now we allocate some memory for the dispatch context
	 * structure, which will later be used when we receive
	 * messages. */
	ctp = dispatch_context_alloc (dpp);

	/* Done! We can now go into our "receive loop" and wait
	 * for messages. The dispatch_block() function is calling
	 * MsgReceive() under the covers, and receives for us.
	 * The dispatch_handler() function analyzes the message
	 * for us and calls the appropriate callback function. */
	while (1) {
		if ((ctp = dispatch_block (ctp)) == NULL) {
			fprintf (stderr, "%s:  dispatch_block failed: %s\n",
					progname, strerror (errno));
			exit (1);
		}
		/* Call the correct callback function for the message
		 * received. This is a single-threaded resource manager,
		 * so the next request will be handled only when this
		 * call returns. Consult our documentation if you want
		 * to create a multi-threaded resource manager. */
		dispatch_handler (ctp);
	}
}

/*
 *  io_open
 *
 * We are called here when the client does an open().
 * In this simple example, we just call the default routine
 * (which would be called anyway if we did not supply our own
 * callback), which creates an OCB (Open Context Block) for us.
 * In more complex resource managers, you will want to check if
 * the hardware is available, for example.
 */

	int
io_open (resmgr_context_t *ctp, io_open_t *msg, RESMGR_HANDLE_T *handle, void *extra)
{
	if (optv) {
		printf ("%s:  in io_open\n", progname);
	}

	return (iofunc_open_default (ctp, msg, handle, extra));
}

/*
 *  io_read
 *
 *  At this point, the client has called the library read()
 *  function, and expects zero or more bytes.  Since this is
 *  the /dev/Null resource manager, we return zero bytes to
 *  indicate EOF -- no more bytes expected.
 */

/* The message that we received can be accessed via the
 * pointer *msg. A pointer to the OCB that belongs to this
 * read is the *ocb. The *ctp pointer points to a context
 * structure that is used by the resource manager framework
 * to determine whom to reply to, and more. */
	int
io_read (resmgr_context_t *ctp, io_read_t *msg, RESMGR_OCB_T *ocb)
{
	int status;

	if (optv) {
		printf ("%s:  in io_read\n", progname);
	}

	/* Here we verify if the client has the access
	 * rights needed to read from our device */
	if ((status = iofunc_read_verify(ctp, msg, ocb, NULL)) != EOK) {
		return (status);
	}

	/* We check if our read callback was called because of
	 * a pread() or a normal read() call. If pread(), we return
	 * with an error code indicating that we don't support it.*/
	if (msg->i.xtype & _IO_XTYPE_MASK != _IO_XTYPE_NONE) {
		return (ENOSYS);
	}
	/* Here we set the number of bytes we will return. As we
	 * are the null device, we will return 0 bytes. Normally,
	 * here you would write the number of bytes you
	 * are actually returning. */
	_IO_SET_READ_NBYTES(ctp, 0);

	/* The next line (commented) is used to tell the system how
	 * large your buffer is in which you want to return your
	 * data for the read() call. We don't set it here because
	 * as a null device, we return 0 data. See the "time
	 * resource manager" example for an actual use.
	 */
	//	SETIOV( ctp->iov, buf, sizeof(buf));

	if (msg->i.nbytes > 0) {
		ocb->attr->flags |= IOFUNC_ATTR_ATIME;
	}

	/* We return 0 parts, because we are the null device.
	 * Normally, if you return actual data, you would return at
	 * least 1 part. A pointer to and a buffer length for 1 part
	 * are located in the ctp structure.  */
	return (_RESMGR_NPARTS (0));
	/* What you return could also be several chunks of
	 * data. In this case, you would return more
	 * than 1 part. See the "time resource manager" example
	 * on how to use this. */

}

/*
 *  io_write
 *
 *  At this point, the client has called the library write()
 *  function, and expects that our resource manager will write
 *  the number of bytes that have been specified to the device.
 *
 *  Since this is /dev/Null, all of the clients' writes always
 *  work -- they just go into Deep Outer Space.
 */

	int
io_write (resmgr_context_t *ctp, io_write_t *msg, RESMGR_OCB_T *ocb)
{
	int status;
	char *buf;

	if (optv) {
		printf ("%s:  in io_write\n", progname);
	}

	/* Check the access permissions of the client */
	if ((status = iofunc_write_verify(ctp, msg, ocb, NULL)) != EOK) {
		return (status);
	}

	/* Check if pwrite() or normal write() */
	if (msg->i.xtype & _IO_XTYPE_MASK != _IO_XTYPE_NONE) {
		return (ENOSYS);
	}

	/* Set the number of bytes successfully written for
	 * the client. This information will be passed to the
	 * client by the resource manager framework upon reply.
	 * In this example, we just take the number of  bytes that
	 * were sent to us and claim we successfully wrote them. */
	_IO_SET_WRITE_NBYTES (ctp, msg -> i.nbytes);
	printf("got write of %d bytes, data:\n", msg->i.nbytes);

	/* Here we print the data. This is a good example for the case
	 * where you actually would like to do something with the data.
	 */
	/* First check if our message buffer was large enough
	 * to receive the whole write at once. If yes, print data.*/
	if( (msg->i.nbytes <= ctp->info.msglen - ctp->offset - sizeof(msg->i)) &&
			(ctp->info.msglen < ctp->msg_max_size))  { // space for NUL byte
		buf = (char *)(msg+1);
		buf[msg->i.nbytes] = '\0';
		printf("%s\n", buf );
	} else {
		/* If we did not receive the whole message because the
		 * client wanted to send more than we could receive, we
		 * allocate memory for all the data and use resmgr_msgread()
		 * to read all the data at once. Although we did not receive
		 * the data completely first, because our buffer was not big
		 * enough, the data is still fully available on the client
		 * side, because its write() call blocks until we return
		 * from this callback! */
		buf = malloc( msg->i.nbytes + 1);
		resmgr_msgread( ctp, buf, msg->i.nbytes, sizeof(msg->i));
		buf[msg->i.nbytes] = '\0';
		printf("%s\n", buf );
		free(buf);
	}

	/* Finally, if we received more than 0 bytes, we mark the
	 * file information for the device to be updated:
	 * modification time and change of file status time. To
	 * avoid constant update of the real file status information
	 * (which would involve overhead getting the current time), we
	 * just set these flags. The actual update is done upon
	 * closing, which is valid according to POSIX. */
	if (msg->i.nbytes > 0) {
		ocb->attr->flags |= IOFUNC_ATTR_MTIME | IOFUNC_ATTR_CTIME;
	}

	return (_RESMGR_NPARTS (0));
}

/* Why we don't have any close callback? Because the default
 * function, iofunc_close_ocb_default(), does all we need in this
 * case: Free the ocb, update the time stamps etc. See the docs
 * for more info.
 */


/*
 *  options
 *
 *  This routine handles the command-line options.
 *  For our simple /dev/Null, we support:
 *      -v      verbose operation
 */

	void
options (int argc, char **argv)
{
	int     opt;
	int     i;

	optv = 0;

	i = 0;
	while (optind < argc) {
		while ((opt = getopt (argc, argv, "v")) != -1) {
			switch (opt) {
				case 'v':
					optv = 1;
					break;
			}
		}
	}
}
