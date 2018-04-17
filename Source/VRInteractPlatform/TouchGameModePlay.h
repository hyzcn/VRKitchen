// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/GameMode.h"
#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "BaxterRobot.h"
#include "TouchAnimateActor.h"
#include "DocParser.h"
#include "TouchGameModePlay.generated.h"

/**
 * 
 */
UCLASS()
class VRINTERACTPLATFORM_API ATouchGameModePlay : public AGameMode
{
	GENERATED_BODY()
public:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	void UpdateAnim();
	void SegmentRecord();
	void TriggerPause();

	UPROPERTY(BlueprintReadWrite, Category = "TouchGameMode")
		int32 RecordApplied;

private:
	FString PoseData;
	FString FileName;
	ATouchAnimateActor* HumanPawn;
	ABaxterRobot* BaxterRobot;
	float UpdateInterval;
	TArray<FString> ApplyPoseArray;
	FString SegmentPoints;
	bool IsPause;
	bool RecordSegment;
};
