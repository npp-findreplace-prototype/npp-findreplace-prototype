#ifndef EMBEDDED_RESOURCES_H
#define EMBEDDED_RESOURCES_H

int EmbeddedResources_Find(
    const char *group,
    const char *name,
    const unsigned char **data,
    unsigned long *size
);

#endif