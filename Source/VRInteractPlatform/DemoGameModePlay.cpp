// Fill out your copyright notice in the Description page of Project Settings.

#include "VRInteractPlatform.h"
#include "DemoGameModePlay.h"



void ADemoGameModePlay::BeginPlay()
{
	Super::BeginPlay();
	UWorld* World = GetWorld();
	FString SegmentPoints = "";
	FString GameDir = FPaths::GameDir();
	FString FileName1 = GameDir + "HumanPoseRecord1.csv";
	FString FileName2 = GameDir + "HumanPoseRecord2.csv";
	IsPause = false;
	RecordSegment = false;

	if (FPaths::FileExists(FileName1))
	{
		FString FileData = "";
		FFileHelper::LoadFileToString(FileData, *FileName1);

		int32 FieldCount = FileData.ParseIntoArray(ApplyPoseArray1, TEXT("\n"), true);
		UE_LOG(LogTemp, Warning, TEXT("record count: %d"), FieldCount);
	}

	if (FPaths::FileExists(FileName2))
	{
		FString FileData = "";
		FFileHelper::LoadFileToString(FileData, *FileName2);

		int32 FieldCount = FileData.ParseIntoArray(ApplyPoseArray2, TEXT("\n"), true);
		UE_LOG(LogTemp, Warning, TEXT("record count: %d"), FieldCount);
	}

	if (World)
	{
		World->GetFirstPlayerController()->InputComponent->BindAction("SegmentRecord", IE_Pressed, this, &ADemoGameModePlay::SegmentRecord);
		World->GetFirstPlayerController()->InputComponent->BindAction("Pause", IE_Pressed, this, &ADemoGameModePlay::TriggerPause);
	}
	for (TObjectIterator<ATouchAnimateActor> ActorItr; ActorItr; ++ActorItr)
	{
		if (ActorItr->GetName() == TEXT("TouchAnimateActorBP_2"))
		{
			HumanPawn1 = *ActorItr;
		}
		if (ActorItr->GetName() == TEXT("TouchAnimateActorBP2_2"))
		{
			HumanPawn2 = *ActorItr;
		}

	}
		

	if (HumanPawn1 == NULL || HumanPawn2 == NULL || HumanPawn1 == HumanPawn2)
		UE_LOG(LogTemp, Warning, TEXT("Can't find human pawn"));

	UpdateInterval = 0.01;
	RecordApplied1 = 0;
	RecordApplied2 = 0;


	FTimerHandle ReceiverHandler;
	GetWorldTimerManager().SetTimer(ReceiverHandler, this, &ADemoGameModePlay::UpdateAnim, UpdateInterval, true);
}

void ADemoGameModePlay::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	// UpdateAnim();
}

void ADemoGameModePlay::TriggerPause()
{
	IsPause = !IsPause;
}

void ADemoGameModePlay::UpdateAnim()
{
	// update human action
	if (IsPause)
		return;

	if (RecordApplied1 < ApplyPoseArray1.Num() && HumanPawn1 != NULL)
	{
		FString HumanRecord1;

		HumanRecord1 = ApplyPoseArray1[RecordApplied1];
		HumanPawn1->UpdateAnim(HumanRecord1);
		RecordApplied1++;
	}

	if (RecordApplied2 < ApplyPoseArray2.Num() && HumanPawn2 != NULL)
	{
		FString HumanRecord2 = ApplyPoseArray2[RecordApplied2];
		HumanPawn2->UpdateAnim(HumanRecord2);
		RecordApplied2++;
	}


}

void ADemoGameModePlay::SegmentRecord()
{

	UE_LOG(LogTemp, Warning, TEXT("record segment place: %d"), RecordApplied1);
	SegmentPoints += std::to_string(RecordApplied1).c_str();
	SegmentPoints += "\n";

}

void ADemoGameModePlay::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (RecordSegment)
	{
		Super::EndPlay(EndPlayReason);
		FString GameDir = FPaths::GameDir();
		FString SaveFileName = GameDir + "HumanBaxterPoseSegPt" + ".csv";
		FFileHelper::SaveStringToFile(SegmentPoints, *SaveFileName, FFileHelper::EEncodingOptions::AutoDetect, &IFileManager::Get());
	}

}

