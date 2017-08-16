#ifndef STUNNATTYPE_C_H
#define STUNNATTYPE_C_H

enum NatType_C
{
    NatType_Unknown = 0,
    NatType_DirectMapping = 1,                  // IP address and port are the same between client and server view (NO NAT)
    NatType_EndpointIndependentMapping = 2,     // same mapping regardless of IP:port original packet sent to (the kind of NAT we like)
    NatType_AddressDependentMapping = 3,        // mapping changes for local socket based on remote IP address only, but remote port can change (partially symmetric, not great)
    NatType_AddressAndPortDependentMapping = 4, // different port mapping if the ip address or port change (symmetric NAT, difficult to predict port mappings)
};

#endif
