# Firejail profile for vim
# This file is overwritten after every install/update
# Persistent local customizations
include /etc/firejail/vim.local
# Persistent global definitions
include /etc/firejail/globals.local

noblacklist ~/.vim
noblacklist ~/.viminfo
noblacklist ~/.vimrc

include /etc/firejail/disable-common.inc
include /etc/firejail/disable-passwdmgr.inc
include /etc/firejail/disable-programs.inc

caps.drop all
netfilter
nodvd
nogroups
nonewprivs
noroot
notv
novideo
protocol unix,inet,inet6
seccomp
