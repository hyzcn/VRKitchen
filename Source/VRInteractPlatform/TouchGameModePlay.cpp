// Fill out your copyright notice in the Description page of Project Settings.

#include "VRInteractPlatform.h"
#include "TouchGameModePlay.h"

void ATouchGameModePlay::BeginPlay()
{
	Super::BeginPlay();
	UWorld* World = GetWorld();
	FString SegmentPoints = "";
	FString GameDir = FPaths::GameDir();
	FString FileName = GameDir + "HumanPoseRecord2.csv";
	IsPause = false;
	RecordSegment = true;
	UpdateBaxter = false;

	if (FPaths::FileExists(FileName))
	{
		FString FileData = "";
		FFileHelper::LoadFileToString(FileData, *FileName);

		int32 FieldCount = FileData.ParseIntoArray(ApplyPoseArray, TEXT("\n"), true);
		UE_LOG(LogTemp, Warning, TEXT("record count: %d"), FieldCount);
	}

	if (World)
	{
		World->GetFirstPlayerController()->InputComponent->BindAction("SegmentRecord", IE_Pressed, this, &ATouchGameModePlay::SegmentRecord);
		World->GetFirstPlayerController()->InputComponent->BindAction("Pause", IE_Pressed, this, &ATouchGameModePlay::TriggerPause);
	}
		

	for (TObjectIterator<ABaxterRobot> ActorItr; ActorItr; ++ActorItr)
		BaxterRobot = *ActorItr;

	for (TObjectIterator<ATouchAnimateActor> ActorItr; ActorItr; ++ActorItr)
		HumanPawn = *ActorItr;

	if (HumanPawn == NULL)
		UE_LOG(LogTemp, Warning, TEXT("Can't find human pawn"));
	if (BaxterRobot == NULL)
		UE_LOG(LogTemp, Warning, TEXT("Can't find robot"));

	UpdateInterval = 0.01;
	RecordApplied = 0;

	FTimerHandle ReceiverHandler;
	GetWorldTimerManager().SetTimer(ReceiverHandler, this, &ATouchGameModePlay::UpdateAnim, UpdateInterval, true);
}

void ATouchGameModePlay::TriggerPause()
{
	IsPause = !IsPause;
}

void ATouchGameModePlay::UpdateAnim()
{
	// update human action
	if (IsPause)
		return;
	if (RecordApplied < ApplyPoseArray.Num())
	{
		FString HumanRecord = ApplyPoseArray[RecordApplied];
		HumanPawn->UpdateAnim(HumanRecord);
		RecordApplied++;
		// UE_LOG(LogTemp, Warning, TEXT("Applied record: %s"), RecordApplied);
	}

	if (UpdateBaxter)
		// update robot action
		if (RecordApplied < ApplyPoseArray.Num())
		{
			FString BaxterRecord = ApplyPoseArray[RecordApplied];
			BaxterRobot->UpdateAnim(BaxterRecord);
			RecordApplied++;
			// UE_LOG(LogTemp, Warning, TEXT("Applied record: %d"), RecordApplied);
		}
}

void ATouchGameModePlay::SegmentRecord()
{

	UE_LOG(LogTemp, Warning, TEXT("record segment place: %d"), RecordApplied);
	SegmentPoints += std::to_string(RecordApplied).c_str();
	SegmentPoints += "\n";

}

void ATouchGameModePlay::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (RecordSegment)
	{
		Super::EndPlay(EndPlayReason);
		FString GameDir = FPaths::GameDir();
		FString SaveFileName = GameDir + "HumanBaxterPoseSegPt" + ".csv";
		FFileHelper::SaveStringToFile(SegmentPoints, *SaveFileName, FFileHelper::EEncodingOptions::AutoDetect, &IFileManager::Get());
	}
	
}


