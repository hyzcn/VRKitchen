// Fill out your copyright notice in the Description page of Project Settings.

#include "VRInteractPlatform.h"
#include "CoffeMakerActor.h"

// Sets default values
ACoffeMakerActor::ACoffeMakerActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bIsOn = false;
	bIsPouringCoffee = false;
	bIsBrewing = false;
	MaterialIndex = 4;
	NozzleSize = 1.f;

	CoffeeMakerMesh = CreateDefaultSubobject<UStaticMeshComponent>(FName("CoffeeMakerMesh"));
	RootComponent = CoffeeMakerMesh;

	PowerButton = CreateDefaultSubobject<USphereComponent>(FName("PowerButton"));
	PourCoffeeButton = CreateDefaultSubobject<USphereComponent>(FName("PourCoffeeButton"));
	PowerButtonLight = CreateDefaultSubobject<UDecalComponent>(FName("PowerButtonLight"));
	CoffeeNozzle = CreateDefaultSubobject<UFluidPouringComponent>(FName("CoffeeNozzle"));

	PowerButton->AttachToComponent(CoffeeMakerMesh, FAttachmentTransformRules::KeepWorldTransform);
	PourCoffeeButton->AttachToComponent(CoffeeMakerMesh, FAttachmentTransformRules::KeepWorldTransform);
	PowerButtonLight->AttachToComponent(PowerButton, FAttachmentTransformRules::KeepWorldTransform);
	CoffeeNozzle->AttachToComponent(CoffeeMakerMesh, FAttachmentTransformRules::KeepWorldTransform);


	//PowerButton->OnComponentBeginOverlap.AddDynamic(this, &ACoffeMakerActor::ToggleMachineOnOff);
	//PourCoffeeButton->OnComponentBeginOverlap.AddDynamic(this, &ACoffeMakerActor::PourCoffee);

	LightTimer = 0;
	CoffeeTimer = 0;
	PourTime = 5;
	BrewTime = 40;
}

// Called when the game starts or when spawned
void ACoffeMakerActor::BeginPlay()
{
	Super::BeginPlay();

	CoffeeMakerMesh->SetMaterial(MaterialIndex, bIsOn ? NeutralOnMaterial : OffMaterial);
	CoffeeNozzle->SetColorParameter(FName("FluidColor"), CoffeeColor);
	CoffeeNozzle->SetFloatParameter(FName("MouthRadius"), NozzleSize);
	CoffeeNozzle->Deactivate();

	Item_ID = FString(TEXT("CoffeeMaker"));
}

// Called every frame
void ACoffeMakerActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!bIsOn)
	{
		LightTimer += DeltaTime;
		if (LightTimer < 2.0)
		{
			PowerButtonLight->SetVisibility(false);
		}
		else if (LightTimer < 2.5)
		{
			PowerButtonLight->SetVisibility(true);
		}
		else
		{
			LightTimer -= 2.5;
		}
	}

	if (bIsPouringCoffee)
	{
		CoffeeTimer += DeltaTime;
		if (CoffeeTimer < BrewTime)
		{
			//do nothing, just brewing
			//UE_LOG(LogTemp, Warning, TEXT("Brewing the coffee!"));
			bIsBrewing = true;
		}
		else if (CoffeeTimer < BrewTime + PourTime)
		{
			CoffeeNozzle->Activate();
			CoffeeNozzle->PourFluid(CoffeeColor, DeltaTime * 0.1);
			bIsBrewing = false;
			//UE_LOG(LogTemp, Warning, TEXT("Pouring the coffee!"));
		}
		else
		{
			CoffeeMakerMesh->SetMaterial(MaterialIndex, NeutralOnMaterial);
			CoffeeNozzle->Deactivate();
			CoffeeTimer = 0.f;
			bIsPouringCoffee = false;
			bIsBrewing = false;
			//UE_LOG(LogTemp, Warning, TEXT("Finished Coffee"));
		}
	}
}

void ACoffeMakerActor::ToggleMachineOnOff(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!bIsPouringCoffee)
	{
		bIsOn = !bIsOn;
		CoffeeMakerMesh->SetMaterial(MaterialIndex, bIsOn ? NeutralOnMaterial : OffMaterial);
		PowerButtonLight->SetVisibility(!bIsOn);
	}

	// UE_LOG(LogTemp, Warning, TEXT("New state is %d: %s"), bIsOn, *OtherActor->GetName());
}

void ACoffeMakerActor::PourCoffee(UPrimitiveComponent * OverlappedComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	//UE_LOG(LogTemp, Warning, TEXT("touched the coffee button"));
	if (bIsOn)
	{
		//UE_LOG(LogTemp, Warning, TEXT("Start making coffee!"));
		bIsPouringCoffee = true;
		CoffeeMakerMesh->SetMaterial(MaterialIndex, PouringMaterial);
	}
}



