create table t_drive_mountpoint
(
    drive_id   int unsigned   not null
        primary key,
    mountpoint varchar(20000) not null
);

create table t_drive_uuid
(
    drive_id  int unsigned                      not null
        primary key,
    disk_uuid char(50) collate ascii_general_ci not null
);

create table t_drives
(
    drive_id  int unsigned auto_increment
        primary key,
    inode     bigint unsigned not null,
    disk_size bigint unsigned not null,
    disk_free bigint unsigned not null,
    auto_scan tinyint(1)      not null,
    constraint inode
        unique (inode)
);

create table t_exif_data
(
    inode      bigint unsigned not null
        primary key,
    exif_field smallint        not null,
    value      varchar(255)    not null
);

create table t_exif_field
(
    field_id   smallint auto_increment
        primary key,
    field_name varchar(128) not null
);

create table t_file_output
(
    inode       bigint unsigned not null
        primary key,
    file_output varchar(20000)  not null
);

create table t_inode
(
    id        bigint unsigned auto_increment
        primary key,
    parent    bigint unsigned                                 null,
    type      enum ('DIRECTORY', 'DRIVE_DIR', 'FILE', 'LINK') null,
    node_name char(255)                                       not null,
    c_time    datetime                                        not null,
    m_time    datetime                                        not null,
    constraint parent_name
        unique (parent, node_name)
);

create table t_mime_type
(
    id        int unsigned auto_increment
        primary key,
    mime_type varchar(255) not null,
    constraint mime_type
        unique (mime_type)
);

create table t_stats
(
    inode     bigint unsigned   not null
        primary key,
    mode      smallint unsigned not null,
    uid       int unsigned      not null,
    gid       int unsigned      not null,
    file_size bigint unsigned   not null,
    blksize   bigint unsigned   not null,
    blkcnt    bigint unsigned   not null,
    a_time    datetime          not null,
    m_time    datetime          not null,
    c_time    datetime          not null,
    mime_type int unsigned      not null
);

