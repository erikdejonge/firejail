# Firejail profile for clementine
# This file is overwritten after every install/update
# Persistent local customizations
include /etc/firejail/clementine.local
# Persistent global definitions
include /etc/firejail/globals.local

noblacklist ~/.config/Clementine

include /etc/firejail/disable-common.inc
include /etc/firejail/disable-devel.inc
include /etc/firejail/disable-passwdmgr.inc
include /etc/firejail/disable-programs.inc

caps.drop all
nonewprivs
noroot
notv
novideo
protocol unix,inet,inet6
# Clementine makes ioprio_set system calls, which are blacklisted by default.
seccomp.drop @cpu-emulation,@debug,@obsolete,@privileged,@resources,add_key,fanotify_init,io_cancel,io_destroy,io_getevents,io_setup,io_submit,kcmp,keyctl,name_to_handle_at,ni_syscall,open_by_handle_at,personality,process_vm_readv,ptrace,remap_file_pages,request_key,syslog,umount,userfaultfd,vmsplice
