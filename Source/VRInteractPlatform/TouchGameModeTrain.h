// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "Engine/GameEngine.h"
#include "EngineGlobals.h"
#include "SlateCore.h"
#include "GameFramework/GameMode.h"
#include "EngineUtils.h"
#include "DocParser.h"
#include "Kismet/GameplayStatics.h"
#include "GTCaptureComponent.h"
#include "GameFramework/Controller.h"
#include "BaxterRobot.h"
#include "TouchAnimateActor.h"
#include "CentralCommunicator.h"
#include "TouchGameModeTrain.generated.h"

/**
 * 
 */
UCLASS()
class VRINTERACTPLATFORM_API ATouchGameModeTrain : public AGameMode
{
	GENERATED_BODY()

public:
	ATouchGameModeTrain();
	virtual void BeginPlay() override;
	void Step();
	void UpdateAnim();
	void CheckDone();
	virtual void Tick(float DeltaSeconds) override;

private:
	FString HumanRecord;
	ATouchAnimateActor* HumanPawn;
	ABaxterRobot* BaxterRobot;
	AStaticMeshActor* Door;
	UStaticMeshComponent* DoorHandle;
	ACentralCommunicator* Communicator;
	UGTCaptureComponents* CapComp;
	uint64 RecordApplied;
	TArray<FString> ApplyPoseArray;
	float StepInterval;
	bool Done;
	float Reward;
	FString StateData;
	bool DataToSend;
	bool ResetLevel;
	bool UpdateHuman;
	FTimerHandle ReceiverHandler;
	Document doc;
	TArray<uint8> DepthData;
	TArray<uint8> RGBImage;
	FString delim;
	TArray<uint8> Frame;
	FVector Goal;
	FVector Loc;
	UWorld* World;
	APlayerController* player;
};
