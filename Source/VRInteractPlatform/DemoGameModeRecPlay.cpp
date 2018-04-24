// Fill out your copyright notice in the Description page of Project Settings.

#include "VRInteractPlatform.h"
#include "DemoGameModeRecPlay.h"

ADemoGameModeRecPlay::ADemoGameModeRecPlay()
{
	// DefaultPawnClass = AIKPawn::StaticClass();
	PoseData = "";
	PoseRecord = true;
	RecordInterval = 0.01;
}

void ADemoGameModeRecPlay::BeginPlay()
{
	Super::BeginPlay();
	UWorld* World = GetWorld();
	FString GameDir = FPaths::GameDir();
	FString FileName2 = GameDir + "HumanPoseRecord2.csv";

	PrimaryActorTick.bCanEverTick = true;

	if (FPaths::FileExists(FileName2))
	{
		FString FileData = "";
		FFileHelper::LoadFileToString(FileData, *FileName2);

		int32 FieldCount = FileData.ParseIntoArray(ApplyPoseArray2, TEXT("\n"), true);
		UE_LOG(LogTemp, Warning, TEXT("record count: %d"), FieldCount);
	}

	if (PoseRecord)
	{
		GameDir = FPaths::GameDir();
		FileName = GameDir + "HumanPoseRecord1.csv";

		if (FPaths::FileExists(FileName))
		{
			FPlatformFileManager::Get().GetPlatformFile().DeleteFile(*FileName);
		}
	}

	for (TObjectIterator<ATouchAnimateActor> ActorItr; ActorItr; ++ActorItr)
	{
		if (ActorItr->GetName() == TEXT("TouchAnimateActorBP2_5"))
		{
			HumanPawn2 = *ActorItr;
		}

	}

	RecordApplied2 = 0;

	for (TObjectIterator<AIKPawn> ActorItr; ActorItr; ++ActorItr)
		HumanPawn1 = *ActorItr;

	if (HumanPawn1 == NULL)
		UE_LOG(LogTemp, Warning, TEXT("Can't find human pawn"));

	FTimerHandle ReceiverHandler;
	GetWorldTimerManager().SetTimer(ReceiverHandler, this, &ADemoGameModeRecPlay::RecordActors, RecordInterval, true);

}

void ADemoGameModeRecPlay::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	// UE_LOG(LogTemp, Warning,TEXT("Door rotation %s"), *(Door->GetActorRotation().ToString()));
	// RecordActors();

}

void ADemoGameModeRecPlay::RecordActors()
{
	if (PoseRecord)
	{
		if (HumanPawn1)
			HumanPawn1->UpdateAnim(PoseData);
	}

	if (RecordApplied2 < ApplyPoseArray2.Num())
	{
		FString HumanRecord2 = ApplyPoseArray2[RecordApplied2];
		HumanPawn2->UpdateAnim(HumanRecord2);
		RecordApplied2++;
	}

}

void ADemoGameModeRecPlay::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	if (PoseRecord)
	{
		FFileHelper::SaveStringToFile(PoseData, *FileName, FFileHelper::EEncodingOptions::AutoDetect, &IFileManager::Get());
	}
}


