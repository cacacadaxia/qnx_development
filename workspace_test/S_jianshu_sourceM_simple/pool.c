//#include <errno.h>
//#include <stdio.h>
//#include <stddef.h>
//#include <stdlib.h>
//#include <unistd.h>
//
///*
// * Define THREAD_POOL_PARAM_T such that we can avoid a compiler
// * warning when we use the dispatch_*() functions below
// */
//#define THREAD_POOL_PARAM_T dispatch_context_t
//
//#include <sys/iofunc.h>
//#include <sys/dispatch.h>
//
//static resmgr_connect_funcs_t    connect_funcs;
//static resmgr_io_funcs_t         io_funcs;
//static iofunc_attr_t             attr;
//
//main(int argc, char **argv)
//{
//    /* declare variables we'll be using */
//    thread_pool_attr_t   pool_attr;
//    resmgr_attr_t        resmgr_attr;
//    dispatch_t           *dpp;
//    thread_pool_t        *tpp;
//    dispatch_context_t   *ctp;
//    int                  id;
//
//    /* initialize dispatch interface */
//    if((dpp = dispatch_create()) == NULL) {
//        fprintf(stderr, "%s: Unable to allocate dispatch handle.\n",
//                argv[0]);
//        return EXIT_FAILURE;
//    }
//
//    /* initialize resource manager attributes */
//    memset(&resmgr_attr, 0, sizeof resmgr_attr);
//    resmgr_attr.nparts_max = 1;
//    resmgr_attr.msg_max_size = 2048;
//
//    /* initialize functions for handling messages */
//    iofunc_func_init(_RESMGR_CONNECT_NFUNCS, &connect_funcs,
//                     _RESMGR_IO_NFUNCS, &io_funcs);
//
//    /* initialize attribute structure used by the device */
//    iofunc_attr_init(&attr, S_IFNAM | 0666, 0, 0);
//
//    /* attach our device name */
//    id = resmgr_attach(dpp,            /* dispatch handle        */
//                       &resmgr_attr,   /* resource manager attrs */
//                       "/dev/sample",  /* device name            */
//                       _FTYPE_ANY,     /* open type              */
//                       0,              /* flags                  */
//                       &connect_funcs, /* connect routines       */
//                       &io_funcs,      /* I/O routines           */
//                       &attr);         /* handle                 */
//    if(id == -1) {
//        fprintf(stderr, "%s: Unable to attach name.\n", argv[0]);
//        return EXIT_FAILURE;
//    }
//
//    /* initialize thread pool attributes */
//    memset(&pool_attr, 0, sizeof pool_attr);
//    pool_attr.handle = dpp;
//    pool_attr.context_alloc = dispatch_context_alloc;
//    pool_attr.block_func = dispatch_block;
//    pool_attr.unblock_func = dispatch_unblock;
//    pool_attr.handler_func = dispatch_handler;
//    pool_attr.context_free = dispatch_context_free;
//    pool_attr.lo_water = 2;
//    pool_attr.hi_water = 4;
//    pool_attr.increment = 1;
//    pool_attr.maximum = 50;
//
//    /* allocate a thread pool handle */
//    if((tpp = thread_pool_create(&pool_attr,
//                                 POOL_FLAG_EXIT_SELF)) == NULL) {
//        fprintf(stderr, "%s: Unable to initialize thread pool.\n",
//                argv[0]);
//        return EXIT_FAILURE;
//    }
//
//    /* start the threads; will not return */
//    thread_pool_start(tpp);
//}
