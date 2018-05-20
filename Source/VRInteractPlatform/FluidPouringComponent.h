// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Particles/ParticleSystemComponent.h"
#include "FluidPouringComponent.generated.h"

/**
 * 
 */
UCLASS()
class VRINTERACTPLATFORM_API UFluidPouringComponent : public UParticleSystemComponent
{
	GENERATED_BODY()
	

public:
	UFluidPouringComponent();
	UFUNCTION(BlueprintCallable, Category = "Pouring")
		void PourFluid(FLinearColor FluidColor, float PourAmount);

	UPROPERTY(EditAnywhere, Category = "Pouring")
		FString FluidKind;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pouring")
		float QueryRange;
	
	
};
