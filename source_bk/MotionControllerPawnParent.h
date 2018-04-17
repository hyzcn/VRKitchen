// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Pawn.h"
#include "MotionControllerPawnParent.generated.h"

UCLASS()
class VRINTERACTPLATFORM_API AMotionControllerPawnParent : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AMotionControllerPawnParent();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaSeconds) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Motion Controller")
		FVector RightHandLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Motion Controller")
		FVector LeftHandLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Motion Controller")
		FRotator RightHandRotation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Motion Controller")
		FRotator LeftHandRotation;

	UFUNCTION(BlueprintCallable, Category = "Motion Controller")
		FVector ReturnRightHandLocation();

	UFUNCTION(BlueprintCallable, Category = "Motion Controller")
		FVector ReturnLeftHandLocation();

	UFUNCTION(BlueprintCallable, Category = "Motion Controller")
		FRotator ReturnRightHandRotation();

	UFUNCTION(BlueprintCallable, Category = "Motion Controller")
		FRotator ReturnLeftHandRotation();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "VRPawn")
		USceneComponent* SceneComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "VRPawn")
		USphereComponent* SphereComponent;

	// UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "VRPawn")
	//     UCameraComponent* CameraComponent;

	UPROPERTY(EditDefaultsOnly, Category = "VRPawn")
		USkeletalMeshComponent* BodyMesh;

	class UControllerPawnMovementComponent* OurMovementComponent;
	virtual UPawnMovementComponent* GetMovementComponent() const override;

	void MoveForward(float AxisValue);
	void MoveRight(float AxisValue);
	void Turn(float AxisValue);

	
	
};
