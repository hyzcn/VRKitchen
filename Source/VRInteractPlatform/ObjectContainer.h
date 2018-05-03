// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ObjectContainer.generated.h"

UCLASS()
class VRINTERACTPLATFORM_API AObjectContainer : public AStaticMeshActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AObjectContainer();
	virtual void Tick(float DeltaTime) override;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ObjectContainer")
		TArray<UPrimitiveComponent*> ContainedObjects;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	
};
