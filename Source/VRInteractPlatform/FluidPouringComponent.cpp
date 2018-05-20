// Fill out your copyright notice in the Description page of Project Settings.

#include "VRInteractPlatform.h"
#include "PourContainer.h"
#include "FluidPouringComponent.h"

#define POUR_TRACE ECollisionChannel::ECC_GameTraceChannel1

UFluidPouringComponent::UFluidPouringComponent()
{
	QueryRange = 1.f;
}

void UFluidPouringComponent::PourFluid(FLinearColor FluidColor, float PourAmount)
{
	UWorld* TheWorld = this->GetWorld();

	TArray<FHitResult> OutResults;
	FCollisionShape GrabSphere = FCollisionShape::MakeSphere(QueryRange);
	FCollisionObjectQueryParams ObjectParams;
	FCollisionQueryParams CollisionParams;
	ObjectParams.AddObjectTypesToQuery(ECollisionChannel::ECC_WorldDynamic);
	ObjectParams.AddObjectTypesToQuery(ECollisionChannel::ECC_PhysicsBody);
	ObjectParams.AddObjectTypesToQuery(ECollisionChannel::ECC_Visibility);
	CollisionParams.bFindInitialOverlaps = false;
	CollisionParams.AddIgnoredActor(this->GetOwner());
	TheWorld->SweepMultiByChannel(OutResults, GetComponentLocation(), GetComponentLocation() + FVector(0, 0, -200), FQuat::Identity, POUR_TRACE, GrabSphere, CollisionParams);
	//DrawDebugLine(
	//	TheWorld,
	//	GetComponentLocation(),
	//	GetComponentLocation() + FVector(0, 0, -200),
	//	FColor(255, 0, 0),
	//	false, -1, 0,
	//	3
	//);

	for (int i = 0; i < OutResults.Num(); i++)
	{
		//DrawDebugSphere(TheWorld, OutResults[i].ImpactPoint, 10.f, 8, FColor(255, 0, 0), true);
		APourContainer* ReceivingContainer = Cast<APourContainer>(OutResults[i].GetActor());
		if (ReceivingContainer)
		{
			ReceivingContainer->RecieveFluid(FluidColor, PourAmount, FluidKind);
			break;
		}

		UE_LOG(LogTemp, Warning, TEXT("Pouring num %d : %s"), i, *AActor::GetDebugName(OutResults[i].GetActor()));
		UE_LOG(LogTemp, Warning, TEXT("Pouring num %d : %s"), i, *OutResults[i].GetComponent()->GetReadableName());
	}
}





