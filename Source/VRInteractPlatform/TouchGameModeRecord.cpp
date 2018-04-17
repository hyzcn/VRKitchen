// Fill out your copyright notice in the Description page of Project Settings.

#include "VRInteractPlatform.h"
#include "TouchGameModeRecord.h"

ATouchGameModeRecord::ATouchGameModeRecord()
{
	// DefaultPawnClass = AIKPawn::StaticClass();
	PoseData = "";
	PoseRecord = true;
	RecordInterval = 0.01;
}

void ATouchGameModeRecord::BeginPlay()
{
	Super::BeginPlay();
	UWorld* World = GetWorld();

	PrimaryActorTick.bCanEverTick = true;

	if (PoseRecord)
	{
		GameDir = FPaths::GameDir();
		FileName = GameDir + "HumanBaxterPoseRecord.csv";

		if (FPaths::FileExists(FileName))
		{
			FPlatformFileManager::Get().GetPlatformFile().DeleteFile(*FileName);
		}
	}

	for (TObjectIterator<ABaxterRobot> ActorItr; ActorItr; ++ActorItr)
		BaxterRobot = *ActorItr;
	
	for (TObjectIterator<AIKPawn> ActorItr; ActorItr; ++ActorItr)
		HumanPawn = *ActorItr;

	for (TObjectIterator<AStaticMeshActor> ActorItr; ActorItr; ++ActorItr)
	{
		if (ActorItr->GetName() == TEXT("leftdoor_shifted_3"))
			Door = *ActorItr;
	}

	if (HumanPawn == NULL)
		UE_LOG(LogTemp, Warning, TEXT("Can't find human pawn"));

	if (BaxterRobot == NULL)
	{
		UE_LOG(LogTemp, Warning, TEXT("Can't find robot"));
	}
	
	if (Door == NULL)
	{
		UE_LOG(LogTemp, Warning, TEXT("Can't find door"));
	}

	FTimerHandle ReceiverHandler;
	GetWorldTimerManager().SetTimer(ReceiverHandler, this, &ATouchGameModeRecord::RecordActors, RecordInterval, true);

}

void ATouchGameModeRecord::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	// UE_LOG(LogTemp, Warning,TEXT("Door rotation %s"), *(Door->GetActorRotation().ToString()));
	// RecordActors();
	
}

void ATouchGameModeRecord::RecordActors()
{
	if (PoseRecord)
	{
		if (HumanPawn)
			HumanPawn->UpdateAnim(PoseData);
		if (BaxterRobot)
			BaxterRobot->AnimateRecord(PoseData);
	}


	
}

void ATouchGameModeRecord::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	if (PoseRecord)
	{
		FFileHelper::SaveStringToFile(PoseData, *FileName, FFileHelper::EEncodingOptions::AutoDetect, &IFileManager::Get());
	}
}

