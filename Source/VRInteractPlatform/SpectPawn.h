// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "SpectPawn.generated.h"

UCLASS()
class VRINTERACTPLATFORM_API ASpectPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ASpectPawn();
	FVector MovementInput;
	float RotationInput;
	float MovementSpeed;
	float TurnSpeed;
	float CurrentSpeed;
	void ProcessForward(float AxisValue);
	void ProcessRight(float AxisValue);
	void ProcessRotate(float AxisValue);
	void UpdateMoveAnim();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	float UpdateInterval;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	
	
};
