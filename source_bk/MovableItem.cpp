// Fill out your copyright notice in the Description page of Project Settings.

#include "VRInteractPlatform.h"
#include "MovableItem.h"


// Sets default values
AMovableItem::AMovableItem()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MovableMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MovableMesh"));
	RootComponent = MovableMesh;


	MovableMesh->SetMobility(EComponentMobility::Movable);
	MovableMesh->SetEnableGravity(true);

	Item_ID = FString(TEXT("MovableMesh"));

}

// Called when the game starts or when spawned
void AMovableItem::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMovableItem::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

