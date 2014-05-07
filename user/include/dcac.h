#ifndef __DCAC_H
#define __DCAC_H

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/fcntl.h>
#include <unistd.h>

#define DCAC_ADDONLY 4
#define DCAC_ADDMOD  5

#define DCAC_FLAGS_CAN_ADD(flags) ((flags & 7) == 4 || (flags & 7) == 5)
#define DCAC_FLAGS_CAN_MOD(flags) ((flags & 7) == 5)

#define DCAC_OP_ADD_UNAME  0
#define DCAC_OP_ADD_GNAME  1
#define DCAC_OP_ADD_ANY    2

#define DCAC_OP_SET_DEF_RDACL  20
#define DCAC_OP_SET_DEF_WRACL  21
#define DCAC_OP_SET_DEF_EXACL  22
#define DCAC_OP_SET_DEF_MDACL  23
#define DCAC_OP_SET_ATTR_ACL   24
#define DCAC_OP_SET_FILE_RDACL 25
#define DCAC_OP_SET_FILE_WRACL 26
#define DCAC_OP_SET_FILE_EXACL 27
#define DCAC_OP_SET_FILE_MDACL 28

#define DCAC_OP_GET_MASK 45
#define DCAC_OP_SET_MASK 46
#define DCAC_OP_LOCKDOWN 47
#define DCAC_OP_UNLOCK   48

#define DCAC_OP_GET_ATTR_FD        60
#define DCAC_OP_GET_ATTR_NAME      61
#define DCAC_OP_GET_ATTR_FD_LIST   62

#define __NR_dcac_add_ops   320
#define __NR_dcac_acl_ops   321
#define __NR_dcac_mask_ops  322
#define __NR_dcac_info_ops  323

static long dcac_add_ops(int type, int gid, const char *attr, int flags)
{
    return syscall(__NR_dcac_add_ops, type, gid, attr, flags);
}

static long dcac_acl_ops(int optype, int afd, int ffd,
        const char *acl1, const char *acl2_or_file)
{
    return syscall(__NR_dcac_acl_ops, optype, afd, ffd, acl1, acl2_or_file);
}

static long dcac_mask_ops(int optype, int mask)
{
    return syscall(__NR_dcac_mask_ops, optype, mask);
}

static long dcac_info_ops(int optype, int fd, const char *attr, void *buf, int bufsize)
{
    return syscall(__NR_dcac_info_ops, optype, fd, attr, buf, bufsize);
}

// special add calls
//
static int dcac_add_uname_attr(int flags)
{
    return (int)dcac_add_ops(DCAC_OP_ADD_UNAME, -1, NULL, flags);
}

static int dcac_add_gname_attr(int gid)
{
    return (int)dcac_add_ops(DCAC_OP_ADD_GNAME, gid, NULL, -1);
}

static int dcac_add_any_attr(const char *attr, int flags)
{
    return (int)dcac_add_ops(DCAC_OP_ADD_ANY, -1, attr, flags);
}

// default acls
//
static int dcac_set_def_rdacl(char *plain_acl)
{
    return (int)dcac_acl_ops(DCAC_OP_SET_DEF_RDACL, -1, -1,
            plain_acl, NULL);
}

static int dcac_set_def_wracl(char *plain_acl)
{
    return (int)dcac_acl_ops(DCAC_OP_SET_DEF_WRACL, -1, -1,
            plain_acl, NULL);
}

static int dcac_set_def_exacl(char *plain_acl)
{
    return (int)dcac_acl_ops(DCAC_OP_SET_DEF_EXACL, -1, -1,
            plain_acl, NULL);
}

static int dcac_set_def_mdacl(char *plain_acl)
{
    return (int)dcac_acl_ops(DCAC_OP_SET_DEF_MDACL, -1, -1,
            plain_acl, NULL);
}

// Set acls of an attr(afd) onto a gateway file(ffd)
static int dcac_set_attr_acl(int afd, int ffd,
        const char *addacl, const char *modacl)
{
    return (int)dcac_acl_ops(DCAC_OP_SET_ATTR_ACL,
            afd, ffd, addacl, modacl);
}

// Set acl on file
static int dcac_set_file_rdacl(const char *file, const char *acl)
{
    return (int)dcac_acl_ops(DCAC_OP_SET_FILE_RDACL,
            -1, -1, acl, file);
}

static int dcac_set_file_wracl(const char *file, const char *acl)
{
    return (int)dcac_acl_ops(DCAC_OP_SET_FILE_WRACL,
            -1, -1, acl, file);
}

static int dcac_set_file_exacl(const char *file, const char *acl)
{
    return (int)dcac_acl_ops(DCAC_OP_SET_FILE_EXACL,
            -1, -1, acl, file);
}

static int dcac_set_file_mdacl(const char *file, const char *acl)
{
    return (int)dcac_acl_ops(DCAC_OP_SET_FILE_MDACL,
            -1, -1, acl, file);
}


// Get the FD of attr.
static int dcac_get_attr_fd(const char *attr)
{
    return (int)dcac_info_ops(DCAC_OP_GET_ATTR_FD, -1, attr, NULL, 0);
}

// Get the name of the attr represented by fd.
static int dcac_get_attr_name(int fd, char *buf, int bufsize)
{
    return (int)dcac_info_ops(DCAC_OP_GET_ATTR_NAME, fd, NULL, buf, bufsize);
}

// An array of fd's representing the attrs will be stored in buf,
// terminated by 0.
static int dcac_get_attr_fd_list(int *buf, int bufsize)
{
    return (int)dcac_info_ops(DCAC_OP_GET_ATTR_FD_LIST, -1, NULL, buf, bufsize);
}


// set mask of current process
static void dcac_set_mask(unsigned short mask)
{
    dcac_mask_ops(DCAC_OP_SET_MASK, (int)mask);
}

// get mask of current process
static unsigned short dcac_get_mask(void)
{
    return (unsigned short)dcac_mask_ops(DCAC_OP_GET_MASK, -1);
}

// lock down the current process
static void dcac_lockdown(void)
{
    dcac_mask_ops(DCAC_OP_LOCKDOWN, -1);
}

// unlock the current process
static void dcac_unlock(void)
{
    dcac_mask_ops(DCAC_OP_UNLOCK, -1);
}

#endif // __DCAC_H
