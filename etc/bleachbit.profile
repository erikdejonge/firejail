# This file is overwritten during software install.
# Persistent customizations should go in a .local file.
include /etc/firejail/bleachbit.local

# bleachbit profile
include /etc/firejail/disable-common.inc
# include /etc/firejail/disable-programs.inc
include /etc/firejail/disable-devel.inc
include /etc/firejail/disable-passwdmgr.inc

caps.drop all
net none
netfilter
no3d
nogroups
nonewprivs
noroot
nosound
protocol unix
seccomp
shell none

# private-bin
# private-dev
# private-tmp
# private-etc

noexec ${HOME}
noexec /tmp
