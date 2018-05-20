// Fill out your copyright notice in the Description page of Project Settings.

#include "VRInteractPlatform.h"
#include "PourContainer.h"


APourContainer::APourContainer()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	UpArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("ArrowRoot"));

	ContainerMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ContainerMesh"));
	FillMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FillMesh"));
	FluidParticles = CreateDefaultSubobject<UFluidPouringComponent>(TEXT("FluidParticles"));
	RootComponent = ContainerMesh;
	UpArrow->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepWorldTransform);
	FluidParticles->AttachToComponent(UpArrow, FAttachmentTransformRules::KeepWorldTransform);

	// FluidParticles->Deactivate();
	FluidParticles->Activate();

	RemainingFluid = 0.05;
	FlowRate = 0.1;
	HasLid = false;
	CanReceiveFluid = true;
	FluidName = TEXT("fluid");
}

// Called when the game starts or when spawned
void APourContainer::BeginPlay()
{
	Super::BeginPlay();
	if (FillMesh)
	{
		FluidMaterialRef = FillMesh->CreateAndSetMaterialInstanceDynamic(0);
		FluidMaterialRef->SetVectorParameterValue(FName("FluidColor"), FluidColor);
		FluidMaterialRef->SetScalarParameterValue(FName("FillHeight"), CurrentFillFraction);
	}
	
	// FluidMaterialRef->SetScalarParameterValue(FName("FillHeight"), 0.5);

	FluidParticles->SetColorParameter(FName("FluidColor"), FluidColor);
	FluidParticles->SetFloatParameter(FName("MouthRadius"), MouthRadius);
	//RecieveFluid(FluidColor, 0.5, TEXT("Coffee"));


}

// Called every frame
void APourContainer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	MaxFillFraction = FVector::DotProduct(UpArrow->GetForwardVector(), FVector(0, 0, 1)) + RemainingFluid; //Arrow component points in the positive x axis (forward)
																										   //MaxFillFraction = 1 - FVector::CrossProduct(UpArrow->GetForwardVector(), FVector(0, 0, 1)).Size()*MouthDiameter;
	if (MaxFillFraction < CurrentFillFraction && Open)
	{
		if (CurrentFillFraction > 0)
		{
			FluidParticles->PourFluid(FluidColor, FlowRate * DeltaTime);
			CurrentFillFraction = CurrentFillFraction - (FlowRate * DeltaTime);
			FluidParticles->Activate();
		}
		else
		{
			ContainedFluids.Empty();
			FluidParticles->Deactivate();
		}
	}
	else
	{
		FluidParticles->Deactivate();
	}
	if (FluidMaterialRef)
		FluidMaterialRef->SetScalarParameterValue(FName("FillHeight"), CurrentFillFraction);
	//UE_LOG(LogTemp, Warning, TEXT("MaxFillFraction %f"), MaxFillFraction);
}

void APourContainer::RecieveFluid(FLinearColor NewFluidColor, float FluidAmount, FString FluidKind)
{
	// FluidColor = (FluidColor * CurrentFillFraction + NewFluidColor * FluidAmount) / (CurrentFillFraction + FluidAmount);
	if (!CanReceiveFluid)
		return;
	FluidColor = NewFluidColor;
	UE_LOG(LogTemp, Warning, TEXT("Current fill fraction + Fluid amount: %f + %f"), CurrentFillFraction, FluidAmount);
	if (CurrentFillFraction < 1.f)
	{
		CurrentFillFraction += FluidAmount;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Full!"));
	}
	if (FluidMaterialRef)
		FluidMaterialRef->SetVectorParameterValue(FName("FluidColor"), FluidColor);
	FluidParticles->SetColorParameter(FName("FluidColor"), FluidColor);
	ContainedFluids.AddUnique(FluidKind);

}


