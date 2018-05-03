// Fill out your copyright notice in the Description page of Project Settings.

#include "VRInteractPlatform.h"
#include "ObjectContainer.h"


// Sets default values
AObjectContainer::AObjectContainer()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AObjectContainer::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AObjectContainer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

