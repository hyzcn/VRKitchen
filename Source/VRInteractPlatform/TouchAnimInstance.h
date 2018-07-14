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
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Avateering")
		FTransform HeadWorldTransform;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Avateering")
		FVector LeftHandWorldPos;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Avateering")
		FVector RightHandWorldPos;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Avateering")
		FRotator LeftHandWorldRot;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Avateering")
		FRotator RightHandWorldRot;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Avateering")
		float Speed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Avateering")
		float Direction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Avateering")
		float LeftGrabAnim;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Avateering")
		float RightGrabAnim;


};
