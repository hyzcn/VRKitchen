// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/GameMode.h"
#include "TestGameMode.generated.h"

/**
 * Basic gamemode, just turns on and off the kinect
 */
UCLASS()
class VRINTERACTPLATFORM_API ATestGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	ATestGameMode();

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void Tick(float DeltaSeconds) override;

};
