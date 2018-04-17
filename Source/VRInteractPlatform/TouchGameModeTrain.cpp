// Fill out your copyright notice in the Description page of Project Settings.

#include "VRInteractPlatform.h"
#include "TouchGameModeTrain.h"

std::string StringFromBinaryArray(const TArray<uint8>& BinaryArray)
{
	std::string cstr(reinterpret_cast<const char*>(BinaryArray.GetData()), BinaryArray.Num());
	// return FString(cstr.c_str());
	return cstr;
}

void InsertBinaryArrayFromString(const FString& Message, TArray<uint8>& OutBinaryArray)
{
	FTCHARToUTF8 Convert(*Message);
	// const TArray<TCHAR>& CharArray = Message.GetCharArray();
	// OutBinaryArray.Append(CharArray);
	// This can work, but will add tailing \0 also behavior is not well defined.

	OutBinaryArray.Append((UTF8CHAR*)Convert.Get(), Convert.Length());
}

ATouchGameModeTrain::ATouchGameModeTrain()
{
	StepInterval = 0.01;
	Done = false;
	delim = "\nEND OF FILE\n";
	Goal = FVector(-1000, -100, 145);
	Reward = 0.0;
	PrimaryActorTick.bStartWithTickEnabled = true;
	PrimaryActorTick.bCanEverTick = true;
	DataToSend = false;
	ResetLevel = false;
	UpdateHuman = false;
	HumanRecord = "";
	StateData = "";
}

void ATouchGameModeTrain::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	// CheckDone();

	if (DataToSend)
	{
		DepthData = CapComp->CaptureNpyFloat16(FString(TEXT("depth")), 1);
		RGBImage = CapComp->CaptureNpyUint8(FString(TEXT("lit")), 3);

		Frame.Empty();
		InsertBinaryArrayFromString(FString("Depth"), Frame);
		InsertBinaryArrayFromString(delim, Frame);
		Frame.Append(DepthData);
		InsertBinaryArrayFromString(delim, Frame);

		InsertBinaryArrayFromString(FString("RGB"), Frame);
		InsertBinaryArrayFromString(delim, Frame);
		Frame.Append(RGBImage);
		InsertBinaryArrayFromString(delim, Frame);

		InsertBinaryArrayFromString(FString("HumanData"), Frame);
		InsertBinaryArrayFromString(delim, Frame);
		InsertBinaryArrayFromString(HumanRecord, Frame);
		InsertBinaryArrayFromString(delim, Frame);

		CheckDone();
		InsertBinaryArrayFromString(FString("StateData"), Frame);
		InsertBinaryArrayFromString(delim, Frame);
		InsertBinaryArrayFromString(StateData, Frame);
		InsertBinaryArrayFromString(delim, Frame);

		InsertBinaryArrayFromString(FString("Reward"), Frame);
		InsertBinaryArrayFromString(delim, Frame);
		InsertBinaryArrayFromString(FString(std::to_string(Reward).c_str()), Frame);
		InsertBinaryArrayFromString(delim, Frame);

		InsertBinaryArrayFromString(FString("Done"), Frame);
		InsertBinaryArrayFromString(delim, Frame);
		InsertBinaryArrayFromString(Done ? FString("1") : FString("0"), Frame);
		InsertBinaryArrayFromString(delim, Frame);
		Communicator->SendData(Frame);

		DataToSend = false;
	}

	if (ResetLevel)
	{
		ResetLevel = false;
		// FPlatformProcess::Sleep(0.1);
		player->RestartLevel();
		UE_LOG(LogTemp, Warning, TEXT("reset!!!"));
	}
}

void ATouchGameModeTrain::BeginPlay()
{
	Super::BeginPlay();
	/*
	UGameEngine* gameEngine = Cast<UGameEngine>(GEngine);
	if (gameEngine)
	{
		TSharedPtr<SWindow> windowPtr = gameEngine->GameViewportWindow.Pin();
		SWindow *window = windowPtr.Get();

		if (window)
		{
	    	window->Minimize();
		}
	}
	*/
	// set default pawn to invisible
	World = GetWorld();
	player = UGameplayStatics::GetPlayerController(World, 0);
	APawn* pawn = player->GetPawn();
	pawn->SetActorHiddenInGame(true);

	TArray<FString> Modes;
	Modes.Add(TEXT("depth"));
	Modes.Add(TEXT("lit"));
	FString GameDir = FPaths::GameDir();
	FString FileName = GameDir + "HumanBaxterPoseRecord.csv";
	// GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("Game dir %s"), *FileName));

	if (FPaths::FileExists(FileName))
	{
		FString FileData = "";
		FFileHelper::LoadFileToString(FileData, *FileName);

		int32 FieldCount = FileData.ParseIntoArray(ApplyPoseArray, TEXT("\n"), true);
		UE_LOG(LogTemp, Warning, TEXT("record count: %d"), FieldCount);
	}
	
	for (TObjectIterator<ABaxterRobot> ActorItr; ActorItr; ++ActorItr)
		BaxterRobot = *ActorItr;

	for (TObjectIterator<ATouchAnimateActor> ActorItr; ActorItr; ++ActorItr)
		HumanPawn = *ActorItr;

	for (TObjectIterator<ACentralCommunicator> ActorItr; ActorItr; ++ActorItr)
		Communicator = *ActorItr;

	for (TObjectIterator<AStaticMeshActor> ActorItr; ActorItr; ++ActorItr)
	{
		if (ActorItr->GetName() == TEXT("rightdoor_shifted"))
		{
			Door = *ActorItr;
			TArray<UStaticMeshComponent*> children;
			Door->GetComponents(children);
	
			for (int i = 0; i < children.Num(); i++)
			{
				if (children[i])
				{
					UStaticMeshComponent* child = children[i];
					// GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("child name %s"), *child->GetName()));
					if (child->GetName() == TEXT("right_handle"))
					{
						DoorHandle = children[i];
						UE_LOG(LogTemp, Warning, TEXT("Find handle"));
						break;
					}
				}
			}
		}

	}
		
	if (HumanPawn == NULL)
		UE_LOG(LogTemp, Warning, TEXT("Can't find human pawn"));

	if (Communicator == NULL)
		UE_LOG(LogTemp, Warning, TEXT("Can't find communicator"));

	if (BaxterRobot == NULL)
	{
		UE_LOG(LogTemp, Warning, TEXT("Can't find robot"));
	}
	else
	{
		CapComp = UGTCaptureComponent::Create(Cast<AActor>(BaxterRobot), Modes, World, BaxterRobot->CameraRoot);
	}

	if (Door == NULL)
	{
		UE_LOG(LogTemp, Warning, TEXT("Can't find door"));
	}

	RecordApplied = 0;

	GetWorldTimerManager().SetTimer(ReceiverHandler, this, &ATouchGameModeTrain::Step, StepInterval, true);
}

void ATouchGameModeTrain::CheckDone()
{
	FRotator rot = Door->GetActorRotation();
	// GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("Door rotation %s"), *(rot.ToString())));
	if (rot.Yaw < 10.0)
	{
		Done = true;
		Reward = 300.0;
	}
	else
	{
		Done = false;
		Reward = -rot.Yaw/10;
	}

	StateData = "";

	FVector loc = DoorHandle->GetComponentLocation();
	StateData += FString(std::to_string(loc.X).c_str());
	StateData += "\n";
	StateData += FString(std::to_string(loc.Y).c_str());
	StateData += "\n";
	StateData += FString(std::to_string(loc.Z).c_str());
	StateData += "\n";

	FVector loc1 = BaxterRobot->SM_RGripperTipL->GetComponentLocation();
	FVector loc2 = BaxterRobot->SM_RGripperTipR->GetComponentLocation();
	FVector loc_avg = 1.0/2.0*(loc1 + loc2);
	StateData += FString(std::to_string(loc_avg.X).c_str());
	StateData += "\n";
	StateData += FString(std::to_string(loc_avg.Y).c_str());
	StateData += "\n";
	StateData += FString(std::to_string(loc_avg.Z).c_str());
	StateData += "\n";

	//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("state data %s"), *(StateData)));

}

void ATouchGameModeTrain::Step()
{
	int GetNewData = BaxterRobot->ParseData();

	if (GetNewData == 1)
	{
		// update rate
		if (UpdateHuman)
		{
			for (int i = 0;i < 20;i++)
			{
				UpdateAnim();
			}
		}
			
		
		if (CapComp)
		{
			UE_LOG(LogTemp, Warning, TEXT("capture image"));
			DataToSend = true;
			
		}
	
	}
	if (GetNewData == -1)
		ResetLevel = true;
		
}

void ATouchGameModeTrain::UpdateAnim()
{
	// update human action
	if (RecordApplied < ApplyPoseArray.Num())
	{
		HumanRecord = ApplyPoseArray[RecordApplied];
		HumanPawn->UpdateAnim(HumanRecord);
		RecordApplied++;
		
		// UE_LOG(LogTemp, Warning, TEXT("Applied record: %d"), RecordApplied);
	}

	// don't update robot action from file
	if (RecordApplied < ApplyPoseArray.Num())
	{
		RecordApplied++;
		// UE_LOG(LogTemp, Warning, TEXT("Applied record: %d"), RecordApplied);
	}
}


