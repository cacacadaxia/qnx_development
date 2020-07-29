/*
 * ResMgr and Message Server Process
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/neutrino.h>
#include <sys/iofunc.h>
#include <sys/dispatch.h>

resmgr_connect_funcs_t  ConnectFuncs;
resmgr_io_funcs_t       IoFuncs;
iofunc_attr_t           IoFuncAttr;


typedef struct
{
    uint16_t msg_no;
    char     msg_data[255];
} server_msg_t;

//这里也有一个回调函数
int message_callback( message_context_t *ctp, int type, unsigned flags,
                      void *handle )
{
    server_msg_t *msg;
    int num;
    char msg_reply[255];

    /* Cast a pointer to the message data */
    msg = (server_msg_t *)ctp->msg;

    /* Print some useful information about the message */
    printf( "\n\nServer Got Message:\n" );
    printf( "  type: %d\n" , type );
    printf( "  data: %s\n\n", msg->msg_data );

    /* Build the reply message */
    num = type - _IO_MAX;
    snprintf( msg_reply, 254, "Server Got Message Code: _IO_MAX + %d", num );

    /* Send a reply to the waiting (blocked) client */
    MsgReply( ctp->rcvid, EOK, msg_reply, strlen( msg_reply ) + 1 );

    return 0;
}



int main( int argc, char **argv )
{
    resmgr_attr_t        resmgr_attr;
    message_attr_t       message_attr;
    dispatch_t           *dpp;
    dispatch_context_t   *ctp, *ctp_ret;
    int                  resmgr_id, message_id;

    /* Create the dispatch interface */
    dpp = dispatch_create();
    if( dpp == NULL )
    {
        fprintf( stderr, "dispatch_create() failed: %s\n",
                 strerror( errno ) );
        return EXIT_FAILURE;
    }

    memset( &resmgr_attr, 0, sizeof( resmgr_attr ) );
    resmgr_attr.nparts_max = 1;
    resmgr_attr.msg_max_size = 2048;

    /* Setup the default I/O functions to handle open/read/write/... */
    iofunc_func_init( _RESMGR_CONNECT_NFUNCS, &ConnectFuncs,
                      _RESMGR_IO_NFUNCS, &IoFuncs );

    /* Setup the attribute for the entry in the filesystem */
    iofunc_attr_init( &IoFuncAttr, S_IFNAM | 0666, 0, 0 );


    //注意这里resmgr_attach的参数“serv”，定义之后在client才能找到
    //有点像name_attach，实际上还真是
    resmgr_id = resmgr_attach( dpp, &resmgr_attr, "serv", _FTYPE_ANY,
                               0, &ConnectFuncs, &IoFuncs, &IoFuncAttr );
    if( resmgr_id == -1 )
    {
        fprintf( stderr, "resmgr_attach() failed: %s\n", strerror( errno ) );
        return EXIT_FAILURE;
    }

    /* Setup our message callback */
    memset( &message_attr, 0, sizeof( message_attr ) );
    message_attr.nparts_max = 1;
    message_attr.msg_max_size = 4096;

    /* Attach a callback (handler) for two message types */
    //_IO_MAX + 1最低  _IO_MAX + 3 最高
    message_id = message_attach( dpp, &message_attr, _IO_MAX + 1,
                                 _IO_MAX + 3, message_callback, NULL );
    //当消息为_IO_MAX + 1 或者 _IO_MAX + 2时，就会回调函数
    if( message_id == -1 )
    {
        fprintf( stderr, "message_attach() failed: %s\n", strerror( errno ) );
        return EXIT_FAILURE;
    }

    /* Setup a context for the dispatch layer to use */
    ctp = dispatch_context_alloc( dpp );
    if( ctp == NULL )
    {
        fprintf( stderr, "dispatch_context_alloc() failed: %s\n",
                 strerror( errno ) );
        return EXIT_FAILURE;
    }


    /* The "Data Pump" - get and process messages */
    while( 1 )
    {
        ctp_ret = dispatch_block( ctp );
        if( ctp_ret )
        {
            dispatch_handler( ctp );//在这里调用MessageCallback函数
        }
        else
        {
            fprintf( stderr, "dispatch_block() failed: %s\n",
                     strerror( errno ) );
            return EXIT_FAILURE;
        }
    }

    return EXIT_SUCCESS;
}
