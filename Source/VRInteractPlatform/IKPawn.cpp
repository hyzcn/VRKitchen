// Fill out your copyright notice in the Description page of Project Settings.

#include "VRInteractPlatform.h"
#include "IKPawn.h"

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

static Value TransformMaker(FTransform tf, Document &doc)
{
	Value oj_tf(kObjectType);

	oj_tf.AddMember("Rot", RotatorMaker(tf.GetRotation().Rotator(), doc), doc.GetAllocator());
	oj_tf.AddMember("Scale", VectorMaker(tf.GetScale3D(), doc), doc.GetAllocator());
	oj_tf.AddMember("Trsl", VectorMaker(tf.GetTranslation(), doc), doc.GetAllocator());

	return oj_tf;
}

AIKPawn::AIKPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	LeftGrabPress = false;
	RightGrabPress = false;
	LeftSqueeze = false;
	RightSqueeze = false;
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
	
	MovementSpeed = 100;
	TurnSpeed = 500;
	UpdateInterval = 0.01;
	ActionToTake = 0;
	// PoseRecord = true;
	// PoseData = "";
}

// Called when the game starts or when spawned
void AIKPawn::BeginPlay()
{
	Super::BeginPlay();
	ShowMenuFlag = false;

	// PoseRecord = false;
	/*
	if (PoseRecord)
	{
		FString GameDir = FPaths::GameDir();
		FString FileName = GameDir + "HumanPoseRecord.csv";

		if (FPaths::FileExists(FileName))
		{
			FPlatformFileManager::Get().GetPlatformFile().DeleteFile(*FileName);
		}
	}
	
	FTimerHandle ReceiverHandler;
    GetWorldTimerManager().SetTimer(ReceiverHandler, this, &AIKPawn::UpdateAnim, UpdateInterval, true);
	*/
}

void AIKPawn::BeginDestroy()
{
	Super::BeginDestroy();

	/*
	if (PoseRecord)
	{
		FString GameDir = FPaths::GameDir();
		FString FileName = GameDir + "HumanPoseRecord.csv";

		FFileHelper::SaveStringToFile(PoseData, *FileName, FFileHelper::EEncodingOptions::AutoDetect, &IFileManager::Get());
	}
	*/
}

// Called every frame
void AIKPawn::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

// Called to bind functionality to input
void AIKPawn::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	Super::SetupPlayerInputComponent(InputComponent);
	InputComponent->BindAction("GrabLeft", EInputEvent::IE_Pressed, this, &AIKPawn::GrabLeft);
	InputComponent->BindAction("GrabLeft", EInputEvent::IE_Released, this, &AIKPawn::ReleaseLeft);
	InputComponent->BindAction("GrabRight", EInputEvent::IE_Pressed, this, &AIKPawn::GrabRight);
	InputComponent->BindAction("GrabRight", EInputEvent::IE_Released, this, &AIKPawn::ReleaseRight);
	InputComponent->BindAction("SqueezeLeft", EInputEvent::IE_Pressed, this, &AIKPawn::SqueezeLeft);
	InputComponent->BindAction("SqueezeLeft", EInputEvent::IE_Released, this, &AIKPawn::SqueezeReleaseLeft);
	InputComponent->BindAction("SqueezeRight", EInputEvent::IE_Pressed, this, &AIKPawn::SqueezeRight);
	InputComponent->BindAction("SqueezeRight", EInputEvent::IE_Released, this, &AIKPawn::SqueezeReleaseRight);

	InputComponent->BindAxis("MoveForward", this, &AIKPawn::ProcessForward);
	// InputComponent->BindAxis("MoveRight", this, &AIKPawn::ProcessRight);
	InputComponent->BindAxis("Turn", this, &AIKPawn::ProcessRotate);
}

void AIKPawn::ProcessForward(float AxisValue)
{
	if (AxisValue > 0.6 || AxisValue < -0.6)
	// if (AxisValue > 0.6)
		MovementInput.X = AxisValue;
	else
		MovementInput.X = 0;
}

void AIKPawn::ProcessRight(float AxisValue)
{
	if (AxisValue > 0.6 || AxisValue < -0.6)
		MovementInput.Y = AxisValue;
	else
		MovementInput.Y = 0;
}

void AIKPawn::ProcessRotate(float AxisValue)
{
	RotationInput = AxisValue;
}

void AIKPawn::UpdateAnim(FString &PoseData)
{
	UpdateMoveAnim();
	UpdateBodyAnim();
	FireGrabEventsLeft(LeftGrabPress);
	FireGrabEventsRight(RightGrabPress);
	AnimRecord(PoseData);
}

void AIKPawn::UpdateMoveAnim()
{
	if (SkeletalMesh != NULL)
	{
		UTouchAnimInstance* MyInstance = Cast<UTouchAnimInstance>(SkeletalMesh->GetAnimInstance());
		if (MyInstance != NULL)
		{
			FRotator NewRotation = GetActorRotation();
			NewRotation.Yaw += TurnSpeed * RotationInput * UpdateInterval; //DeltaTime
			SetActorRotation(NewRotation);
			FVector DisplacementVector = FVector(0, 0, 0);
			DisplacementVector = GetActorForwardVector() * MovementInput.X + GetActorRightVector() * MovementInput.Y;
			DisplacementVector = DisplacementVector.GetSafeNormal();
			DisplacementVector = MovementSpeed * DisplacementVector * UpdateInterval; //DeltaTime;
			FVector NewLocation = GetActorLocation() + DisplacementVector;
			SetActorLocation(NewLocation);

			// update movement velocity
			CurrentSpeed = MovementSpeed * MovementInput.Size();
			MyInstance->Speed = CurrentSpeed;
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Fail to find skeletalmesh"));
	}

	
}

void AIKPawn::UpdateBodyAnim()
{
	if (SkeletalMesh != NULL)
	{
		UTouchAnimInstance* MyInstance = Cast<UTouchAnimInstance>(SkeletalMesh->GetAnimInstance());
		if (MyInstance != NULL)
		{
			// update hand animation
			MyInstance->LeftHandWorldRot = MotionController_L->GetComponentRotation();
			MyInstance->LeftHandWorldPos = MotionController_L->GetComponentLocation() - (SkeletalMesh->GetSocketLocation("hand_lSocket") - SkeletalMesh->GetSocketLocation("hand_l"));
			MyInstance->RightHandWorldRot = MotionController_R->GetComponentRotation();
			MyInstance->RightHandWorldPos = MotionController_R->GetComponentLocation() - (SkeletalMesh->GetSocketLocation("hand_rSocket") - SkeletalMesh->GetSocketLocation("hand_r"));

			// update head animation
			MyInstance->HeadWorldTransform = Camera->GetComponentTransform();

		}
	}

}

void AIKPawn::Grab(bool IsLeft)
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

void AIKPawn::Release(bool IsLeft)
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

void AIKPawn::FireGrabEventsLeft(bool pressed)
{
	UTouchAnimInstance* MyInstance = Cast<UTouchAnimInstance>(SkeletalMesh->GetAnimInstance());
	if (pressed)
	{
		Grab(true);
		LeftGrab = true;
		LeftRelease = false;
		if (MyInstance != NULL)
			MyInstance->LeftGrabAnim = 1.0;
	}

	else
	{
		Release(true);
		LeftRelease = true;
		LeftGrab = false;
		if (MyInstance != NULL)
			MyInstance->LeftGrabAnim = 0.0;
	}
		
}

void AIKPawn::FireGrabEventsRight(bool pressed)
{
	UTouchAnimInstance* MyInstance = Cast<UTouchAnimInstance>(SkeletalMesh->GetAnimInstance());
	if (pressed)
	{
		Grab(false);
		RightGrab = true;
		RightRelease = false;
		if (MyInstance != NULL)
			MyInstance->RightGrabAnim = 1.0;

	}

	else
	{
		Release(false);
		RightRelease = true;
		RightGrab = false;
		if (MyInstance != NULL)
			MyInstance->RightGrabAnim = 0.0;
	}
		
}

void AIKPawn::AnimRecord(FString &PoseData)
{
	Value OJ(kObjectType);

	// Actor Pose
	Value OJActor(kObjectType);
	FVector Loc = GetActorLocation();
	// UE_LOG(LogTemp, Warning, TEXT("actor location %s"), *Loc.ToString());

	FRotator Rot = GetActorRotation();
	OJActor.AddMember("ActorLoc", VectorMaker(Loc, doc), doc.GetAllocator());
	OJActor.AddMember("ActorRot", RotatorMaker(Rot, doc), doc.GetAllocator());
	OJActor.AddMember("CurrentSpeed", CurrentSpeed, doc.GetAllocator());
	OJ.AddMember("ActorPose", OJActor, doc.GetAllocator());

	if (SkeletalMesh != NULL)
	{
		UTouchAnimInstance* MyInstance = Cast<UTouchAnimInstance>(SkeletalMesh->GetAnimInstance());

		if (MyInstance)
		{
			// Head Pose
			Value OJHead(kObjectType);
			OJHead.AddMember("HeadWorldTransform", TransformMaker(MyInstance->HeadWorldTransform, doc), doc.GetAllocator());
			OJ.AddMember("HeadPose", OJHead, doc.GetAllocator());

			// Left Hand Pose
			Value OJLeftHand(kObjectType);
			OJLeftHand.AddMember("LeftHandWorldPos", VectorMaker(MyInstance->LeftHandWorldPos, doc), doc.GetAllocator());
			OJLeftHand.AddMember("LeftHandWorldRot", RotatorMaker(MyInstance->LeftHandWorldRot, doc), doc.GetAllocator());
			Value OJLeftGrabObj(kArrayType);
			OJLeftGrabObj.Clear();
			for (auto& comp : LeftHandGrabbedComponents)
			{
				std::string temp(TCHAR_TO_UTF8(*(comp->GetOwner()->GetName())));
				Value CompName(temp.c_str(), doc.GetAllocator());
				OJLeftGrabObj.PushBack(CompName, doc.GetAllocator());
			}
			Value OJLeftGrab(kObjectType);
			OJLeftGrab.AddMember("LeftGrab", LeftGrab, doc.GetAllocator());
			OJLeftGrab.AddMember("LeftRelease", LeftRelease, doc.GetAllocator());

			OJ.AddMember("LeftHandGrab", OJLeftGrab, doc.GetAllocator());
			OJ.AddMember("LeftHandPose", OJLeftHand, doc.GetAllocator());
			OJ.AddMember("LeftHandGrabObj", OJLeftGrabObj, doc.GetAllocator());


			// Right Hand Pose
			Value OJRightHand(kObjectType);
			OJRightHand.AddMember("RightHandWorldPos", VectorMaker(MyInstance->RightHandWorldPos, doc), doc.GetAllocator());
			OJRightHand.AddMember("RightHandWorldRot", RotatorMaker(MyInstance->RightHandWorldRot, doc), doc.GetAllocator());
			Value OJRightGrabObj(kArrayType);
			OJRightGrabObj.Clear();
			for (auto& comp : RightHandGrabbedComponents)
			{
				std::string temp(TCHAR_TO_UTF8(*(comp->GetOwner()->GetName())));
				Value CompName(temp.c_str(), doc.GetAllocator());
				OJRightGrabObj.PushBack(CompName, doc.GetAllocator());
			}
			Value OJRightGrab(kObjectType);
			OJRightGrab.AddMember("RightGrab", RightGrab, doc.GetAllocator());
			OJRightGrab.AddMember("RightRelease", RightRelease, doc.GetAllocator());

			OJ.AddMember("RightHandGrab", OJRightGrab, doc.GetAllocator());
			OJ.AddMember("RightHandPose", OJRightHand, doc.GetAllocator());
			OJ.AddMember("RightHandGrabObj", OJRightGrabObj, doc.GetAllocator());
		}

		StringBuffer buffer;
		Writer<StringBuffer> writer(buffer);
		OJ.Accept(writer);
		std::string str = buffer.GetString();
		FString data(str.c_str());

		PoseData += data + "\n";
	}

}

void AIKPawn::GrabLeft()
{
	LeftGrabPress = true;
}

void AIKPawn::GrabRight()
{
	RightGrabPress = true;
}

void AIKPawn::ReleaseLeft()
{
	LeftGrabPress = false;
}

void AIKPawn::ReleaseRight()
{
	RightGrabPress = false;
}

void AIKPawn::SqueezeLeft()
{
	LeftSqueeze = true;
}

void AIKPawn::SqueezeRight()
{
	RightSqueeze = true;
}

void AIKPawn::SqueezeReleaseLeft()
{
	LeftSqueeze = false;
}

void AIKPawn::SqueezeReleaseRight()
{
	RightSqueeze = false;
}
