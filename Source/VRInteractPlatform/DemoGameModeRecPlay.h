// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/GameMode.h"
#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "TouchAnimateActor.h"
#include "IKPawn.h"
#include "DocParser.h"
#include "OnOffObject.h"
#include "DemoGameModeRecPlay.generated.h"

/**
 * 
 */
UCLASS()
class VRINTERACTPLATFORM_API ADemoGameModeRecPlay : public AGameMode
{
	GENERATED_BODY()
	
public:
	ADemoGameModeRecPlay();

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void Tick(float DeltaSeconds) override;
	void RecordActors();
	int32 RecordApplied1;
	int32 RecordApplied2;
	int32 RecordApplied3;
	int32 RecordApplied4;
	int32 RecordApplied5;

private:
	AOnOffObject* LeftDoor;
	FString HumanRecord;
	bool PoseRecord;
	FString PoseData;
	FString GameDir;
	FString FileName1;
	FString FileName2;
	FString FileName3_1;
	FString FileName3_2;
	FString FileName3_3;
	FString FileName4;
	FString FileName5_1;
	FString FileName5_2;
	FString FileName5_3;
	AIKPawn* HumanPawn;
	ATouchAnimateActor* MachineActor;
	float RecordInterval;
	TArray<FString> ApplyPoseArray1;
	TArray<FString> ApplyPoseArray2;
	TArray<FString> ApplyPoseArray3_1;
	TArray<FString> ApplyPoseArray3_2;
	TArray<FString> ApplyPoseArray3_3;
	TArray<FString> ApplyPoseArray4;
	TArray<FString> ApplyPoseArray5_1;
	TArray<FString> ApplyPoseArray5_2;
	TArray<FString> ApplyPoseArray5_3;
	bool OpenDoorFlag;
	int ShowMenu;
	
};
