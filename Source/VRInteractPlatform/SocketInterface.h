// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "VRInteractPlatform.h"

#include "Runtime/Core/Public/HAL/ThreadingBase.h"
#include "Networking.h"
#include "TCPReceiver.h"

#include "SocketInterface.generated.h"

#define UB_TCP_BUFFER_SIZE 50000

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class USocketInterface : public UInterface
{
	GENERATED_UINTERFACE_BODY()
};

/**
 * 
 */
class VRINTERACTPLATFORM_API ISocketInterface
{
	GENERATED_IINTERFACE_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	
	FSocket* ListenerSocket;
    FSocket* ConnectionSocket;
	FSocket* SenderSocket;
    FTcpListener* TcpListener;
    TCPReceiver* Receiver;
    // Whether we are accepting
    bool Accepting;
	bool Connected;
	void SocketInit();
	bool SocketConnect();
	void SocketDestroy();
};
