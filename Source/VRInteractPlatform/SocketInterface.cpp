// Fill out your copyright notice in the Description page of Project Settings.

#include "VRInteractPlatform.h"
#include "SocketInterface.h"

// This function does not need to be modified.
USocketInterface::USocketInterface(const class FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
}

// Add default functionality here for any ISocketInterface functions that are not pure virtual.
void ISocketInterface::SocketInit()
{
	FIPv4Endpoint Endpoint(FIPv4Address(128, 97, 86, 170), 10120);
    ListenerSocket = FTcpSocketBuilder(TEXT("TCPLISTENER"))
        .AsReusable()
        .BoundToEndpoint(Endpoint)
        .WithReceiveBufferSize(UB_TCP_BUFFER_SIZE)
        .WithSendBufferSize(UB_TCP_BUFFER_SIZE)
        .Listening(1);

	SenderSocket = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateSocket(NAME_Stream, TEXT("sender"), false);
	
}

bool ISocketInterface::SocketConnect()
{
	FIPv4Address SenderIP(128, 97, 86, 199);
	TSharedRef<FInternetAddr> addr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
	addr->SetIp(SenderIP.Value);
	addr->SetPort(10121);
	return SenderSocket->Connect(*addr);
}

void ISocketInterface::SocketDestroy()
{
	// Destroy the TCPListener.
    if (TcpListener != NULL) {
        delete TcpListener;
        TcpListener = NULL;
    }
    // Release the socket.
    if (ListenerSocket != NULL) {
        ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(ListenerSocket);
        ListenerSocket = NULL;
    }
    // Destroy the Receiver.
    if (Receiver != NULL) {
        delete Receiver;
        Receiver = NULL;
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("Destroy receiver!")));
    }
    // Release the socket of the connection.
    if (ConnectionSocket != NULL) {
        ConnectionSocket->Close();
        ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(ConnectionSocket);
        ConnectionSocket = NULL;
    }

	if (SenderSocket != NULL) {
		ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(SenderSocket);
		SenderSocket = NULL;
	}
}
