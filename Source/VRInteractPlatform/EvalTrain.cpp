// Fill out your copyright notice in the Description page of Project Settings.

#include "VRInteractPlatform.h"
#include "EvalTrain.h"

AEvalTrain::AEvalTrain()
{
	// DefaultPawnClass = AIKPawn::StaticClass();
	PoseData = "";
	PoseRecord = false;
	TrainRes = "";
	RecordInterval = 0.01;
	TimeStart = 0;
}

void AEvalTrain::BeginPlay()
{
	PrimaryActorTick.bCanEverTick = true;

	Super::BeginPlay();
	UWorld* World = GetWorld();
	FString GameDir = FPaths::GameDir();
	FString FileName1 = GameDir + "XAI_record/orange_juice/1.txt";
	FString FileName2 = GameDir + "XAI_record/orange_juice/2.txt";
	FString FileName3_1 = GameDir + "XAI_record/orange_juice/3_1.txt";
	FString FileName3_2 = GameDir + "XAI_record/orange_juice/3_2.txt";
	FString FileName3_3 = GameDir + "XAI_record/orange_juice/3_3.txt";
	FString FileName4 = GameDir + "XAI_record/orange_juice/4.txt";
	FString FileName5_1 = GameDir + "XAI_record/orange_juice/5_1.txt";
	FString FileName5_2 = GameDir + "XAI_record/orange_juice/5_2.txt";
	FString FileName5_3 = GameDir + "XAI_record/orange_juice/5_3.txt";
	OpenDoorFlag = false;
	ShowMenu = -1;

	if (FPaths::FileExists(FileName1))
	{
		FString FileData = "";
		FFileHelper::LoadFileToString(FileData, *FileName1);

		int32 FieldCount = FileData.ParseIntoArray(ApplyPoseArray1, TEXT("\n"), true);
		UE_LOG(LogTemp, Warning, TEXT("file 1 record count: %d"), FieldCount);
	}

	if (FPaths::FileExists(FileName2))
	{
		FString FileData = "";
		FFileHelper::LoadFileToString(FileData, *FileName2);

		int32 FieldCount = FileData.ParseIntoArray(ApplyPoseArray2, TEXT("\n"), true);
		UE_LOG(LogTemp, Warning, TEXT("file 2 record count: %d"), FieldCount);
	}

	if (FPaths::FileExists(FileName3_1))
	{
		FString FileData = "";
		FFileHelper::LoadFileToString(FileData, *FileName3_1);

		int32 FieldCount = FileData.ParseIntoArray(ApplyPoseArray3_1, TEXT("\n"), true);
		UE_LOG(LogTemp, Warning, TEXT("file 3_1 record count: %d"), FieldCount);
	}

	if (FPaths::FileExists(FileName3_2))
	{
		FString FileData = "";
		FFileHelper::LoadFileToString(FileData, *FileName3_2);

		int32 FieldCount = FileData.ParseIntoArray(ApplyPoseArray3_2, TEXT("\n"), true);
		UE_LOG(LogTemp, Warning, TEXT("file 3_2 record count: %d"), FieldCount);
	}

	if (FPaths::FileExists(FileName3_3))
	{
		FString FileData = "";
		FFileHelper::LoadFileToString(FileData, *FileName3_3);

		int32 FieldCount = FileData.ParseIntoArray(ApplyPoseArray3_3, TEXT("\n"), true);
		UE_LOG(LogTemp, Warning, TEXT("file 3_3 record count: %d"), FieldCount);
	}

	if (FPaths::FileExists(FileName4))
	{
		FString FileData = "";
		FFileHelper::LoadFileToString(FileData, *FileName4);

		int32 FieldCount = FileData.ParseIntoArray(ApplyPoseArray4, TEXT("\n"), true);
		UE_LOG(LogTemp, Warning, TEXT("file 4 record count: %d"), FieldCount);
	}

	if (FPaths::FileExists(FileName5_1))
	{
		FString FileData = "";
		FFileHelper::LoadFileToString(FileData, *FileName5_1);

		int32 FieldCount = FileData.ParseIntoArray(ApplyPoseArray5_1, TEXT("\n"), true);
		UE_LOG(LogTemp, Warning, TEXT("file 5_1 record count: %d"), FieldCount);
	}

	if (FPaths::FileExists(FileName5_2))
	{
		FString FileData = "";
		FFileHelper::LoadFileToString(FileData, *FileName5_2);

		int32 FieldCount = FileData.ParseIntoArray(ApplyPoseArray5_2, TEXT("\n"), true);
		UE_LOG(LogTemp, Warning, TEXT("file 5_2 record count: %d"), FieldCount);
	}

	if (FPaths::FileExists(FileName5_3))
	{
		FString FileData = "";
		FFileHelper::LoadFileToString(FileData, *FileName5_3);

		int32 FieldCount = FileData.ParseIntoArray(ApplyPoseArray5_3, TEXT("\n"), true);
		UE_LOG(LogTemp, Warning, TEXT("file 5_3 record count: %d"), FieldCount);
	}

	RecordApplied1 = 0;
	RecordApplied2 = 0;
	RecordApplied3 = 0;
	RecordApplied4 = 0;
	RecordApplied5 = 0;

	for (TObjectIterator<AIKPawn> ActorItr; ActorItr; ++ActorItr)
		HumanPawn = *ActorItr;

	if (HumanPawn == NULL)
		UE_LOG(LogTemp, Warning, TEXT("Can't find human pawn"));

	for (TObjectIterator<ATouchAnimateActor> ActorItr; ActorItr; ++ActorItr)
	{

		MachineActor = *ActorItr;

	}

	for (TObjectIterator<AOnOffObject> ActorItr; ActorItr; ++ActorItr)
	{
		if (ActorItr->GetName() == TEXT("CabinetLeftDoor"))
		{
			LeftDoor = *ActorItr;
		}

	}

	FTimerHandle ReceiverHandler;
	GetWorldTimerManager().SetTimer(ReceiverHandler, this, &AEvalTrain::RecordActors, RecordInterval, true);

}

void AEvalTrain::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	// UE_LOG(LogTemp, Warning,TEXT("Door rotation %s"), *(Door->GetActorRotation().ToString()));
	// RecordActors();

}

void AEvalTrain::RecordActors()
{
	/*
	if (HumanPawn)
	HumanPawn->UpdateAnim(PoseData);
	*/

	if (HumanPawn->ActionToTake == -1)
	{
		ShowMenu = 0;
	}
	else if (TimeStart == 0)
	{
		TimeStart = FPlatformTime::Seconds();
	}
	else if (RecordApplied1 < ApplyPoseArray1.Num())
	{

		HumanRecord = ApplyPoseArray1[RecordApplied1];
		MachineActor->UpdateAnim(HumanRecord);
		RecordApplied1++;
	}
	else if (ShowMenu == 0)
	{
		HumanPawn->ShowMenuFlag = true;
		ShowMenu = 1;
	}
	else if (HumanPawn->ActionToTake == 1)
	{
		if (RecordApplied2 < ApplyPoseArray2.Num())
		{
			HumanRecord = ApplyPoseArray2[RecordApplied2];
			MachineActor->UpdateAnim(HumanRecord);
			RecordApplied2++;
		}
		else if (RecordApplied3 < ApplyPoseArray3_3.Num())
		{
			HumanRecord = ApplyPoseArray3_3[RecordApplied3];
			MachineActor->UpdateAnim(HumanRecord);
			RecordApplied3++;
		}
		else if (RecordApplied4 < ApplyPoseArray4.Num())
		{
			HumanRecord = ApplyPoseArray4[RecordApplied4];
			MachineActor->UpdateAnim(HumanRecord);
			RecordApplied4++;
		}
		else if (ShowMenu == 1)
		{
			HumanPawn->ShowMenuFlag = true;
			ShowMenu = 2;
		}
	}
	else if (HumanPawn->ActionToTake == 2)
	{
		if (RecordApplied2 < ApplyPoseArray2.Num())
		{
			HumanRecord = ApplyPoseArray2[RecordApplied2];
			MachineActor->UpdateAnim(HumanRecord);
			RecordApplied2++;
		}
		else if (RecordApplied3 < ApplyPoseArray3_2.Num())
		{
			HumanRecord = ApplyPoseArray3_2[RecordApplied3];
			MachineActor->UpdateAnim(HumanRecord);
			RecordApplied3++;
		}
		else if (RecordApplied4 < ApplyPoseArray4.Num())
		{
			HumanRecord = ApplyPoseArray4[RecordApplied4];
			MachineActor->UpdateAnim(HumanRecord);
			RecordApplied4++;
		}
		else if (ShowMenu == 1)
		{
			HumanPawn->ShowMenuFlag = true;
			ShowMenu = 2;
		}
	}
	else if (HumanPawn->ActionToTake == 3)
	{
		if (OpenDoorFlag == false)
		{
			OpenDoorFlag = true;
			FRotator DoorOpenRot(0, 180, 0);
			LeftDoor->GetStaticMeshComponent()->SetRelativeRotation(DoorOpenRot);
		}
		if (RecordApplied2 < ApplyPoseArray2.Num())
		{
			HumanRecord = ApplyPoseArray2[RecordApplied2];
			MachineActor->UpdateAnim(HumanRecord);
			RecordApplied2++;
		}
		else if (RecordApplied3 < ApplyPoseArray3_1.Num())
		{
			HumanRecord = ApplyPoseArray3_1[RecordApplied3];
			MachineActor->UpdateAnim(HumanRecord);
			RecordApplied3++;
		}
		else if (RecordApplied4 < ApplyPoseArray4.Num())
		{
			HumanRecord = ApplyPoseArray4[RecordApplied4];
			MachineActor->UpdateAnim(HumanRecord);
			RecordApplied4++;
		}
		else if (ShowMenu == 1)
		{
			HumanPawn->ShowMenuFlag = true;
			ShowMenu = 2;
		}
	}

	else if (HumanPawn->ActionToTake == 4)
	{
		if (RecordApplied5 < ApplyPoseArray5_2.Num())
		{
			HumanRecord = ApplyPoseArray5_2[RecordApplied5];
			MachineActor->UpdateAnim(HumanRecord);
			RecordApplied5++;
		}

	}
	else if (HumanPawn->ActionToTake == 5)
	{
		if (RecordApplied5 < ApplyPoseArray5_1.Num())
		{
			HumanRecord = ApplyPoseArray5_1[RecordApplied5];
			MachineActor->UpdateAnim(HumanRecord);
			RecordApplied5++;
		}

	}
	else if (HumanPawn->ActionToTake == 6)
	{
		if (RecordApplied5 < ApplyPoseArray5_3.Num())
		{
			HumanRecord = ApplyPoseArray5_3[RecordApplied5];
			MachineActor->UpdateAnim(HumanRecord);
			RecordApplied5++;
		}

	}

	/*
	if (RecordApplied2 < ApplyPoseArray2.Num())
	{
	FString HumanRecord2 = ApplyPoseArray2[RecordApplied2];
	MachineActor->UpdateAnim(HumanRecord2);
	RecordApplied2++;
	}
	*/
}

void AEvalTrain::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// Save evaluation result
	Super::EndPlay(EndPlayReason);
	TimeUsed = FPlatformTime::Seconds() - TimeStart;
	FString GameDir = FPaths::GameDir();
	FString SaveFileName = GameDir + "TrainResult" + ".txt";
	for (auto& ans : HumanPawn->AnsArr)
	{
		TrainRes += std::to_string(ans).c_str();
		TrainRes += "\n";
	}

	TrainRes += "\n";

	for (auto& ans : HumanPawn->NumExpArr)
	{
		TrainRes += std::to_string(ans).c_str();
		TrainRes += "\n";
	}

	TrainRes += std::to_string(TimeUsed).c_str();

	FFileHelper::SaveStringToFile(TrainRes, *SaveFileName, FFileHelper::EEncodingOptions::AutoDetect, &IFileManager::Get());
}

