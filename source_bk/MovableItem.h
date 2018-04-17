// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "MovableItem.generated.h"

UCLASS()
class VRINTERACTPLATFORM_API AMovableItem : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AMovableItem();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaSeconds) override;

	UPROPERTY(EditDefaultsOnly, Category = "MovableItem", Meta = (AllowPrivateAccess = "true"))
		class UStaticMeshComponent* MovableMesh;

	UPROPERTY(EditAnywhere, Category = "MovableItem", Meta = (BlueprintProtected = "true"))
		FString Item_ID;

	
};
