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
#include "GTCaptureComponent.h"
#include "Public/RenderResource.h"
#include "TouchGameModePlay.generated.h"

/**
*
*/
struct RGB
{
	TArray<uint8> rgb;
	int count;
	RGB(TArray<uint8> a, int b) :rgb(a), count(b) {};
};
struct MASK
{
	TArray<uint8> mask;
	int count;
	MASK(TArray<uint8> a, int b) :mask(a), count(b) {};
};

struct DEPTH
{
	TArray<uint8> depth;
	int count;
	DEPTH(TArray<uint8> a, int b) :depth(a), count(b) {};
};

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
	void TakeScreenShot();
	void recordMask();
	void recordRgb();
	void recordDepth();


	UPROPERTY(BlueprintReadWrite, Category = "TouchGameMode")
		int32 RecordApplied;

private:
	//bool PaintObject(AActor* Actor, const FColor& Color, bool IsColorGammaEncoded);
	void PaintSkelMesh(USkinnedMeshComponent* SkinnedMeshComponent, const FColor& VertexColor);
	void PaintVertexColor(AActor* Actor, const FColor& PaintColor);
	void PaintStaticMesh(UStaticMeshComponent* StaticMeshComponent, const FColor& VertexColor);
	FColor GenerateColor();

	FString PoseData;
	FString FileName;
	ATouchAnimateActor* HumanPawn;
	UCameraComponent* Camera;
	USceneComponent* CameraRoot;
	float UpdateInterval;
	TArray<FString> ApplyPoseArray;
	FString SegmentPoints;
	bool IsPause;
	bool RecordSegment;
	FString GameDir;

	/////
	ACameraActor *Cam;
	UGTCaptureComponents *CapComp;
	TArray<RGB> rgb;
	TArray<MASK> mask;
	TArray<DEPTH> depth;
	TArray<AActor*> FoundActors;
	TMap<FColor, AActor*> ColorActorTable;


};