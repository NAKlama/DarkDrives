CREATE TABLE IF NOT EXISTS t_inode (
    id     BIGINT UNSIGNED NOT NULL PRIMARY KEY AUTO_INCREMENT,
    parent BIGINT UNSIGNED NULL,
    type   ENUM('DIRECTORY', 'DRIVE_DIR', 'FILE', 'LINK') NULL,
    node_name CHAR(255) CHARACTER SET utf8 NOT NULL,
    c_time DATETIME NOT NULL,
    m_time DATETIME NOT NULL
);

CREATE UNIQUE INDEX parent_name ON t_inode (parent, node_name);

CREATE TABLE IF NOT EXISTS t_stats (
    inode BIGINT UNSIGNED NOT NULL PRIMARY KEY,
    mode SMALLINT UNSIGNED NOT NULL,
    uid INT UNSIGNED NOT NULL,
    gid INT UNSIGNED NOT NULL,
    file_size BIGINT UNSIGNED NOT NULL,
    blksize BIGINT UNSIGNED NOT NULL,
    blkcnt BIGINT UNSIGNED NOT NULL,
    a_time DATETIME NOT NULL,
    m_time DATETIME NOT NULL,
    c_time DATETIME NOT NULL,
    mime_type INT UNSIGNED NOT NULL
);

create table IF NOT EXISTS t_mime_type (
    id INT UNSIGNED NOT NULL PRIMARY KEY AUTO_INCREMENT,
    mime_type VARCHAR(255) CHARACTER SET utf8 NOT NULL UNIQUE KEY
);

create table IF NOT EXISTS t_file_output (
    inode BIGINT UNSIGNED NOT NULL PRIMARY KEY,
    file_output VARCHAR(20000) CHARACTER SET utf8 NOT NULL
);

create table IF NOT EXISTS t_exif_field (
    field_id SMALLINT NOT NULL PRIMARY KEY AUTO_INCREMENT,
    field_name VARCHAR(128) CHARACTER SET utf8 NOT NULL
);

create table IF NOT EXISTS t_exif_data (
    inode BIGINT UNSIGNED NOT NULL PRIMARY KEY,
    exif_field SMALLINT NOT NULL,
    value VARCHAR(255) CHARACTER SET utf8 NOT NULL
);

create table IF NOT EXISTS t_drives (
    drive_id INT UNSIGNED NOT NULL PRIMARY KEY AUTO_INCREMENT,
    inode BIGINT UNSIGNED NOT NULL UNIQUE KEY,
    type ENUM('PART', 'ZFS', 'MOUNT') NOT NULL,
    disk_size BIGINT UNSIGNED NOT NULL,
    disk_free BIGINT UNSIGNED NOT NULL,
    auto_scan BOOL NOT NULL
);

create table IF NOT EXISTS t_drive_uuid (
    drive_id INT UNSIGNED NOT NULL PRIMARY KEY,
    disk_uuid CHAR(50) CHARACTER SET ascii NOT NULL
)

create table IF NOT EXISTS t_drive_mountpoint (
    drive_id INT UNSIGNED NOT NULL PRIMARY KEY,
    mountpoint VARCHAR(20000) CHARACTER SET utf8 NOT NULL
);

create table IF NOT EXISTS t_drive_zfs (
    drive_id INT UNSIGNED NOT NULL PRIMARY KEY,
    zfs_guid BIGINT UNSIGNED NOT NULL,
    zfs_name CHAR(255) CHARACTER SET utf8 NOT NULL
);

insert into t_inode set
                        id=0,
                        parent=NULL,
                        type='DIRECTORY',
                        node_name='/',
                        c_time='2026-10-04 02:21:45',
                        m_time='2026-10-04 02:21:45';
