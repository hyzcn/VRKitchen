// Fill out your copyright notice in the Description page of Project Settings.

#include "VRInteractPlatform.h"
#include "TouchGameModeRecord.h"

static Value VectorMaker(FVector vec, Document &doc)
{
	Value oj_vec(kObjectType);

	oj_vec.AddMember("X", vec.X, doc.GetAllocator());
	oj_vec.AddMember("Y", vec.Y, doc.GetAllocator());
	oj_vec.AddMember("Z", vec.Z, doc.GetAllocator());

	return oj_vec;
}

static Value RotatorMaker(FRotator rot, Document &doc)
{
	Value oj_rot(kObjectType);

	oj_rot.AddMember("Pitch", rot.Pitch, doc.GetAllocator());
	oj_rot.AddMember("Yaw", rot.Yaw, doc.GetAllocator());
	oj_rot.AddMember("Roll", rot.Roll, doc.GetAllocator());

	return oj_rot;
}

ATouchGameModeRecord::ATouchGameModeRecord()
{
	// DefaultPawnClass = AIKPawn::StaticClass();
	PoseData = "";
	ObjData = "";
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
		FileNameObj = GameDir + "ObjFluentRecord.csv";

		if (FPaths::FileExists(FileName))
		{
			FPlatformFileManager::Get().GetPlatformFile().DeleteFile(*FileName);
		}

		if (FPaths::FileExists(FileNameObj))
		{
			FPlatformFileManager::Get().GetPlatformFile().DeleteFile(*FileNameObj);
		}
	}

	for (TObjectIterator<ABaxterRobot> ActorItr; ActorItr; ++ActorItr)
		BaxterRobot = *ActorItr;

	for (TObjectIterator<AIKPawn> ActorItr; ActorItr; ++ActorItr)
		HumanPawn = *ActorItr;

	for (TObjectIterator<ACoffeMakerActor> ActorItr; ActorItr; ++ActorItr)
		CofMaker = *ActorItr;

	for (TObjectIterator<APourContainer> ActorItr; ActorItr; ++ActorItr)
		Cup = *ActorItr;

	for (TObjectIterator<APickCookCutObject> ActorItr; ActorItr; ++ActorItr)
	{
		Carrot = *ActorItr;
		TArray<UProceduralMeshComponent*> ProcComps;
		Carrot->GetComponents(ProcComps);
		CarrotMesh = ProcComps[0];
	}
		

	for (TObjectIterator<AOnOffObject> ActorItr; ActorItr; ++ActorItr)
	{
		if (ActorItr->GetName() == TEXT("Stove"))
			Stove = *ActorItr;

		if (ActorItr->GetName() == TEXT("Cabinet_leftdoor"))
			CabinetDoor = *ActorItr;
	}

	for (TObjectIterator<AStaticMeshActor> ActorItr; ActorItr; ++ActorItr)
	{
		if (ActorItr->GetName() == TEXT("Plate"))
			Plate = *ActorItr;

		if (ActorItr->GetName() == TEXT("Knife"))
			Knife = *ActorItr;
	}



	if (HumanPawn == NULL)
		UE_LOG(LogTemp, Warning, TEXT("Can't find human pawn"));

	if (BaxterRobot == NULL)
	{
		UE_LOG(LogTemp, Warning, TEXT("Can't find robot"));
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

		RecordObjData(ObjData);
	}

}

void ATouchGameModeRecord::RecordObjData(FString &Pose)
{
	Value ObjectData(kObjectType);
	Value CarrotData(kObjectType);
	Value CabinetData(kObjectType);
	Value StoveData(kObjectType);
	Value CofMakerData(kObjectType);
	Value CupData(kObjectType);

	if (Carrot && CarrotMesh)
	{
		CarrotData.AddMember("Cut", Carrot->cut, doc.GetAllocator());
		CarrotData.AddMember("Cook", Carrot->cook, doc.GetAllocator());
		Value CarrotPose(kObjectType);
		CarrotPose.AddMember("Loc", VectorMaker(CarrotMesh->GetComponentLocation(), doc), doc.GetAllocator());
		CarrotPose.AddMember("Rot", RotatorMaker(CarrotMesh->GetComponentRotation(), doc), doc.GetAllocator());
		CarrotData.AddMember("Pose", CarrotPose, doc.GetAllocator());

	}
	ObjectData.AddMember("Carrot", CarrotData, doc.GetAllocator());

	if (CabinetDoor)
	{
		CabinetData.AddMember("Open", CabinetDoor->on, doc.GetAllocator());
	}
	ObjectData.AddMember("Cabinet", CabinetData, doc.GetAllocator());

	if (Stove)
	{
		StoveData.AddMember("On", Stove->on, doc.GetAllocator());
	}
	ObjectData.AddMember("Stove", StoveData, doc.GetAllocator());

	if (CofMaker)
	{
		CofMakerData.AddMember("Power", CofMaker->GetPowerStatus(), doc.GetAllocator());
		CofMakerData.AddMember("IsPouring", CofMaker->GetIsPouringCoffee(), doc.GetAllocator());
		CofMakerData.AddMember("IsBrewing", CofMaker->GetIsBrewing(), doc.GetAllocator());
	}
	ObjectData.AddMember("CoffeeMaker", CofMakerData, doc.GetAllocator());

	if (Cup)
	{
		CupData.AddMember("FillFraction", Cup->CurrentFillFraction, doc.GetAllocator());
	}
	ObjectData.AddMember("Cup", CupData, doc.GetAllocator());


	StringBuffer buffer;
	Writer<StringBuffer> writer(buffer);
	ObjectData.Accept(writer);
	std::string str = buffer.GetString();
	FString data(str.c_str());

	Pose += data + "\n";
}

void ATouchGameModeRecord::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	if (PoseRecord)
	{
		FFileHelper::SaveStringToFile(PoseData, *FileName, FFileHelper::EEncodingOptions::AutoDetect, &IFileManager::Get());
		FFileHelper::SaveStringToFile(ObjData, *FileNameObj, FFileHelper::EEncodingOptions::AutoDetect, &IFileManager::Get());
	}
}

