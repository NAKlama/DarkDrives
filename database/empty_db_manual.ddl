CREATE TABLE IF NOT EXISTS inode (
    id     BIGINT UNSIGNED NOT NULL PRIMARY KEY AUTO_INCREMENT,
    parent BIGINT UNSIGNED NULL,
    type   ENUM('DRIVE', 'DIR', 'FILE', 'LINK') NULL,
    name   CHAR(255) CHARACTER SET utf8 NOT NULL,
    c_time DATETIME NOT NULL,
    m_time DATETIME NOT NULL
);

CREATE UNIQUE INDEX parent_name ON inode (parent, name);

CREATE TABLE IF NOT EXISTS stats (
    inode BIGINT UNSIGNED NOT NULL PRIMARY KEY,
    mode BIT(9) NOT NULL,
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

create table IF NOT EXISTS mime_type (
    id INT UNSIGNED NOT NULL PRIMARY KEY,
    mime_type VARCHAR(255) CHARACTER SET utf8 NOT NULL UNIQUE KEY
);

create table IF NOT EXISTS file_output (
    inode BIGINT UNSIGNED NOT NULL PRIMARY KEY,
    file_output VARCHAR(20000) CHARACTER SET utf8 NOT NULL
);

create table IF NOT EXISTS exif_field (
    field_id SMALLINT NOT NULL PRIMARY KEY,
    name VARCHAR(128) CHARACTER SET utf8 NOT NULL
)

create table IF NOT EXISTS exif_data (
    inode BIGINT UNSIGNED NOT NULL PRIMARY KEY,
    exif_field SMALLINT NOT NULL,
    value VARCHAR(255) CHARACTER SET utf8 NOT NULL
)

insert into inode set id=0, parent=NULL, type='DRIVE', name='/', c_time='2026-10-04 02:21:45', m_time='2026-10-04 02:21:45';
