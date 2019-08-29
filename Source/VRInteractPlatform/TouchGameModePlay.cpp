// Fill out your copyright notice in the Description page of Project Settings.

#include "VRInteractPlatform.h"
#include "TouchGameModePlay.h"

void ATouchGameModePlay::BeginPlay()
{
	Super::BeginPlay();

	TArray<FString> Modes;
	Modes.Add(TEXT("depth"));
	Modes.Add(TEXT("lit"));
	Modes.Add(TEXT("object_mask"));

	UWorld* World = GetWorld();
	FString SegmentPoints = "";
	GameDir = FPaths::GameDir();
	FString FileName = GameDir + "abcde.csv";
	IsPause = false;
	RecordSegment = true;

	/*
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor::StaticClass(), FoundActors);
	
	for (AActor* TActor : FoundActors)
	{

		if (TActor != nullptr)
		{
			bool hasColor;
			FColor color;
			do {
				color = GenerateColor();
				hasColor = ColorActorTable.Contains(color);

			} while (hasColor == true);

			ColorActorTable.Emplace(color, TActor);
			PaintVertexColor(TActor, color);
		}
	}

	*/

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


	for (TObjectIterator<ATouchAnimateActor> ActorItr; ActorItr; ++ActorItr)
		HumanPawn = *ActorItr;

	for (TObjectIterator<ACameraActor> ActorItr; ActorItr; ++ActorItr)
		if (ActorItr->GetName() == "CameraActor")
			Cam = *ActorItr;

	if (Cam == NULL)
	{
		UE_LOG(LogTemp, Warning, TEXT("Can't find camera"));
	}
	else if (CapComp == NULL)
	{
		CapComp = UGTCaptureComponents::Create(Cast<AActor>(Cam), Modes, World,
			Cam->GetDefaultAttachComponent());
		// CapComp = UGTCaptureComponents::Create(Cast<AActor>(HumanPawn), Modes, World, HumanPawn->CameraRoot);
	}

	if (HumanPawn == NULL)
		UE_LOG(LogTemp, Warning, TEXT("Can't find human pawn"));

	UpdateInterval = 0.1;
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
		//TakeScreenShot();
		//recordRgb();
		// recordMask();
		// recordDepth();
		RecordApplied += 10;
		// UE_LOG(LogTemp, Warning, TEXT("Applied record: %s"), RecordApplied);
	}

}
void ATouchGameModePlay::recordMask()
{
	mask.Add(MASK(CapComp->CapturePng(FString(TEXT("object_mask"))), RecordApplied));
	//FString temp = FString::FromInt(RecordApplied);
	//FString Filename = FPaths::Combine(*GameDir, FString("mask/sequence_") + temp + FString(".png"));
	//CapComp->Capture("mask", Filename);
}

void ATouchGameModePlay::recordRgb()
{
	rgb.Add(RGB(CapComp->CapturePng(FString(TEXT("lit"))), RecordApplied));
	//FString temp = FString::FromInt(RecordApplied);
	//FString Filename = FPaths::Combine(*GameDir, FString("normal/sequence_") + temp + FString(".png"));
	//CapComp->Capture("lit", Filename);
}

void ATouchGameModePlay::recordDepth()
{
	depth.Add(DEPTH(CapComp->CaptureExr(FString(TEXT("depth"))), RecordApplied));
	//FString temp = FString::FromInt(RecordApplied);
	//FString Filename = FPaths::Combine(*GameDir, FString("depth/sequence_") + temp + FString(".png"));
	//CapComp->Capture("depth", Filename);

}

void ATouchGameModePlay::TakeScreenShot()
{
	FString FullFilename;
	char s[4];
	sprintf(s, "%04d", RecordApplied);
	FString temp(s);
	FullFilename = FPaths::Combine(*GameDir, FString("normal/sequence_") + temp + FString(".png"));
	FScreenshotRequest::RequestScreenshot(FullFilename, false, false);
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

	for (auto it : rgb)
	{
		char s[4];
		sprintf(s, "%04d", it.count);
		FString temp(s);
		FString Filename = FPaths::Combine(*GameDir, FString("normal/sequence_") + temp + FString(".png"));
		FFileHelper::SaveArrayToFile(it.rgb, *Filename);
	}

	for (auto it : mask)
	{
		char s[4];
		sprintf(s, "%04d", it.count);
		FString temp(s);
		FString Filename = FPaths::Combine(*GameDir, FString("mask/sequence_") + temp + FString(".png"));
		FFileHelper::SaveArrayToFile(it.mask, *Filename);
	}


	for (auto it : depth)
	{
		char s[4];
		sprintf(s, "%04d", it.count);
		FString temp(s);
		FString Filename = FPaths::Combine(*GameDir, FString("depth/sequence_") + temp + FString(".exr"));
		FFileHelper::SaveArrayToFile(it.depth, *Filename);
	}
}

void ATouchGameModePlay::PaintSkelMesh(USkinnedMeshComponent* SkinnedMeshComponent, const FColor& VertexColor)
{
	USkeletalMesh* SkeletalMesh = SkinnedMeshComponent->SkeletalMesh;
	if (!SkeletalMesh)
	{
		return;
	}

	TIndirectArray<FStaticLODModel>& LODModels = SkeletalMesh->GetResourceForRendering()->LODModels;
	uint32 NumLODLevel = LODModels.Num();
	// SkinnedMeshComponent->SetLODDataCount(NumLODLevel, NumLODLevel);

	for (uint32 LODIndex = 0; LODIndex < NumLODLevel; LODIndex++)
	{
		FStaticLODModel& LODModel = LODModels[LODIndex];
		FSkelMeshComponentLODInfo* LODInfo = &SkinnedMeshComponent->LODInfo[LODIndex];
		if (LODInfo->OverrideVertexColors)
		{
			BeginReleaseResource(LODInfo->OverrideVertexColors);
			// Ensure the RT no longer accessed the data, might slow down
			FlushRenderingCommands();
			// The RT thread has no access to it any more so it's safe to delete it.
			// CleanUp();
		}
		// LODInfo->ReleaseOverrideVertexColorsAndBlock();
		LODInfo->OverrideVertexColors = new FColorVertexBuffer;
		LODInfo->OverrideVertexColors->InitFromSingleColor(VertexColor, LODModel.NumVertices);

		BeginInitResource(LODInfo->OverrideVertexColors);
		UE_LOG(LogTemp, Warning, TEXT("paint color for SkeletalMesh"));
	}
	SkinnedMeshComponent->MarkRenderStateDirty();
}

void ATouchGameModePlay::PaintVertexColor(AActor* Actor, const FColor& PaintColor)
{
	TArray<UMeshComponent*> PaintableComponents;
	Actor->GetComponents<UMeshComponent>(PaintableComponents);

	for (auto MeshComponent : PaintableComponents)
	{
		if (UStaticMeshComponent* StaticMeshComponent = Cast<UStaticMeshComponent>(MeshComponent))
		{
			PaintStaticMesh(StaticMeshComponent, PaintColor);
		}
		if (USkinnedMeshComponent* SkinnedMeshComponent = Cast<USkinnedMeshComponent>(MeshComponent))
		{
			PaintSkelMesh(SkinnedMeshComponent, PaintColor);

		}
	}
}

void ATouchGameModePlay::PaintStaticMesh(UStaticMeshComponent* StaticMeshComponent, const FColor& VertexColor)
{
	UStaticMesh* StaticMesh;
	// Assume major version is 4
	StaticMesh = StaticMeshComponent->GetStaticMesh(); // This is a new function introduced in 4.14

	if (!StaticMesh)
	{
		return;
	}

	uint32 NumLODLevel = StaticMesh->RenderData->LODResources.Num();
	for (uint32 PaintingMeshLODIndex = 0; PaintingMeshLODIndex < NumLODLevel; PaintingMeshLODIndex++)
	{
		FStaticMeshLODResources& LODModel = StaticMesh->RenderData->LODResources[PaintingMeshLODIndex];
		FStaticMeshComponentLODInfo* InstanceMeshLODInfo = NULL;

		// PaintingMeshLODIndex + 1 is the minimum requirement, enlarge if not satisfied
		StaticMeshComponent->SetLODDataCount(PaintingMeshLODIndex + 1, StaticMeshComponent->LODData.Num());
		InstanceMeshLODInfo = &StaticMeshComponent->LODData[PaintingMeshLODIndex];

		InstanceMeshLODInfo->ReleaseOverrideVertexColorsAndBlock();
		InstanceMeshLODInfo->OverrideVertexColors = new FColorVertexBuffer;
		check(InstanceMeshLODInfo->OverrideVertexColors);

		InstanceMeshLODInfo->OverrideVertexColors->InitFromSingleColor(VertexColor, LODModel.GetNumVertices());

		BeginInitResource(InstanceMeshLODInfo->OverrideVertexColors);
		StaticMeshComponent->MarkRenderStateDirty();

	}
}

// bool ATouchGameModePlay::PaintObject(AActor* Actor, const FColor& Color, bool IsColorGammaEncoded)
// {
// 	if (!Actor) return false;

// 	FColor NewColor;
// 	if (IsColorGammaEncoded)
// 	{
// 		FLinearColor LinearColor = FLinearColor::FromPow22Color(Color);
// 		NewColor = LinearColor.ToFColor(false);
// 	}
// 	else
// 	{
// 		NewColor = Color;
// 	}

// 	TArray<UMeshComponent*> PaintableComponents;
// 	Actor->GetComponents<UMeshComponent>(PaintableComponents);


// 	for (auto MeshComponent : PaintableComponents)
// 	{
// 		if (UStaticMeshComponent* StaticMeshComponent = Cast<UStaticMeshComponent>(MeshComponent))
// 		{
// 			UStaticMesh* StaticMesh;

// 			StaticMesh = StaticMeshComponent->GetStaticMesh(); // This is a new function introduced in 4.14

// 			if (StaticMesh)
// 			{
// 				uint32 NumLODLevel = StaticMesh->RenderData->LODResources.Num();
// 				for (uint32 PaintingMeshLODIndex = 0; PaintingMeshLODIndex < NumLODLevel; PaintingMeshLODIndex++)
// 				{
// 					FStaticMeshLODResources& LODModel = StaticMesh->RenderData->LODResources[PaintingMeshLODIndex];
// 					FStaticMeshComponentLODInfo* InstanceMeshLODInfo = NULL;

// 					// PaintingMeshLODIndex + 1 is the minimum requirement, enlarge if not satisfied
// 					StaticMeshComponent->SetLODDataCount(PaintingMeshLODIndex + 1, StaticMeshComponent->LODData.Num());
// 					InstanceMeshLODInfo = &StaticMeshComponent->LODData[PaintingMeshLODIndex];

// 					InstanceMeshLODInfo->ReleaseOverrideVertexColorsAndBlock();
// 					// Setup OverrideVertexColors
// 					// if (!InstanceMeshLODInfo->OverrideVertexColors) // TODO: Check this
// 					{
// 						InstanceMeshLODInfo->OverrideVertexColors = new FColorVertexBuffer;

// 						FColor FillColor = FColor(255, 255, 255, 255);
// 						InstanceMeshLODInfo->OverrideVertexColors->InitFromSingleColor(FColor::White, LODModel.GetNumVertices());
// 					}

// 					uint32 NumVertices = LODModel.GetNumVertices();
// 					check(InstanceMeshLODInfo->OverrideVertexColors);
// 					check(NumVertices <= InstanceMeshLODInfo->OverrideVertexColors->GetNumVertices());
// 					// StaticMeshComponent->CachePaintedDataIfNecessary();

// 					for (uint32 ColorIndex = 0; ColorIndex < NumVertices; ++ColorIndex)
// 					{
// 						// LODModel.ColorVertexBuffer.VertexColor(ColorIndex) = NewColor;  // This is vertex level
// 						// Need to initialize the vertex buffer first
// 						uint32 NumOverrideVertexColors = InstanceMeshLODInfo->OverrideVertexColors->GetNumVertices();
// 						uint32 NumPaintedVertices = InstanceMeshLODInfo->PaintedVertices.Num();
// 						// check(NumOverrideVertexColors == NumPaintedVertices);
// 						InstanceMeshLODInfo->OverrideVertexColors->VertexColor(ColorIndex) = NewColor;
// 						// InstanceMeshLODInfo->PaintedVertices[ColorIndex].Color = NewColor;
// 					}
// 					BeginInitResource(InstanceMeshLODInfo->OverrideVertexColors);
// 					StaticMeshComponent->MarkRenderStateDirty();
// 					// BeginUpdateResourceRHI(InstanceMeshLODInfo->OverrideVertexColors);


// 					/*
// 					// TODO: Need to check other LOD levels
// 					// Use flood fill to paint mesh vertices
// 					UE_LOG(LogUnrealCV, Warning, TEXT("%s:%s has %d vertices"), *Actor->GetActorLabel(), *StaticMeshComponent->GetName(), NumVertices);
// 					if (LODModel.ColorVertexBuffer.GetNumVertices() == 0)
// 					{
// 					// Mesh doesn't have a color vertex buffer yet!  We'll create one now.
// 					LODModel.ColorVertexBuffer.InitFromSingleColor(FColor(255, 255, 255, 255), LODModel.GetNumVertices());
// 					}
// 					*/
// 				}
// 			}
// 		}
// 	}
// 	return true;
// }


FColor ATouchGameModePlay::GenerateColor()
{
	uint8 r, g, b;
	r = FMath::RandRange(0, 255);
	g = FMath::RandRange(0, 255);
	b = FMath::RandRange(0, 255);
	return FColor(r, g, b, 255);
}