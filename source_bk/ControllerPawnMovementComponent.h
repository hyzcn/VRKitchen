// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/PawnMovementComponent.h"
#include "ControllerPawnMovementComponent.generated.h"

/**
 * 
 */
UCLASS()
class VRINTERACTPLATFORM_API UControllerPawnMovementComponent : public UPawnMovementComponent
{
		GENERATED_BODY()
public:
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;
	
	
	
};
