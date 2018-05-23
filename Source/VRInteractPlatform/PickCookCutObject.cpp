// Fill out your copyright notice in the Description page of Project Settings.

#include "VRInteractPlatform.h"
#include "PickCookCutObject.h"


// Sets default values
APickCookCutObject::APickCookCutObject()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	CanChangeColor = false;
	Cooked = false;
	Cut = false;
	CanSqueeze = false;
}

// Called when the game starts or when spawned
void APickCookCutObject::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APickCookCutObject::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

