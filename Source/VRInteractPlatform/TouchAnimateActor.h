// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "TouchAnimInstance.h"
#include "Runtime/Engine/Classes/Camera/CameraComponent.h"
#include "Runtime/HeadMountedDisplay/Public/MotionControllerComponent.h"
#include <string>
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "TouchAnimateActor.generated.h"

using namespace rapidjson;

UCLASS()
class VRINTERACTPLATFORM_API ATouchAnimateActor : public AActor
{
	GENERATED_BODY()
	
public:
	ATouchAnimateActor();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimatePawn")
		USkeletalMeshComponent* SkeletalMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimatePawn")
		UCameraComponent* Camera;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimatePawn")
		UMotionControllerComponent* MotionController_R;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimatePawn")
		UMotionControllerComponent* MotionController_L;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimatePawn")
		USceneComponent* CameraRoot;

	FName LeftHandAttachPoint;
	FName RightHandAttachPoint;

	virtual void BeginPlay() override;
	// virtual void BeginDestroy() override;

	virtual void Tick(float DeltaSeconds) override;

	void Grab(bool IsLeft);
	void Release(bool IsLeft);

	void UpdateAnim(FString record);
	void UpdateActor(Document &doc);
	void UpdateHead(Document &doc);
	void UpdateBody(Document &doc);

private:
	bool RecordTrigger;
	FString RecordSeg;

	TArray<UPrimitiveComponent*> LeftHandGrabbedComponents;
	TArray<UPrimitiveComponent*> RightHandGrabbedComponents;

	TArray<AActor*> LeftHandCustomGrab;
	TArray<AActor*> RightHandCustomGrab;

	UTouchAnimInstance* AnimInstance;


	
};
