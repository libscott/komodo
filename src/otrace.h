#pragma once


static size_t djb_hash(const uint8_t* cp, size_t size)
{
    size_t hash = 5381;
    while (size--)
        hash = 33 * hash ^ (unsigned char) cp[size];
    return hash;
}

#define otrace(x,s) \
    printf("%s:%i\n", __FILE__, __LINE__); \
    _otrace(x, s);

void _otrace(const uint8_t* p, size_t size)
{
    printf("%u\t", (unsigned int)djb_hash(p, size));
    for (size_t i=0; i<size; i++)
        printf("%02x", p[i]);
    printf("\n");
}
