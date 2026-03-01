struct sockaddr {
    unsigned short  sa_family;      // AF_INET, AF_INET6
    char            sa_data[14];    // useless
};

// IPv4:port
struct sockaddr_in {
    sa_family_t     sin_family;     // AF_INET
    uint16_t        sin_port;       // port number, big-endian
    struct in_addr  sin_addr;       // IPv4 address
};

// IPv6:port
struct sockaddr_in6 {
    sa_family_t     sin6_family;    // AF_INET6
    uint16_t        sin6_port;      // port number, big-endian
    uint32_t        sin6_flowinfo;
    struct in6_addr sin6_addr;      // IPv6 address
    uint32_t        sin6_scope_id;
};

// can store both sockaddr_in & sockaddr_in6
struct sockaddr_storage {
    sa_family_t     ss_family;      // AF_INET, AF_INET6
    char __some_padding[__BIG_ENOUGH_NUMBER];
};
