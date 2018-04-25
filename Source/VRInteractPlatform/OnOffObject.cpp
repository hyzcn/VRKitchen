// Fill out your copyright notice in the Description page of Project Settings.

#include "VRInteractPlatform.h"
#include "OnOffObject.h"


// Sets default values
AOnOffObject::AOnOffObject()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	on = false;
}

// Called when the game starts or when spawned
void AOnOffObject::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AOnOffObject::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

