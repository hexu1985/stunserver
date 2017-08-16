/*
   Copyright 2011 John Selbie

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/



#include "commonincludes.hpp"
#include "socketaddress.h"
#include <string>
using std::string;


static void InitSocketAddress(int family, CSocketAddress* pAddr)
{
    if (family == AF_INET)
    {
        sockaddr_in addr = {};
        addr.sin_family = AF_INET;
        *pAddr = CSocketAddress(addr);
    }
    else if (family == AF_INET6)
    {
        sockaddr_in6 addr = {};
        addr.sin6_family = AF_INET6;
        *pAddr = CSocketAddress(addr);
    }
    else
    {
        ASSERT(false);
    }
}


#ifdef _WIN32
ssize_t recvfromex(int sockfd, void* buf, size_t len, int flags, CSocketAddress* pSrcAddr, CSocketAddress* pDstAddr)
{
    GUID WSARecvMsg_GUID = WSAID_WSARECVMSG;
    LPFN_WSARECVMSG WSARecvMsg;
    char ControlBuffer[1000];
    DWORD NumberOfBytes;
    string ErrorMessage, Address;
    WSABUF WSABuf;
    WSAMSG Msg;
    int nResult;
    nResult = WSAIoctl(sockfd, SIO_GET_EXTENSION_FUNCTION_POINTER,
            &WSARecvMsg_GUID, sizeof WSARecvMsg_GUID,
            &WSARecvMsg, sizeof WSARecvMsg,
            &NumberOfBytes, NULL, NULL);
    if (nResult == SOCKET_ERROR) {
        int ErrorCode = WSAGetLastError();
        fprintf(stderr, "WSAIoctl error: %d\n", ErrorCode);
        return -1;
    }

    sockaddr_storage addrRemote = {};
    Msg.name = (sockaddr *) &addrRemote;
    Msg.namelen = sizeof(addrRemote);
    WSABuf.buf = (char *)buf;
    WSABuf.len = len;
    Msg.lpBuffers = &WSABuf;
    Msg.dwBufferCount = 1;
    Msg.Control.buf = ControlBuffer;
    Msg.Control.len = sizeof ControlBuffer;
    Msg.dwFlags = 0;

    nResult = WSARecvMsg(sockfd, &Msg, &NumberOfBytes, NULL, NULL);
    if (nResult == SOCKET_ERROR) {
        int ErrorCode = WSAGetLastError();
        fprintf(stderr, "WSARecvMsg error: %d\n", ErrorCode);
        return -2;
    }

    if (pSrcAddr)
    {
        *pSrcAddr = CSocketAddress(*(sockaddr*)&addrRemote);
    }

    if (pDstAddr)
    {
        WSACMSGHDR *pCMsgHdr = NULL;

        InitSocketAddress(addrRemote.ss_family, pDstAddr);


        for (pCMsgHdr = WSA_CMSG_FIRSTHDR(&Msg); pCMsgHdr != NULL; pCMsgHdr = WSA_CMSG_NXTHDR(&Msg, pCMsgHdr))
        {
            // IPV6 address ----------------------------------------------------------
            if ((pCMsgHdr->cmsg_level == IPPROTO_IPV6) && (pCMsgHdr->cmsg_type == IPV6_PKTINFO) && WSA_CMSG_DATA(pCMsgHdr))
            {
                struct in6_pktinfo* pInfo = (in6_pktinfo*)WSA_CMSG_DATA(pCMsgHdr);
                sockaddr_in6 addr = {};
                addr.sin6_family = AF_INET6;
                addr.sin6_addr = pInfo->ipi6_addr;
                *pDstAddr = CSocketAddress(addr);
                break;
            }


            // IPV4 address ----------------------------------------------------------
            // if you change the ifdef's below, make sure you it's matched with the same logic in stunsocket.cpp
            // Might be worthwhile to just use IP_RECVORIGDSTADDR and IP_ORIGDSTADDR so we can merge with the bsd code

#ifdef IP_PKTINFO
            if ((pCMsgHdr->cmsg_level == IPPROTO_IP) && (pCMsgHdr->cmsg_type==IP_PKTINFO) && WSA_CMSG_DATA(pCMsgHdr))
            {
                struct in_pktinfo* pInfo = (in_pktinfo*)WSA_CMSG_DATA(pCMsgHdr);
                sockaddr_in addr = {};
                addr.sin_family = AF_INET;
                addr.sin_addr = pInfo->ipi_addr;
                *pDstAddr = CSocketAddress(addr);
                break;
            }
#endif
        }

    }

    return NumberOfBytes;
}
#else
ssize_t recvfromex(int sockfd, void* buf, size_t len, int flags, CSocketAddress* pSrcAddr, CSocketAddress* pDstAddr)
{
    struct iovec vec;
    ssize_t ret;

    char controldata[1000];

    struct msghdr hdr = {};
    sockaddr_storage addrRemote = {};

    vec.iov_base = buf;
    vec.iov_len = len;

    hdr.msg_name = &addrRemote;
    hdr.msg_namelen = sizeof(addrRemote);
    hdr.msg_iov = &vec;
    hdr.msg_iovlen = 1;
    hdr.msg_control = controldata;
    hdr.msg_controllen = ARRAYSIZE(controldata);

    ret = ::recvmsg(sockfd, &hdr, flags);

    if (ret > 0)
    {
        if (pSrcAddr)
        {
            *pSrcAddr = CSocketAddress(*(sockaddr*)&addrRemote);
        }

        if (pDstAddr)
        {
            struct cmsghdr* pCmsg = NULL;

            InitSocketAddress(addrRemote.ss_family, pDstAddr);

            for (pCmsg = CMSG_FIRSTHDR(&hdr); pCmsg != NULL; pCmsg = CMSG_NXTHDR(&hdr, pCmsg))
            {
                // IPV6 address ----------------------------------------------------------
                if ((pCmsg->cmsg_level == IPPROTO_IPV6) && (pCmsg->cmsg_type == IPV6_PKTINFO) && CMSG_DATA(pCmsg))
                {
                    struct in6_pktinfo* pInfo = (in6_pktinfo*)CMSG_DATA(pCmsg);
                    sockaddr_in6 addr = {};
                    addr.sin6_family = AF_INET6;
                    addr.sin6_addr = pInfo->ipi6_addr;
                    *pDstAddr = CSocketAddress(addr);
                    break;
                }


                // IPV4 address ----------------------------------------------------------
                // if you change the ifdef's below, make sure you it's matched with the same logic in stunsocket.cpp
                // Might be worthwhile to just use IP_RECVORIGDSTADDR and IP_ORIGDSTADDR so we can merge with the bsd code

#ifdef IP_PKTINFO
                if ((pCmsg->cmsg_level == IPPROTO_IP) && (pCmsg->cmsg_type==IP_PKTINFO) && CMSG_DATA(pCmsg))
                {
                    struct in_pktinfo* pInfo = (in_pktinfo*)CMSG_DATA(pCmsg);
                    sockaddr_in addr = {};
                    addr.sin_family = AF_INET;
                    addr.sin_addr = pInfo->ipi_addr;
                    *pDstAddr = CSocketAddress(addr);
                    break;
                }
#endif
                
#ifdef IP_RECVDSTADDR
                // This code path for MacOSX and likely BSD as well
                if ((pCmsg->cmsg_level == IPPROTO_IP) && (pCmsg->cmsg_type==IP_RECVDSTADDR) && CMSG_DATA(pCmsg))
                {
                    sockaddr_in addr = {};
                    addr.sin_family = AF_INET;
                    addr.sin_addr = *(in_addr*)CMSG_DATA(pCmsg);
                    *pDstAddr = CSocketAddress(addr);
                    break;
                }
#endif
            }
        }
    }

    return ret;
}
#endif

