/*
 * Please do not edit this file.
 * It was generated using rpcgen.
 */

#ifndef _MOUNT_H_RPCGEN
#define	_MOUNT_H_RPCGEN

#include <rpc/rpc.h>

#ifdef __cplusplus
extern "C" {
#endif

/* This file is copied from RFC1813
 * Copyright 1995 Sun Micrososystems (I assume)
 */
#define	MNTPATHLEN 1024
#define	MNTNAMLEN 255
#define	FHSIZE3 64

typedef struct {
	u_int fhandle3_len;
	char *fhandle3_val;
} fhandle3;

typedef char *dirpath;

typedef char *name;

typedef struct exportnode *exports;

typedef struct groupnode *groups;

typedef struct mountbody *mountlist;

enum mountstat3 {
	MNT3_OK = 0,
	MNT3ERR_PERM = 1,
	MNT3ERR_NOENT = 2,
	MNT3ERR_IO = 5,
	MNT3ERR_ACCES = 13,
	MNT3ERR_NOTDIR = 20,
	MNT3ERR_INVAL = 22,
	MNT3ERR_NAMETOOLONG = 63,
	MNT3ERR_NOTSUPP = 10004,
	MNT3ERR_SERVERFAULT = 10006
};
typedef enum mountstat3 mountstat3;

struct mountres3_ok {
	fhandle3 fhandle;
	struct {
		u_int auth_flavors_len;
		int *auth_flavors_val;
	} auth_flavors;
};
typedef struct mountres3_ok mountres3_ok;

struct mountres3 {
	mountstat3 fhs_status;
	union {
		mountres3_ok mountinfo;
	} mountres3_u;
};
typedef struct mountres3 mountres3;

struct mountbody {
	name ml_hostname;
	dirpath ml_directory;
	mountlist ml_next;
};
typedef struct mountbody mountbody;

struct groupnode {
	name gr_name;
	groups gr_next;
};
typedef struct groupnode groupnode;

struct exportnode {
	dirpath ex_dir;
	groups ex_groups;
	exports ex_next;
};
typedef struct exportnode exportnode;

#define	MOUNT_PROGRAM	100005
#define	MOUNT_V3	3

#define	MOUNTPROC3_NULL	0
extern  enum clnt_stat mountproc3_null_3(void *, CLIENT *);
#define	MOUNTPROC3_MNT	1
extern  enum clnt_stat mountproc3_mnt_3(dirpath , mountres3 *, CLIENT *);
#define	MOUNTPROC3_DUMP	2
extern  enum clnt_stat mountproc3_dump_3(mountlist *, CLIENT *);
#define	MOUNTPROC3_UMNT	3
extern  enum clnt_stat mountproc3_umnt_3(dirpath , void *, CLIENT *);
#define	MOUNTPROC3_UMNTALL	4
extern  enum clnt_stat mountproc3_umntall_3(void *, CLIENT *);
#define	MOUNTPROC3_EXPORT	5
extern  enum clnt_stat mountproc3_export_3(exports *, CLIENT *);

/* the xdr functions */

extern  bool_t xdr_fhandle3(XDR *, fhandle3*);
extern  bool_t xdr_dirpath(XDR *, dirpath*);
extern  bool_t xdr_name(XDR *, name*);
extern  bool_t xdr_exports(XDR *, exports*);
extern  bool_t xdr_groups(XDR *, groups*);
extern  bool_t xdr_mountlist(XDR *, mountlist*);
extern  bool_t xdr_mountstat3(XDR *, mountstat3*);
extern  bool_t xdr_mountres3_ok(XDR *, mountres3_ok*);
extern  bool_t xdr_mountres3(XDR *, mountres3*);
extern  bool_t xdr_mountbody(XDR *, mountbody*);
extern  bool_t xdr_groupnode(XDR *, groupnode*);
extern  bool_t xdr_exportnode(XDR *, exportnode*);

#ifdef __cplusplus
}
#endif

#endif /* !_MOUNT_H_RPCGEN */
