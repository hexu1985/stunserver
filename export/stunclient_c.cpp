#include "stunclient_c.h"
#include "commonincludes.hpp"
#include "stuncore.h"
#include "socketrole.h" // so we can re-use the "SocketRole" definitions again
#include "stunsocket.h"
#include "cmdlineparser.h"
#include "recvfromex.h"
#include "resolvehostname.h"
#include "stringhelper.h"
#include "adapters.h"
#include "oshelper.h"
#include "prettyprint.h"

// These files are in ../resources
#include "stunclient.txtcode"
#include "stunclient_lite.txtcode"

#include <mutex>

namespace {

#ifdef _WIN32
struct WSADATA_Guard {
    WSADATA_Guard() {
        if (WSAStartup(MAKEWORD(2, 0), &wsadata) != 0) {
            printf("WSAStartup failed\n");
            exit(1);
        }
    }

    ~WSADATA_Guard() {
        WSACleanup();
    }

    WSADATA wsadata;
};

std::shared_ptr<WSADATA_Guard> wsadata_guard;

std::once_flag init_wsadata_guard_flag;

void init_wsadata_guard()
{   
    wsadata_guard.reset(new WSADATA_Guard);
}

std::shared_ptr<WSADATA_Guard> get_wsadata_guard()
{
    std::call_once(init_wsadata_guard_flag, &init_wsadata_guard);
    return wsadata_guard;
}
#endif

struct ClientSocketConfig
{
    int family;
    int socktype;
    CSocketAddress addrLocal;
};

void DumpConfig(StunClientLogicConfig& config, ClientSocketConfig& socketConfig)
{
    std::string strAddr;


    Logging::LogMsg(LL_DEBUG, "config.fBehaviorTest = %s", config.fBehaviorTest?"true":"false");
    Logging::LogMsg(LL_DEBUG, "config.fFilteringTest = %s", config.fFilteringTest?"true":"false");
    Logging::LogMsg(LL_DEBUG, "config.timeoutSeconds = %d", config.timeoutSeconds);
    Logging::LogMsg(LL_DEBUG, "config.uMaxAttempts = %d", config.uMaxAttempts);

    config.addrServer.ToString(&strAddr);
    Logging::LogMsg(LL_DEBUG, "config.addrServer = %s",  strAddr.c_str());

    socketConfig.addrLocal.ToString(&strAddr);
    Logging::LogMsg(LL_DEBUG, "socketconfig.addrLocal = %s", strAddr.c_str());

}

HRESULT CreateConfigFromArgs(const StunClientArgs_C& args, StunClientLogicConfig* pConfig, ClientSocketConfig* pSocketConfig)
{
    HRESULT hr = S_OK;
    StunClientLogicConfig& config = *pConfig;
    ClientSocketConfig& socketconfig = *pSocketConfig;
    uint16_t localport = 0;
    uint16_t remoteport = 0;
    char szIP[100];
    bool fTCP = false;


    config.fBehaviorTest = false;
    config.fFilteringTest = false;
    config.fTimeoutIsInstant = false;
    config.timeoutSeconds = 0; // use default
    config.uMaxAttempts = 0;

    socketconfig.family = AF_INET;
    socketconfig.socktype = SOCK_DGRAM;
    socketconfig.addrLocal = CSocketAddress(0, 0);

    ChkIfA(pConfig == NULL, E_INVALIDARG);
    ChkIfA(pSocketConfig==NULL, E_INVALIDARG);


    // family (protocol type) ------------------------------------
    if (args.family) 
    {
        switch (args.family)
        {
            case 4: socketconfig.family = AF_INET; break;
            case 6: socketconfig.family = AF_INET6; break;
            default:
            {
                Logging::LogMsg(LL_ALWAYS, "Family option must be either 4 or 6");
                Chk(E_INVALIDARG);
            }
        }
    }
    
    // remote port ---------------------------------------------
    if (args.remoteServerPort) 
    {
        remoteport = args.remoteServerPort;
    }
    else 
    {
        remoteport = DEFAULT_STUN_PORT;
    }

    // remote server -----------------------------------------
    if (StringHelper::IsNullOrEmpty(args.remoteServerHost))
    {
        Logging::LogMsg(LL_ALWAYS, "No server address specified");
        Chk(E_INVALIDARG);
    }

    hr = ::ResolveHostName(args.remoteServerHost, socketconfig.family, false, &config.addrServer);
    
    if (FAILED(hr))
    {
        Logging::LogMsg(LL_ALWAYS, "Unable to resolve hostname for %s", args.remoteServerHost);
        Chk(hr);
    }
    
    config.addrServer.ToStringBuffer(szIP, ARRAYSIZE(szIP));
    Logging::LogMsg(LL_DEBUG, "Resolved %s to %s", args.remoteServerHost, szIP);
    config.addrServer.SetPort(remoteport);


    // local port --------------------------------------------
    if (args.localPort)
    {
        localport = args.localPort;
    }

#if 0
    // local address ------------------------------------------
    if (args.localAddr)
    {
        hr = GetSocketAddressForAdapter(socketconfig.family, args.localAddr, localport, &socketconfig.addrLocal);
        if (FAILED(hr))
        {
            Logging::LogMsg(LL_ALWAYS, "Unable to find matching adapter or interface for local address option");
            Chk(hr);
        }
    }
    else
    {
#endif
        if (socketconfig.family == AF_INET6)
        {
            sockaddr_in6 addr6 = {};
            addr6.sin6_family = AF_INET6;
            socketconfig.addrLocal = CSocketAddress(addr6);
            socketconfig.addrLocal.SetPort(localport);
        }
        else
        {
            socketconfig.addrLocal = CSocketAddress(0,localport);
        }
#if 0
    }
#endif

    // mode ---------------------------------------------
    if (args.mode)
    {
        std::string mode = args.mode;
        if (mode == "basic")
        {
            ;
        }
        else if (mode == "full")
        {
            config.fBehaviorTest = true;
            config.fFilteringTest = (fTCP == false); // impossible to to a filtering test in TCP
        }
        else if (mode == "behavior")
        {
            config.fBehaviorTest = true;
        }
        else if (mode == "filtering")
        {
            config.fFilteringTest = true;
        }
        else
        {
            Logging::LogMsg(LL_ALWAYS, "Mode option must be 'full', 'basic', 'behavior', or 'filtering'");
        }
    }



Cleanup:
    return hr;
}

void NatBehaviorToString(NatBehavior behavior, std::string* pStr)
{
    std::string& str = *pStr;

    switch (behavior)
    {
        case UnknownBehavior:  str="Unknown Behavior"; break;
        case DirectMapping:  str="Direct Mapping"; break;
        case EndpointIndependentMapping: str = "Endpoint Independent Mapping"; break;
        case AddressDependentMapping: str = "Address Dependent Mapping"; break;
        case AddressAndPortDependentMapping: str = "Address and Port Dependent Mapping"; break;
        default: ASSERT(false); str = ""; break;
    }
}

void NatFilteringToString(NatFiltering filtering, std::string* pStr)
{
    std::string& str = *pStr;

    switch (filtering)
    {
        case UnknownFiltering:  str="Unknown Behavior"; break;
        case DirectConnectionFiltering:  str="Direct Mapping"; break;
        case EndpointIndependentFiltering: str = "Endpoint Independent Filtering"; break;
        case AddressDependentFiltering: str = "Address Dependent Filtering"; break;
        case AddressAndPortDependentFiltering: str = "Address and Port Dependent Filtering"; break;
        default: ASSERT(false); str = ""; break;
    }
}

void DumpResults(StunClientLogicConfig& config, StunClientResults& results)
{
    char szBuffer[100];
    const int buffersize = 100;
    std::string strResult;

    Logging::LogMsg(LL_ALWAYS, "Binding test: %s", results.fBindingTestSuccess?"success":"fail");
    if (results.fBindingTestSuccess)
    {
        results.addrLocal.ToStringBuffer(szBuffer, buffersize);
        Logging::LogMsg(LL_ALWAYS, "Local address: %s", szBuffer);

        results.addrMapped.ToStringBuffer(szBuffer, buffersize);
        Logging::LogMsg(LL_ALWAYS, "Mapped address: %s", szBuffer);
    }

    if (config.fBehaviorTest)
    {

        Logging::LogMsg(LL_ALWAYS, "Behavior test: %s", results.fBehaviorTestSuccess?"success":"fail");
        if (results.fBehaviorTestSuccess)
        {
            NatBehaviorToString(results.behavior, &strResult);
            Logging::LogMsg(LL_ALWAYS, "Nat behavior: %s", strResult.c_str());
        }
    }

    if (config.fFilteringTest)
    {
        Logging::LogMsg(LL_ALWAYS, "Filtering test: %s", results.fFilteringTestSuccess?"success":"fail");
        if (results.fFilteringTestSuccess)
        {
            NatFilteringToString(results.filtering, &strResult);
            Logging::LogMsg(LL_ALWAYS, "Nat filtering: %s", strResult.c_str());
        }
    }
}

HRESULT UdpClientLoop(StunClientLogicConfig& config, const ClientSocketConfig& socketconfig, StunClientResults *results)
{
    HRESULT hr = S_OK;
    CStunSocket stunSocket;
    CRefCountedBuffer spMsg(new CBuffer(MAX_STUN_MESSAGE_SIZE));
    int sock = -1;
    CSocketAddress addrDest;   // who we send to
    CSocketAddress addrRemote; // who we
    CSocketAddress addrLocal;
    int ret;
    struct pollfd       client[1];
    std::string strAddr;
    std::string strAddrLocal;

    CStunClientLogic clientlogic;

    hr = clientlogic.Initialize(config);
    
    if (FAILED(hr))
    {
        Logging::LogMsg(LL_ALWAYS, "Unable to initialize client: (error = x%x)", hr);
        Chk(hr);
    }

    hr = stunSocket.UDPInit(socketconfig.addrLocal, RolePP);
    if (FAILED(hr))
    {
        Logging::LogMsg(LL_ALWAYS, "Unable to create local socket: (error = x%x)", hr);
        Chk(hr);
    }
    

    stunSocket.EnablePktInfoOption(true);

    sock = stunSocket.GetSocketHandle();

    // let's get a loop going!

    while (true)
    {
        HRESULT hrRet;
        spMsg->SetSize(0);
        hrRet = clientlogic.GetNextMessage(spMsg, &addrDest, GetMillisecondCounter());

        if (SUCCEEDED(hrRet))
        {
            addrDest.ToString(&strAddr);
            ASSERT(spMsg->GetSize() > 0);
            
            if (Logging::GetLogLevel() >= LL_DEBUG)
            {
                std::string strAddr;
                addrDest.ToString(&strAddr);
                Logging::LogMsg(LL_DEBUG, "Sending message to %s", strAddr.c_str());
            }

            ret = ::sendto(sock, (char *)spMsg->GetData(), spMsg->GetSize(), 0, addrDest.GetSockAddr(), addrDest.GetSockAddrLength());

            if (ret <= 0)
            {
                Logging::LogMsg(LL_DEBUG, "ERROR.  sendto failed (errno = %d)", errno);
            }
            // there's not much we can do if "sendto" fails except time out and try again
        }
        else if (hrRet == E_STUNCLIENT_STILL_WAITING)
        {
            Logging::LogMsg(LL_DEBUG, "Continuing to wait for response...");
        }
        else if (hrRet == E_STUNCLIENT_RESULTS_READY)
        {
            break;
        }
        else
        {
            Logging::LogMsg(LL_DEBUG, "Fatal error (hr == %x)", hrRet);
            Chk(hrRet);
        }


        // now wait for a response
        spMsg->SetSize(0);
        client[0].fd = sock;
        client[0].events = POLLRDNORM;

#ifdef  _WIN32
        ret = WSAPoll(client, 1, 500); // half-second
#else
        ret = poll(client, 1, 500); // half-second
#endif

        if (ret > 0)
        {
            ret = ::recvfromex(sock, spMsg->GetData(), spMsg->GetAllocatedSize(), 0, &addrRemote, &addrLocal);
            if (ret > 0)
            {
                addrLocal.SetPort(stunSocket.GetLocalAddress().GetPort()); // recvfromex doesn't fill in the dest port value, only dest IP
                addrRemote.ToString(&strAddr);
                addrLocal.ToString(&strAddrLocal);
                Logging::LogMsg(LL_DEBUG, "Got response (%d bytes) from %s on interface %s", ret, strAddr.c_str(), strAddrLocal.c_str());
                spMsg->SetSize(ret);
                clientlogic.ProcessResponse(spMsg, addrRemote, addrLocal);
            }
        }
    }

    clientlogic.GetResults(results);

Cleanup:
    return hr;
}

NatType_C NatBehaviorToEnumC(NatBehavior behavior)
{
    NatType_C type = NatType_Unknown;
    switch (behavior)
    {
        case UnknownBehavior: type = NatType_Unknown; break;
        case DirectMapping: type = NatType_DirectMapping; break;
        case EndpointIndependentMapping: type = NatType_EndpointIndependentMapping; break;
        case AddressDependentMapping: type = NatType_AddressDependentMapping; break;
        case AddressAndPortDependentMapping: type = NatType_AddressAndPortDependentMapping; break;
        default: ASSERT(false); type = NatType_Unknown; break;
    }
    return type;
}

void StunClientResultsToStructC(struct StunClientResults_C *results_c, const StunClientResults &results)
{
    results_c->fBindingTestSuccess = results.fBindingTestSuccess;
    memcpy(&results_c->addrLocal, results.addrLocal.GetSockAddr(), results.addrLocal.GetSockAddrLength());
    results_c->addrLocalLen = results.addrLocal.GetSockAddrLength();
    memcpy(&results_c->addrMapped, results.addrMapped.GetSockAddr(), results.addrMapped.GetSockAddrLength());
    results_c->addrMappedLen = results.addrMapped.GetSockAddrLength();

    results_c->fBehaviorTestSuccess = results.fBehaviorTestSuccess;
    results_c->natType = NatBehaviorToEnumC(results.behavior);
}

}   // namespace 

void stun_client_args_c_init(StunClientArgs_C *args)
{
    assert(args);

    memset(args, 0x0, sizeof(*args));
    args->family = 0;
    args->remoteServerHost = NULL;
    args->remoteServerPort = 0;
    args->localAddr = NULL;
    args->localPort = 0;
    args->mode = NULL;
}

int stun_client_udp_loop(const struct StunClientArgs_C *args, struct StunClientResults_C *results_c)
{
    StunClientLogicConfig config;
    ClientSocketConfig socketconfig;

#ifdef _WIN32
	auto guard = get_wsadata_guard();
#endif

    HRESULT hr;
    hr = CreateConfigFromArgs(*args, &config, &socketconfig);
    if (FAILED(hr))
    {
        Logging::LogMsg(LL_ALWAYS, "CreateConfigFromCommandLine error");
        errno = hr;
        return -1;
    }

#ifdef DEBUG
    DumpConfig(config, socketconfig);
#endif

    StunClientResults results;
    results.Init();
    hr = UdpClientLoop(config, socketconfig, &results);
    if (FAILED(hr))
    {
        Logging::LogMsg(LL_ALWAYS, "UdpClientLoop error");
        errno = hr;
        return -2;
    }

#ifdef DEBUG
    DumpResults(config, results);
#endif

    StunClientResultsToStructC(results_c, results);

    return 0;
}

