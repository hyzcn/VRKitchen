// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/GameMode.h"
#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"
#include "GTCaptureComponent.h"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "BaxterRobot.h"
#include "ProceduralMeshComponent.h"
#include "IKPawn.h"
#include "OnOffObject.h"
#include "PickCookCutObject.h"
#include "CoffeMakerActor.h"
#include "PourContainer.h"
#include "ObjectContainer.h"
#include "TouchGameModeRecord.generated.h"

/**
*
*/
UCLASS()
class VRINTERACTPLATFORM_API ATouchGameModeRecord : public AGameMode
{
	GENERATED_BODY()

public:
	ATouchGameModeRecord();

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void Tick(float DeltaSeconds) override;
	void RecordActors();

private:
	Document doc;
	bool PoseRecord;
	FString PoseData;
	FString ObjData;
	FString GameDir;
	FString FileName;
	FString FileNameObj;
	AIKPawn* HumanPawn;
	ABaxterRobot* BaxterRobot;
	APickCookCutObject* Carrot;
	UProceduralMeshComponent* CarrotMesh;
	AOnOffObject* Stove;
	AOnOffObject* CabinetDoor;
	AStaticMeshActor* Knife;
	ACoffeMakerActor* CofMaker;
	APourContainer* Cup;
	AObjectContainer* Pan;
	AObjectContainer* Cabinet;
	AObjectContainer* Plate;
	float RecordInterval;
	void RecordObjData(FString &Pose);
};
