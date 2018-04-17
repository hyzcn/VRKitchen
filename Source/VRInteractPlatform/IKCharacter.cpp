// Fill out your copyright notice in the Description page of Project Settings.

#include "VRInteractPlatform.h"
#include "IKCharacter.h"


// Sets default values
AIKCharacter::AIKCharacter()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	// MovementSpeed = 150;
	// TurnSpeed = .1f;
	UpdateInterval = .01f;
	LeftGrabPress = false;
	RightGrabPress = false;
	LeftHandAttachPoint = FName("hand_lSocket");
	RightHandAttachPoint = FName("hand_rSocket");

}

// Called when the game starts or when spawned
void AIKCharacter::BeginPlay()
{
	Super::BeginPlay();

	FTimerHandle ReceiverHandler;
	GetWorldTimerManager().SetTimer(ReceiverHandler, this, &AIKCharacter::UpdateAnim, UpdateInterval, true);
	
}

// Called every frame
void AIKCharacter::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

// Called to bind functionality to input
void AIKCharacter::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	Super::SetupPlayerInputComponent(InputComponent);
	InputComponent->BindAction("GrabLeft", EInputEvent::IE_Pressed, this, &AIKCharacter::GrabLeft);
	InputComponent->BindAction("GrabLeft", EInputEvent::IE_Released, this, &AIKCharacter::ReleaseLeft);
	InputComponent->BindAction("GrabRight", EInputEvent::IE_Pressed, this, &AIKCharacter::GrabRight);
	InputComponent->BindAction("GrabRight", EInputEvent::IE_Released, this, &AIKCharacter::ReleaseRight);

}


void AIKCharacter::UpdateAnim()
{
	LeftGrab = false; RightGrab = false;
	LeftRelease = false; RightRelease = false;
	FireGrabEventsLeft(LeftGrabPress);
	FireGrabEventsRight(RightGrabPress);
}

void AIKCharacter::Grab(bool IsLeft)
{
	//Spherecast to find all primitive components in grab range
	UWorld* TheWorld = this->GetWorld();

	FVector HandLocation = GetMesh()->GetSocketLocation(IsLeft ? LeftHandAttachPoint : RightHandAttachPoint);
	// DrawDebugSphere(TheWorld, HandLocation, 5.f, 8, FColor(255, 0, 0), true);
	UE_LOG(LogTemp, Warning, TEXT("hand location %s"), *HandLocation.ToString());
	TArray<FHitResult> OutResults;
	FCollisionShape GrabSphere = FCollisionShape::MakeSphere(5.f);
	FCollisionObjectQueryParams ObjectParams;
	FCollisionQueryParams CollisionParams;
	ObjectParams.AddObjectTypesToQuery(ECollisionChannel::ECC_WorldDynamic);
	ObjectParams.AddObjectTypesToQuery(ECollisionChannel::ECC_PhysicsBody);
	CollisionParams.bFindInitialOverlaps = true;
	TheWorld->SweepMultiByObjectType(OutResults, HandLocation, HandLocation + FVector(.1, 0, 0), FQuat(), ObjectParams, GrabSphere, CollisionParams);

	//Calling grab logic on all components in range
	FAttachmentTransformRules GrabRules = FAttachmentTransformRules::KeepWorldTransform;
	GrabRules.bWeldSimulatedBodies = true;
	if (IsLeft)
	{
		for (auto& Hit : OutResults)
		{
			UPrimitiveComponent* Comp = Hit.GetComponent();
			Comp->SetSimulatePhysics(false);

			Comp->AttachToComponent(GetMesh(), GrabRules, LeftHandAttachPoint);
			LeftHandGrabbedComponents.Add(Comp);
		}
	}
	else
	{
		for (auto& Hit : OutResults)
		{

			UPrimitiveComponent* Comp = Hit.GetComponent();
			Comp->SetSimulatePhysics(false);

			Comp->AttachToComponent(GetMesh(), GrabRules, RightHandAttachPoint);
			RightHandGrabbedComponents.Add(Comp);
		}
	}
	//UE_LOG(LogTemp, Warning, TEXT("Grabbing"));
}

void AIKCharacter::Release(bool IsLeft)
{
	if (IsLeft)
	{
		for (auto& Comp : LeftHandGrabbedComponents)
		{
			Comp->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
			Comp->SetSimulatePhysics(true);
		}
		LeftHandGrabbedComponents.Empty();
	}
	else
	{
		for (auto& Comp : RightHandGrabbedComponents)
		{
			Comp->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
			Comp->SetSimulatePhysics(true);
		}
		RightHandGrabbedComponents.Empty();
	}

	//UE_LOG(LogTemp, Warning, TEXT("Releasing"));
}

void AIKCharacter::FireGrabEventsLeft(bool pressed)
{
	if (pressed)
	{
		Grab(true);
		LeftGrab = true;
		LeftRelease = false;
	}

	else
	{
		Release(true);
		LeftRelease = true;
		LeftGrab = false;
	}

}

void AIKCharacter::FireGrabEventsRight(bool pressed)
{
	if (pressed)
	{
		Grab(false);
		RightGrab = true;
		RightRelease = false;

	}

	else
	{
		Release(false);
		RightRelease = true;
		RightGrab = false;
	}

}

void AIKCharacter::GrabLeft()
{
	LeftGrabPress = true;
}

void AIKCharacter::GrabRight()
{
	RightGrabPress = true;
}

void AIKCharacter::ReleaseLeft()
{
	LeftGrabPress = false;
}

void AIKCharacter::ReleaseRight()
{
	RightGrabPress = false;
}