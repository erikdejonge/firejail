# Firejail profile for geeqie
# This file is overwritten after every install/update
# Persistent local customizations
include /etc/firejail/geeqie.local
# Persistent global definitions
include /etc/firejail/globals.local

noblacklist ~/.cache/geeqie
noblacklist ~/.config/geeqie
noblacklist ~/.local/share/geeqie

include /etc/firejail/disable-common.inc
include /etc/firejail/disable-devel.inc
include /etc/firejail/disable-passwdmgr.inc
include /etc/firejail/disable-programs.inc

caps.drop all
nodvd
nogroups
nonewprivs
noroot
nosound
notv
novideo
protocol unix
seccomp
shell none

# private-bin geeqie
private-dev
# private-etc X11
