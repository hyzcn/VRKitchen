// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Animation/AnimInstance.h"
#include "TouchAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class VRINTERACTPLATFORM_API UTouchAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	UTouchAnimInstance();
	
	virtual void NativeInitializeAnimation() override;

	virtual void NativeUpdateAnimation(float DeltaTimeX) override;
	
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "Avateering")
		FTransform HeadWorldTransform;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "Avateering")
		FVector LeftHandWorldPos;
	
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "Avateering")
		FVector RightHandWorldPos;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "Avateering")
		FRotator LeftHandWorldRot;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "Avateering")
		FRotator RightHandWorldRot;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "Avateering")
		float Speed;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "Avateering")
		float Direction;


};
