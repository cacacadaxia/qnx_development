/*
 * Message Client Process
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <sys/neutrino.h>
#include <sys/iofunc.h>
#include <sys/dispatch.h>

typedef struct
{
    uint16_t msg_no;
    char msg_data[255];
} client_msg_t;

int main( int argc, char **argv )
{
    int fd;
    int c;
    client_msg_t msg;
    int ret;
    int num;
    char msg_reply[255];

    num = 3;

    /* Process any command line arguments */
    while( ( c = getopt( argc, argv, "n:" ) ) != -1 )
    {
        if( c == 'n' )
        {
            num = strtol( optarg, 0, 0 );
        }
    }
    /* Open a connection to the server (fd == coid) */
    fd = open( "serv", O_RDWR );
    if( fd == -1 )
    {
        fprintf( stderr, "Unable to open server connection: %s\n",
            strerror( errno ) );
        return EXIT_FAILURE;
    }

    /* Clear the memory for the msg and the reply */
    //其实不要这个也行
    memset( &msg, 0, sizeof( msg ) );
    memset( &msg_reply, 0, sizeof( msg_reply ) );

    /* Set up the message data to send to the server */
    msg.msg_no = _IO_MAX + num;
    snprintf( msg.msg_data, 254, "client %d requesting reply.", getpid() );

    printf( "client: msg_no: _IO_MAX + %d\n", num );
    fflush( stdout );

    /* Send the data to the server and get a reply */
    ret = MsgSend( fd, &msg, sizeof( msg ), msg_reply, 255 );
    if( ret == -1 )
    {
        fprintf( stderr, "Unable to MsgSend() to server: %s\n", strerror( errno ) );
        return EXIT_FAILURE;
    }

    /* Print out the reply data */
    printf( "client: server replied: %s\n", msg_reply );

    close( fd );

    return EXIT_SUCCESS;
}
