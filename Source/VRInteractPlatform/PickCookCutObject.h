// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "Materials/Material.h"
#include "ProceduralMeshComponent.h"
#include "PickCookCutObject.generated.h"

UCLASS()
class VRINTERACTPLATFORM_API APickCookCutObject : public AStaticMeshActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APickCookCutObject();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Objects")
		bool cook;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Objects")
		bool cut;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Objects")
		bool ChangeColor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Objects")
		bool CanSqueeze;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Objects")
		FLinearColor SqueezeColor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Objects")
		UMaterial* CapMat;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Objects")
		UMaterial* SourceMat;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Objects")
		TArray<UProceduralMeshComponent*> ProcMeshes;
	
};
