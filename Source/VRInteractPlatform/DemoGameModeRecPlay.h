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
	UPROPERTY(BlueprintReadWrite, Category = "TouchGameMode")
		int32 RecordApplied2;

private:
	bool PoseRecord;
	FString PoseData;
	FString GameDir;
	FString FileName;
	AIKPawn* HumanPawn1;
	ATouchAnimateActor* HumanPawn2;
	float RecordInterval;
	TArray<FString> ApplyPoseArray2;

	
};