/* Matija Nalis, mnalis-libchmod@voyager.hr, 2013 released under GPLv3+ */

/* this essentially implements umask()-alike restrictions on chmod(2) and fchmod(2)
   Use it to limit effects of 'SITE CHMOD' in vsftpd(8), for example (allow chmod 644, but not 777)
   by doing "LD_PRELOAD=/usr/local/lib/libchmod.so vsftpd"
   
   v1.0, 2013-01-16
*/


#define _GNU_SOURCE
#include <sys/stat.h>
#include <errno.h>
#include <dlfcn.h>

#define FORCE_UMASK (S_IRUSR|S_IWUSR|S_IXUSR|S_IRGRP|S_IXGRP|S_IROTH|S_IXOTH) /* disable group and world writeable files, as well as SUID/SGID etc. special flags */

 
/*
 * given mode, mask it with forced umask (removing group and others writeable bits)
 */
mode_t fix_mode(mode_t mode)
{ 
  mode_t new_mode;
  new_mode = mode & (FORCE_UMASK);
  return (new_mode);
}
       

int chmod(const char *path, mode_t mode)
{
  int (*libc_chmod)(const char *path, mode_t mode);
  *(void **)(&libc_chmod) = dlsym(RTLD_NEXT, "chmod");
  if(dlerror()) {
    errno = EPERM;
    return -1;
  }
  return (*libc_chmod)(path, fix_mode(mode));
}

int fchmod(int fd, mode_t mode)
{
  int (*libc_fchmod)(int fd, mode_t mode);
  *(void **)(&libc_fchmod) = dlsym(RTLD_NEXT, "fchmod");
  if(dlerror()) {
    errno = EPERM;
    return -1;
  }
  return (*libc_fchmod)(fd, fix_mode(mode));
}
