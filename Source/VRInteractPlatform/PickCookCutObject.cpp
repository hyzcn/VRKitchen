// Fill out your copyright notice in the Description page of Project Settings.

#include "VRInteractPlatform.h"
#include "PickCookCutObject.h"


// Sets default values
APickCookCutObject::APickCookCutObject()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	CanChangeColor = false;
	Cooked = false;
	Cut = false;
	CanSqueeze = false;
	OnFire = false;
	Spoon = false;
	MyTimeLine = CreateDefaultSubobject<UTimelineComponent>(TEXT("MyTimeLine"));
}

// Called when the game starts or when spawned
void APickCookCutObject::BeginPlay()
{
	FOnTimelineLinearColor onTimelineCallback;
	FOnTimelineEventStatic onTimelineFinishedCallback;

		MyTimeLine->SetNetAddressable();
		MyTimeLine->SetLooping(false);
		MyTimeLine->SetTimelineLength(10.0f);
		MyTimeLine->SetPlaybackPosition(0.0f, false);
		MyTimeLine->SetTimelineLengthMode(ETimelineLengthMode::TL_LastKeyFrame);
		onTimelineCallback.BindUFunction(this, FName{ TEXT("TimelineCallback") });
		onTimelineFinishedCallback.BindUFunction(this, FName{ TEXT("TimelineFinishedCallback") });
		MyTimeLine->AddInterpLinearColor(ColorCurve, onTimelineCallback);
		MyTimeLine->SetTimelineFinishedFunc(onTimelineFinishedCallback);
		MyTimeLine->RegisterComponent();

		Super::BeginPlay();

	
}

// Called every frame
void APickCookCutObject::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );
	if (MyTimeLine != NULL)
	{
		MyTimeLine->TickComponent(DeltaTime, ELevelTick::LEVELTICK_TimeOnly, NULL);
	}
	if (OnFire)
	{
		PlayTimeline();
	}

}

void APickCookCutObject::TimelineCallback(FLinearColor Value)
{
	if (DynMaterial != NULL)
	{
		DynMaterial->SetVectorParameterValue(FName{ TEXT("Color") }, Value);
	}
}

void APickCookCutObject::TimelineFinishedCallback()
{
	Cooked = true;
}

void APickCookCutObject::PlayTimeline()
{
	if (MyTimeLine != NULL)
	{
		MyTimeLine->Play();
	}
}