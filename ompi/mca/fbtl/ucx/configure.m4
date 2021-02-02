# -*- shell-script -*-
#
# Copyright (c) 2004-2005 The Trustees of Indiana University and Indiana
#                         University Research and Technology
#                         Corporation.  All rights reserved.
# Copyright (c) 2004-2005 The University of Tennessee and The University
#                         of Tennessee Research Foundation.  All rights
#                         reserved.
# Copyright (c) 2004-2005 High Performance Computing Center Stuttgart,
#                         University of Stuttgart.  All rights reserved.
# Copyright (c) 2004-2012 The Regents of the University of California.
#                         All rights reserved.
# Copyright (c) 2010-2014 Cisco Systems, Inc.  All rights reserved.
# Copyright (c) 2008-2011 University of Houston. All rights reserved.
# $COPYRIGHT$
#
# Additional copyrights may follow
#
# $HEADER$
#

# MCA_fbtl_ucx_CONFIG(action-if-can-compile,
#                        [action-if-cant-compile])
# ------------------------------------------------
AC_DEFUN([MCA_ompi_fbtl_ucx_CONFIG],[
    AC_CONFIG_FILES([ompi/mca/fbtl/ucx/Makefile])

    fbtl_ucx_happy=no
    AC_CHECK_HEADER([aio.h],
                    [dnl NetBSD has aio_* in -lrt, vs. the usual libc
                     OPAL_SEARCH_LIBS_COMPONENT([fbtl_ucx],
                                    [aio_write], [rt],
                                    [fbtl_ucx_happy="yes"])])

    AC_CHECK_FUNCS([pwritev],[],[])
    AC_CHECK_FUNCS([preadv],[],[])

    AS_IF([test "$fbtl_ucx_happy" = "yes"],
          [$1],
          [$2])
])dnl
