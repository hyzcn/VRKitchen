// Fill out your copyright notice in the Description page of Project Settings.

#include "VRInteractPlatform.h"
#include "TouchAnimInstance.h"

UTouchAnimInstance::UTouchAnimInstance()
{
	//HeadWorldTransform = FTransform(FRotator(0,0,90), FVector(0,0,160.0), FVector(1.0,1.0,1.0));
	LeftHandWorldPos = FVector(40.0, 20.0, 100.0);
	RightHandWorldPos = FVector(-40.0, 20.0, 100.0);
	LeftGrabAnim = 0.0;
	RightGrabAnim = 0.0;
}

void UTouchAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

}

void UTouchAnimInstance::NativeUpdateAnimation(float DeltaTimeX)
{
	Super::NativeUpdateAnimation(DeltaTimeX);

}


