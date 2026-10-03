//
// Created by Nina Alexandra Klama on 2026-10-03.
//

#ifndef DARKDRIVES_FUSE_INTERFACE_H
#define DARKDRIVES_FUSE_INTERFACE_H

#include <fuse.h>

/*
 * WARNING: This class contains C Code
 *
 * Take care to not shoot yourself into your foot.
 */


class FuseInterface
{
    FuseInterface();
    ~FuseInterface();

private:
    struct fuse_operations operations;
};


#endif //DARKDRIVES_FUSE_INTERFACE_H
