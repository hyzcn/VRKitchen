// Fill out your copyright notice in the Description page of Project Settings.
#include "VRInteractPlatform.h"
#include "MotionControllerPawnParent.h"
#include "ControllerPawnMovementComponent.h"

// Sets default values
AMotionControllerPawnParent::AMotionControllerPawnParent()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp_Root"));
	SphereComponent->SetupAttachment(SceneComponent);

	RootComponent = SphereComponent;

	// CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	// CameraComponent->SetupAttachment(RootComponent);

	BodyMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("BodyMesh"));

	SphereComponent->InitSphereRadius(.05f);
	SphereComponent->SetCollisionProfileName(TEXT("Pawn"));

	AutoPossessPlayer = EAutoReceiveInput::Player0;
	OurMovementComponent = CreateDefaultSubobject<UControllerPawnMovementComponent>(TEXT("CustomMovementComponent"));
	OurMovementComponent->UpdatedComponent = RootComponent;
}

// Called when the game starts or when spawned
void AMotionControllerPawnParent::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AMotionControllerPawnParent::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AMotionControllerPawnParent::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	Super::SetupPlayerInputComponent(InputComponent);

	InputComponent->BindAxis("MoveForward", this, &AMotionControllerPawnParent::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &AMotionControllerPawnParent::MoveRight);
	InputComponent->BindAxis("Turn", this, &AMotionControllerPawnParent::Turn);
}

FVector AMotionControllerPawnParent::ReturnRightHandLocation()
{
	return RightHandLocation;

}

FVector AMotionControllerPawnParent::ReturnLeftHandLocation()
{
	return LeftHandLocation;

}

FRotator AMotionControllerPawnParent::ReturnRightHandRotation()
{
	return RightHandRotation;

}

FRotator AMotionControllerPawnParent::ReturnLeftHandRotation()
{
	return LeftHandRotation;

}

void AMotionControllerPawnParent::MoveForward(float AxisValue)
{
	if (OurMovementComponent && (OurMovementComponent->UpdatedComponent == RootComponent))
	{
		OurMovementComponent->AddInputVector(GetActorForwardVector() * AxisValue);
	}
}

void AMotionControllerPawnParent::MoveRight(float AxisValue)
{
	if (OurMovementComponent && (OurMovementComponent->UpdatedComponent == RootComponent))
	{
		OurMovementComponent->AddInputVector(GetActorRightVector() * AxisValue);
	}
}

void AMotionControllerPawnParent::Turn(float AxisValue)
{
	FRotator NewRotation = GetActorRotation();
	NewRotation.Yaw += AxisValue;
	SetActorRotation(NewRotation);
}

UPawnMovementComponent* AMotionControllerPawnParent::GetMovementComponent() const
{
	return OurMovementComponent;
}