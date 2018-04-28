// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/GameMode.h"
#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "TouchAnimateActor.h"
#include "DocParser.h"
#include "DemoGameModePlay.generated.h"

/**
 * 
 */
UCLASS()
class VRINTERACTPLATFORM_API ADemoGameModePlay : public AGameMode
{
	GENERATED_BODY()


public:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void Tick(float DeltaSeconds) override;
	void UpdateAnim();
	void SegmentRecord();
	void TriggerPause();

	UPROPERTY(BlueprintReadWrite, Category = "TouchGameMode")
		int32 RecordApplied1;

	UPROPERTY(BlueprintReadWrite, Category = "TouchGameMode")
		int32 RecordApplied2;

private:
	FString PoseData;
	FString FileName;
	class ATouchAnimateActor* HumanPawn1;
	class ATouchAnimateActor* HumanPawn2;
	float UpdateInterval;
	TArray<FString> ApplyPoseArray1;
	TArray<FString> ApplyPoseArray2;
	FString SegmentPoints;
	bool IsPause;
	bool RecordSegment;
	
	
	
};
