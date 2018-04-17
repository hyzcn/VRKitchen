// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "VRInteractPlatform.h"
#include "Components/SceneComponent.h"
#include "Engine.h"
#include "ViewMode.h"
#include "GTCaptureComponent.generated.h"

struct FGTCaptureTask
{
	FString Mode;
	FString Filename;
	uint64 CurrentFrame;
	FGTCaptureTask() {}
	FGTCaptureTask(FString InMode, FString InFilename, uint64 InCurrentFrame) :
		Mode(InMode), Filename(InFilename), CurrentFrame(InCurrentFrame){}
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class VRINTERACTPLATFORM_API UGTCaptureComponent : public USceneComponent
{
	GENERATED_BODY()
private:
	UGTCaptureComponent();
	AActor* Actor;

	TArray<uint8> NpySerialization(TArray<FColor> ImageData, int32 Width, int32 Height, int32 Channel);
	TArray<uint8> NpySerialization(TArray<FFloat16Color> ImageData, int32 Width, int32 Height, int32 Channel);

public:
	static UMaterial* GetMaterial(FString ModeName);

	// virtual void Tick(float DeltaTime) override; // TODO
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override; // TODO

	void SetFOVAngle(float FOV);

	static UGTCaptureComponent* Create(AActor* InActor, TArray<FString> Modes, UWorld* World, USceneComponent* AttachComponent);

	/** Read binary data in png format */
	TArray<uint8> CapturePng(FString Mode);

	/** Read binary data in uncompressed numpy array (e.g. depth image) */
	TArray<uint8> CaptureNpyUint8(FString Mode, int32 Channels);

	/** Read binary data in uncompressed numpy array (e.g. depth data) */
	TArray<uint8> CaptureNpyFloat16(FString Mode, int32 Channels);

	USceneCaptureComponent2D* GetCaptureComponent(FString Mode);

	void InitCaptureComponent(USceneCaptureComponent2D* CaptureComponent);

	/** Save image to a file */
	void Capture(FString Mode, FString InFilename);

private:
	const bool bIsTicking = true;

	TQueue<FGTCaptureTask, EQueueMode::Spsc> PendingTasks;
	TMap<FString, USceneCaptureComponent2D*> CaptureComponents;
	
};

