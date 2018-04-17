// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/GameMode.h"
#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"
#include "GTCaptureComponent.h"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "BaxterRobot.h"
#include "IKPawn.h"
#include "TouchGameModeRecord.generated.h"

/**
 * 
 */
UCLASS()
class VRINTERACTPLATFORM_API ATouchGameModeRecord : public AGameMode
{
	GENERATED_BODY()
	
public:
	ATouchGameModeRecord();

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void Tick(float DeltaSeconds) override;
	void RecordActors();

private:
	AStaticMeshActor* Door;
	bool PoseRecord;
	FString PoseData;
	FString GameDir;
	FString FileName;
	AIKPawn* HumanPawn;
	ABaxterRobot* BaxterRobot;
	float RecordInterval;
};
