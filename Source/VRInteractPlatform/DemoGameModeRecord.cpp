// Fill out your copyright notice in the Description page of Project Settings.

#include "VRInteractPlatform.h"
#include "DemoGameModeRecord.h"


ADemoGameModeRecord::ADemoGameModeRecord()
{
	// DefaultPawnClass = AIKPawn::StaticClass();
	PoseData = "";
	PoseRecord = false;
	RecordInterval = 0.01;
}

void ADemoGameModeRecord::BeginPlay()
{
	Super::BeginPlay();
	UWorld* World = GetWorld();

	PrimaryActorTick.bCanEverTick = true;

	if (PoseRecord)
	{
		GameDir = FPaths::GameDir();
		FileName = GameDir + "HumanPoseRecord2.csv";

		if (FPaths::FileExists(FileName))
		{
			FPlatformFileManager::Get().GetPlatformFile().DeleteFile(*FileName);
		}
	}

	for (TObjectIterator<AIKPawn> ActorItr; ActorItr; ++ActorItr)
		HumanPawn = *ActorItr;

	if (HumanPawn == NULL)
		UE_LOG(LogTemp, Warning, TEXT("Can't find human pawn"));

	FTimerHandle ReceiverHandler;
	GetWorldTimerManager().SetTimer(ReceiverHandler, this, &ADemoGameModeRecord::RecordActors, RecordInterval, true);

}

void ADemoGameModeRecord::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	// UE_LOG(LogTemp, Warning,TEXT("Door rotation %s"), *(Door->GetActorRotation().ToString()));
	// RecordActors();

}

void ADemoGameModeRecord::RecordActors()
{
	if (HumanPawn)
		HumanPawn->UpdateAnim(PoseData);

}

void ADemoGameModeRecord::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	if (PoseRecord)
	{
		FFileHelper::SaveStringToFile(PoseData, *FileName, FFileHelper::EEncodingOptions::AutoDetect, &IFileManager::Get());
	}
}


