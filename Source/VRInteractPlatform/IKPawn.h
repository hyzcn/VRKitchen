// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "GameFramework/Pawn.h"
#include "TouchAnimInstance.h"
#include "Runtime/Engine/Classes/Camera/CameraComponent.h"
#include "Runtime/HeadMountedDisplay/Public/MotionControllerComponent.h"
#include "PickCookCutObject.h"
#include <string>
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "IKPawn.generated.h"

using namespace rapidjson;

UCLASS()
class VRINTERACTPLATFORM_API AIKPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AIKPawn();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IKPawn")
		USkeletalMeshComponent* SkeletalMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IKPawn")
		UCameraComponent* Camera;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IKPawn")
		UMotionControllerComponent* MotionController_R;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IKPawn")
		UMotionControllerComponent* MotionController_L;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IKPawn")
		USceneComponent* CameraRoot;

	//Name of the joint where objects should be attached
	FName LeftHandAttachPoint;
	FName RightHandAttachPoint;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void BeginDestroy() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;
	
	virtual void UpdateAnim(FString &PoseData);

	void UpdateMoveAnim();

	void UpdateBodyAnim();

	void Grab(bool IsLeft);

	void Release(bool IsLeft);

	void GrabLeft();

	void GrabRight();

	void ReleaseLeft();

	void ReleaseRight();

	void SqueezeLeft();

	void SqueezeRight();

	void SqueezeReleaseLeft();

	void SqueezeReleaseRight();

	void FireGrabEventsLeft(bool pressed);

	void FireGrabEventsRight(bool pressed);

	void AnimRecord(FString &PoseData);

	void ProcessForward(float AxisValue);
	void ProcessRight(float AxisValue);
	void ProcessRotate(float AxisValue);

	FVector MovementInput;
	float RotationInput;
	float MovementSpeed;
	float TurnSpeed;
	float CurrentSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IKPawn")
		int ActionToTake;

protected:
	float UpdateInterval;
	TArray<UPrimitiveComponent*> LeftHandGrabbedComponents;
	TArray<UPrimitiveComponent*> RightHandGrabbedComponents;


private:
	bool LeftGrabPress, RightGrabPress;
	bool LeftGrab, RightGrab, LeftRelease, RightRelease;
	Document doc;
	bool LeftSqueeze;
	bool RightSqueeze;

	
	// bool PoseRecord;
	// FString PoseData;

};
