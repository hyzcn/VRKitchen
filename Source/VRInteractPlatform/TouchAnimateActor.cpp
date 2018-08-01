// Fill out your copyright notice in the Description page of Project Settings.

#include "VRInteractPlatform.h"
#include "TouchAnimateActor.h"

static Document ParsedDoc(const Value &obj)
{
	Document d;
	StringBuffer buf;
	Writer<StringBuffer> writer(buf);

	obj.Accept(writer);
	std::string objStr = buf.GetString();
	d.Parse(objStr.c_str());

	return d;
}

static FRotator RotatorMaker(const Value &rotator)
{
	Document d = ParsedDoc(rotator);
	double pitch = d["Pitch"].GetDouble();
	double yaw = d["Yaw"].GetDouble();
	double roll = d["Roll"].GetDouble();

	return FRotator(pitch, yaw, roll);
}

static FVector VectorMaker(const Value &vector)
{
	Document d = ParsedDoc(vector);
	double x = d["X"].GetDouble();
	double y = d["Y"].GetDouble();
	double z = d["Z"].GetDouble();

	return FVector(x, y, z);
}

static FTransform TransformMaker(const Value &transform)
{
	Document d = ParsedDoc(transform);
	FRotator Rot = RotatorMaker(d["Rot"]);
	FVector Scale = VectorMaker(d["Scale"]);
	FVector Trsl = VectorMaker(d["Trsl"]);

	return FTransform(Rot, Trsl, Scale);
}

ATouchAnimateActor::ATouchAnimateActor()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	LeftHandAttachPoint = FName("hand_lSocket");
	RightHandAttachPoint = FName("hand_rSocket");

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneRoot"));
	SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	MotionController_R = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("MotionController_R"));
	MotionController_L = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("MotionController_L"));

	CameraRoot = CreateDefaultSubobject<USceneComponent>(TEXT("CameraRoot"));
	SkeletalMesh->SetupAttachment(RootComponent);
	CameraRoot->SetupAttachment(RootComponent);
	Camera->SetupAttachment(CameraRoot);
	MotionController_R->SetupAttachment(RootComponent);
	MotionController_L->SetupAttachment(RootComponent);

	RecordTrigger = false;
	RecordSeg = "";


}

void ATouchAnimateActor::BeginPlay()
{
	Super::BeginPlay();
	/*
	FString GameDir = FPaths::GameDir();
	FString FileName = GameDir + "HumanBaxterPoseRecord.csv";

	if (FPaths::FileExists(FileName))
	{
		FString FileData = "";
		FFileHelper::LoadFileToString(FileData, *FileName);

		int32 FieldCount = FileData.ParseIntoArray(ApplyPoseArray, TEXT("\n"), true);
		UE_LOG(LogTemp, Warning, TEXT("record count: %d"), FieldCount);
	}
	*/
	if (SkeletalMesh)
	{
		AnimInstance = Cast<UTouchAnimInstance>(SkeletalMesh->GetAnimInstance());
		if (AnimInstance)
			UE_LOG(LogTemp, Warning, TEXT("Anim instance exists!!!"));
	}
	/*
	FTimerHandle ReceiverHandler;
	GetWorldTimerManager().SetTimer(ReceiverHandler, this, &ATouchAnimateActor::UpdateAnim, UpdateInterval, true);
	*/
}

void ATouchAnimateActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


void ATouchAnimateActor::Grab(bool IsLeft)
{
	//Spherecast to find all primitive components in grab range
	UWorld* TheWorld = this->GetWorld();

	FVector HandLocation = SkeletalMesh->GetSocketLocation(IsLeft ? LeftHandAttachPoint : RightHandAttachPoint);
	// DrawDebugSphere(TheWorld, HandLocation, 5.f, 8, FColor(255, 0, 0), true);
	UE_LOG(LogTemp, Warning, TEXT("hand location %s"), *HandLocation.ToString());
	TArray<FHitResult> OutResults;
	FCollisionShape GrabSphere = FCollisionShape::MakeSphere(3.f);
	FCollisionObjectQueryParams ObjectParams;
	FCollisionQueryParams CollisionParams;
	ObjectParams.AddObjectTypesToQuery(ECollisionChannel::ECC_WorldDynamic);
	ObjectParams.AddObjectTypesToQuery(ECollisionChannel::ECC_PhysicsBody);
	CollisionParams.bFindInitialOverlaps = true;
	TheWorld->SweepMultiByObjectType(OutResults, HandLocation, HandLocation + FVector(.1, 0, 0), FQuat(), ObjectParams, GrabSphere, CollisionParams);

	//Calling grab logic on all components in range
	FAttachmentTransformRules GrabRules = FAttachmentTransformRules::KeepWorldTransform;
	GrabRules.bWeldSimulatedBodies = true;
	if (IsLeft)
	{
		for (auto& Hit : OutResults)
		{
			UPrimitiveComponent* Comp = Hit.GetComponent();
			if (Comp->Mobility == EComponentMobility::Movable)
			{
				Comp->SetSimulatePhysics(false);
				Comp->AttachToComponent(SkeletalMesh, GrabRules, LeftHandAttachPoint);
				if (!LeftHandGrabbedComponents.Contains(Comp))
					LeftHandGrabbedComponents.Add(Comp);

			}

		}
	}
	else
	{
		for (auto& Hit : OutResults)
		{

			UPrimitiveComponent* Comp = Hit.GetComponent();
			if (Comp->Mobility == EComponentMobility::Movable)
			{
				Comp->SetSimulatePhysics(false);
				Comp->AttachToComponent(SkeletalMesh, GrabRules, RightHandAttachPoint);
				if (!RightHandGrabbedComponents.Contains(Comp))
					RightHandGrabbedComponents.Add(Comp);
			}
		}
	}
	//UE_LOG(LogTemp, Warning, TEXT("Grabbing"));
}

void ATouchAnimateActor::Release(bool IsLeft)
{
	if (IsLeft)
	{
		for (auto& Comp : LeftHandGrabbedComponents)
		{
			Comp->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
			Comp->SetSimulatePhysics(true);
		}
		LeftHandGrabbedComponents.Empty();
	}
	else
	{
		for (auto& Comp : RightHandGrabbedComponents)
		{
			Comp->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
			Comp->SetSimulatePhysics(true);
		}
		RightHandGrabbedComponents.Empty();
	}

	//UE_LOG(LogTemp, Warning, TEXT("Releasing"));
}

void ATouchAnimateActor::UpdateAnim(FString record)
{
		std::string json_data(TCHAR_TO_UTF8(*record));
		Document doc;
		doc.Parse(json_data.c_str());
		

		if (AnimInstance)
		{
			// UE_LOG(LogTemp, Warning, TEXT("Update anim instance!!!"));
			UpdateActor(doc);
			// UpdateHead(doc);
			UpdateBody(doc);
		}

}

void ATouchAnimateActor::UpdateActor(Document &doc)
{
	Document d = ParsedDoc(doc["ActorPose"].GetObject());

	SetActorRotation(RotatorMaker(d["ActorRot"].GetObject()));
	FVector calibratedLoc = VectorMaker(d["ActorLoc"].GetObject());
	SetActorLocation(calibratedLoc);
    // UE_LOG(LogTemp, Warning, TEXT("Location %s"), *calibratedLoc.ToString());


	AnimInstance->Speed = d["CurrentSpeed"].GetDouble();
}

void ATouchAnimateActor::UpdateHead(Document &doc)
{
	Document d = ParsedDoc(doc["HeadPose"].GetObject());

	AnimInstance->HeadWorldTransform = TransformMaker(d["HeadWorldTransform"].GetObject());
}

void ATouchAnimateActor::UpdateBody(Document &doc)
{
	Document d;

	// Left hand pose
	d = ParsedDoc(doc["LeftHandPose"].GetObject());
	AnimInstance->LeftHandWorldRot = RotatorMaker(d["LeftHandWorldRot"].GetObject());
	AnimInstance->LeftHandWorldPos = VectorMaker(d["LeftHandWorldPos"].GetObject());

	// Right hand pose
	d = ParsedDoc(doc["RightHandPose"].GetObject());
	AnimInstance->RightHandWorldRot = RotatorMaker(d["RightHandWorldRot"].GetObject());
	AnimInstance->RightHandWorldPos = VectorMaker(d["RightHandWorldPos"].GetObject());

	UpdateHead(doc);

	// Left hand grab
	d = ParsedDoc(doc["LeftHandGrab"].GetObject());
	if (d["LeftGrab"].GetBool())
		Grab(true);
	if (d["LeftRelease"].GetBool())
		Release(true);

	// Right hand grab
	d = ParsedDoc(doc["RightHandGrab"].GetObject());
	if (d["RightGrab"].GetBool())
		Grab(false);
	if (d["RightRelease"].GetBool())
		Release(false);


}

