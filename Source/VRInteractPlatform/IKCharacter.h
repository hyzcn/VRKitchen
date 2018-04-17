// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Character.h"
#include "IKCharacter.generated.h"

UCLASS()
class VRINTERACTPLATFORM_API AIKCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AIKCharacter();

	//Name of the joint where objects should be attached
	FName LeftHandAttachPoint;
	FName RightHandAttachPoint;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaSeconds) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;

	virtual void UpdateAnim();

	void Grab(bool IsLeft);

	void Release(bool IsLeft);

	void GrabLeft();

	void GrabRight();

	void ReleaseLeft();

	void ReleaseRight();

	void FireGrabEventsLeft(bool pressed);

	void FireGrabEventsRight(bool pressed);



protected:
	float UpdateInterval;

	TArray<UPrimitiveComponent*> LeftHandGrabbedComponents;
	TArray<UPrimitiveComponent*> RightHandGrabbedComponents;

private:
	bool LeftGrabPress, RightGrabPress;
	bool LeftGrab, RightGrab, LeftRelease, RightRelease;

	
	
};
