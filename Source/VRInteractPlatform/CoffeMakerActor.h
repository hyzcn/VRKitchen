// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "FluidPouringComponent.h"
#include "CoffeMakerActor.generated.h"

UCLASS()
class VRINTERACTPLATFORM_API ACoffeMakerActor : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ACoffeMakerActor();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaSeconds) override;

	UPROPERTY(EditDefaultsOnly, Category = "CoffeeMaker")
		UStaticMeshComponent* CoffeeMakerMesh;

	UPROPERTY(EditDefaultsOnly, Category = "CoffeeMaker")
		USphereComponent* PowerButton;

	UPROPERTY(EditDefaultsOnly, Category = "CoffeeMaker")
		USphereComponent* PourCoffeeButton;

	UPROPERTY(EditDefaultsOnly, Category = "CoffeeMaker")
		UDecalComponent* PowerButtonLight;

	UPROPERTY(EditDefaultsOnly, Category = "CoffeeMaker")
		UFluidPouringComponent* CoffeeNozzle;

	UPROPERTY(EditAnywhere, Category = "CoffeeMaker")
		UMaterialInterface* OffMaterial;

	UPROPERTY(EditAnywhere, Category = "CoffeeMaker")
		UMaterialInterface* NeutralOnMaterial;

	UPROPERTY(EditAnywhere, Category = "CoffeeMaker")
		UMaterialInterface* PouringMaterial;

	UPROPERTY(EditAnywhere, Category = "CoffeeMaker")
		int MaterialIndex;

	UPROPERTY(EditAnywhere, Category = "CoffeeMaker")
		FLinearColor CoffeeColor;

	UPROPERTY(EditAnywhere, Category = "CoffeeMaker")
		float PourTime;

	UPROPERTY(EditAnywhere, Category = "PourContainer")
		float NozzleSize;

	UFUNCTION(BlueprintCallable, Category = "CoffeeMaker")
		void ToggleMachineOnOff(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION(BlueprintCallable, Category = "CoffeeMaker")
		void PourCoffee(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UPROPERTY(EditAnywhere, Category = "CoffeeMaker", Meta = (BlueprintProtected = "true"))
		FString Item_ID;


	FORCEINLINE bool GetPowerStatus() const {
		return bIsOn;
	}

	FORCEINLINE bool GetIsPouringCoffee() const {
		return bIsPouringCoffee;
	}

private:
	bool bIsOn;
	bool bIsPouringCoffee;
	float LightTimer;
	float CoffeeTimer;
	
};
