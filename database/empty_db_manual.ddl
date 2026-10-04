CREATE TABLE IF NOT EXISTS t_inode (
    id     BIGINT UNSIGNED NOT NULL PRIMARY KEY AUTO_INCREMENT,
    parent BIGINT UNSIGNED NULL,
    type   ENUM('DRIVE', 'DIR', 'FILE', 'LINK') NULL,
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
    size BIGINT UNSIGNED NOT NULL,
    blksize BIGINT UNSIGNED NOT NULL,
    blkcnt BIGINT UNSIGNED NOT NULL,
    a_time DATETIME NOT NULL,
    m_time DATETIME NOT NULL,
    c_time DATETIME NOT NULL,
    mime_type INT UNSIGNED NOT NULL
);

create table IF NOT EXISTS t_mime_type (
    id INT UNSIGNED NOT NULL PRIMARY KEY,
    mime_type VARCHAR(255) CHARACTER SET utf8 NOT NULL UNIQUE KEY
);

create table IF NOT EXISTS t_file_output (
    inode BIGINT UNSIGNED NOT NULL PRIMARY KEY,
    file_output VARCHAR(20000) CHARACTER SET utf8 NOT NULL
);

create table IF NOT EXISTS t_exif_field (
    field_id SMALLINT NOT NULL PRIMARY KEY,
    field_name VARCHAR(128) CHARACTER SET utf8 NOT NULL
);

create table IF NOT EXISTS t_exif_data (
    inode BIGINT UNSIGNED NOT NULL PRIMARY KEY,
    exif_field SMALLINT NOT NULL,
    value VARCHAR(255) CHARACTER SET utf8 NOT NULL
);

insert into t_inode set id=0, parent=NULL, type='DRIVE', node_name='/', c_time='2026-10-04 02:21:45', m_time='2026-10-04 02:21:45';
