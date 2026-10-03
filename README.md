# DarkDrives

## Description

DarkDrives is supposed to be a FUSE interface giving access to the filesystem structure and various metadata of dark or cold hard-drives (not connected to any computer).

To do this, the software will scan drives when mounted and add them to a SQLite3 database, and replicate their filesystem structure providing metadata.