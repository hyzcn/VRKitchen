// Fill out your copyright notice in the Description page of Project Settings.

#include "VRInteractPlatform.h"
#include "SpectPawn.h"


// Sets default values
ASpectPawn::ASpectPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	MovementSpeed = 100;
	TurnSpeed = 500;
	UpdateInterval = 0.01;
}

// Called when the game starts or when spawned
void ASpectPawn::BeginPlay()
{
	Super::BeginPlay();

	FTimerHandle ReceiverHandler;
	GetWorldTimerManager().SetTimer(ReceiverHandler, this, &ASpectPawn::UpdateMoveAnim, UpdateInterval, true);
	
}

// Called every frame
void ASpectPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ASpectPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	InputComponent->BindAxis("MoveForward", this, &ASpectPawn::ProcessForward);
	InputComponent->BindAxis("MoveRight", this, &ASpectPawn::ProcessRight);
	InputComponent->BindAxis("Turn", this, &ASpectPawn::ProcessRotate);

}

void ASpectPawn::ProcessForward(float AxisValue)
{
	if (AxisValue > 0.6 || AxisValue < -0.6)
		// if (AxisValue > 0.6)
		MovementInput.X = AxisValue;
	else
		MovementInput.X = 0;
}

void ASpectPawn::ProcessRight(float AxisValue)
{
	if (AxisValue > 0.6 || AxisValue < -0.6)
		MovementInput.Y = AxisValue;
	else
		MovementInput.Y = 0;
}

void ASpectPawn::ProcessRotate(float AxisValue)
{
	RotationInput = AxisValue;
}

void ASpectPawn::UpdateMoveAnim()
{

	FRotator NewRotation = GetActorRotation();
	NewRotation.Yaw += TurnSpeed * RotationInput * UpdateInterval; //DeltaTime
	SetActorRotation(NewRotation);
	FVector DisplacementVector = FVector(0, 0, 0);
	DisplacementVector = GetActorForwardVector() * MovementInput.X + GetActorRightVector() * MovementInput.Y;
	DisplacementVector = DisplacementVector.GetSafeNormal();
	DisplacementVector = MovementSpeed * DisplacementVector * UpdateInterval; //DeltaTime;
	FVector NewLocation = GetActorLocation() + DisplacementVector;
	SetActorLocation(NewLocation);

}