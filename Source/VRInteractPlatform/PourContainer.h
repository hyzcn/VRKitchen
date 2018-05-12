// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "FluidPouringComponent.h"
#include "PourContainer.generated.h"

UCLASS()
class VRINTERACTPLATFORM_API APourContainer : public AActor
{
	GENERATED_BODY()
public:
	// Sets default values for this actor's properties
	APourContainer();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaSeconds) override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "PourContainer")
		bool Open;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "PourContainer")
		bool HasLid;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "PourContainer")
		float OrigFillFraction;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "PourContainer")
		FString FluidName;

	UPROPERTY(EditDefaultsOnly, Category = "PourContainer")
		UArrowComponent* UpArrow;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "PourContainer")
		UStaticMeshComponent* ContainerMesh;

	UPROPERTY(EditDefaultsOnly, Category = "PourContainer")
		UStaticMeshComponent* FillMesh;

	UPROPERTY(EditAnywhere, Category = "PourContainer")
		UFluidPouringComponent* FluidParticles;

	UPROPERTY(BlueprintReadOnly, Category = "PourContainer")
		float MaxFillFraction;

	UPROPERTY(EditAnywhere, Category = "PourContainer")
		float CurrentFillFraction;

	UPROPERTY(EditAnywhere, Category = "PourContainer")
		float RemainingFluid;

	UPROPERTY(EditAnywhere, Category = "PourContainer")
		float MouthRadius;

	UPROPERTY(EditAnywhere, Category = "PourContainer")
		float FlowRate;

	UPROPERTY(EditAnywhere, Category = "PourContainer")
		bool bIsInfinite;

	UPROPERTY(EditAnywhere, Category = "PourContainer")
		FLinearColor FluidColor;

	UFUNCTION(BlueprintCallable, Category = "Pouring")
		void RecieveFluid(FLinearColor NewFluidColor, float FluidAmount, FString FluidKind);

	TArray<FString> ContainedFluids;

	UPROPERTY(EditAnywhere, Category = "PourContainer", Meta = (BlueprintProtected = "true"))
		FString Item_ID;

	//UFUNCTION(BlueprintCallable, Category = "Pouring")
	//	void PourFluid(float PourAmount);
private:
	UMaterialInstanceDynamic* FluidMaterialRef;
};
