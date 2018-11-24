#include "VRInteractPlatform.h"
#include "GTCaptureComponent.h"
#include "ViewMode.h"
#include "Serialization.h"
#include "cnpy/cnpy.h"
#include "IImageWrapperModule.h"

void UGTCaptureComponents::InitCaptureComponent(USceneCaptureComponent2D* CaptureComponent)
{
	UWorld* World = GetWorld();
	// Can not use ESceneCaptureSource::SCS_SceneColorHDR, this option will disable post-processing
	CaptureComponent->CaptureSource = ESceneCaptureSource::SCS_FinalColorLDR;

	CaptureComponent->TextureTarget = NewObject<UTextureRenderTarget2D>();
	CaptureComponent->TextureTarget->InitAutoFormat(224, 224);
	CaptureComponent->FOVAngle = 105;
	CaptureComponent->RegisterComponentWithWorld(World);
}

void SaveExr(UTextureRenderTarget2D* RenderTarget, FString Filename)
{
	int32 Width = RenderTarget->SizeX, Height = RenderTarget->SizeY;
	TArray<FFloat16Color> FloatImage;
	FloatImage.AddZeroed(Width * Height);
	FTextureRenderTargetResource* RenderTargetResource = RenderTarget->GameThread_GetRenderTargetResource();
	RenderTargetResource->ReadFloat16Pixels(FloatImage);

	TArray<uint8> ExrData = SerializationUtils::Image2Exr(FloatImage, Width, Height);
	FFileHelper::SaveArrayToFile(ExrData, *Filename);
}

void SavePng(UTextureRenderTarget2D* RenderTarget, FString Filename)
{
	int32 Width = RenderTarget->SizeX, Height = RenderTarget->SizeY;
	TArray<FColor> Image;
	FTextureRenderTargetResource* RenderTargetResource;
	Image.AddZeroed(Width * Height);
	{
		RenderTargetResource = RenderTarget->GameThread_GetRenderTargetResource();
	}
	{
		FReadSurfaceDataFlags ReadSurfaceDataFlags;
		ReadSurfaceDataFlags.SetLinearToGamma(false); // This is super important to disable this!
													  // Instead of using this flag, we will set the gamma to the correct value directly
		RenderTargetResource->ReadPixels(Image, ReadSurfaceDataFlags);
	}
	TArray<uint8> ImgData = SerializationUtils::Image2Png(Image, Width, Height);
	FFileHelper::SaveArrayToFile(ImgData, *Filename);
}

UMaterial* UGTCaptureComponents::GetMaterial(FString InModeName = TEXT(""))
{
	// Load material for visualization
	static TMap<FString, FString>* MaterialPathMap = nullptr;
	if (MaterialPathMap == nullptr)
	{
		MaterialPathMap = new TMap<FString, FString>();
		MaterialPathMap->Add(TEXT("depth"), TEXT("Material'/Game/Material/SceneDepthWorldUnits.SceneDepthWorldUnits'"));
		MaterialPathMap->Add(TEXT("plane_depth"), TEXT("Material'/Game/Material/ScenePlaneDepthWorldUnits.ScenePlaneDepthWorldUnits'"));
		MaterialPathMap->Add(TEXT("vis_depth"), TEXT("Material'/Game/Material/SceneDepth.SceneDepth'"));
		MaterialPathMap->Add(TEXT("debug"), TEXT("Material'/Game/Material/debug.debug'"));
		MaterialPathMap->Add(TEXT("object_mask"), TEXT("Material'/Game/Material/VertexColorMaterial.VertexColorMaterial'"));
		MaterialPathMap->Add(TEXT("normal"), TEXT("Material'/Game/Material/WorldNormal.WorldNormal'"));

		FString OpaqueMaterialName = "Material'/Game/Material/OpaqueMaterial.OpaqueMaterial'";
		MaterialPathMap->Add(TEXT("opaque"), OpaqueMaterialName);
	}

	static TMap<FString, UMaterial*>* StaticMaterialMap = nullptr;
	if (StaticMaterialMap == nullptr)
	{
		StaticMaterialMap = new TMap<FString, UMaterial*>();
		for (auto& Elem : *MaterialPathMap)
		{
			FString ModeName = Elem.Key;
			FString MaterialPath = Elem.Value;
			ConstructorHelpers::FObjectFinder<UMaterial> Material(*MaterialPath); // ConsturctorHelpers is only available for UObject.

			if (Material.Object != NULL)
			{
				StaticMaterialMap->Add(ModeName, (UMaterial*)Material.Object);
			}
		}
	}

	UMaterial* Material = StaticMaterialMap->FindRef(InModeName);
	if (Material == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Can not recognize visualization mode %s"), *InModeName);
		UE_LOG(LogTemp, Warning, TEXT("not material"));
	}
	return Material;
}

/**
Attach a GTCaptureComponent to an actor
*/
UGTCaptureComponents* UGTCaptureComponents::Create(AActor* InActor, TArray<FString> Modes, UWorld* World, USceneComponent* AttachComponent)
{
	UGTCaptureComponents* GTCapturer = NewObject<UGTCaptureComponents>();

	GTCapturer->bIsActive = true;
	// check(GTCapturer->IsComponentTickEnabled() == true);
	GTCapturer->Actor = InActor;

	// This snippet is from Engine/Source/Runtime/Engine/Private/Components/SceneComponent.cpp, AttachTo
	FAttachmentTransformRules AttachmentRules(EAttachmentRule::KeepRelative, false);
	ConvertAttachLocation(EAttachLocation::KeepRelativeOffset, AttachmentRules.LocationRule, AttachmentRules.RotationRule, AttachmentRules.ScaleRule);
	GTCapturer->AttachToComponent(AttachComponent, AttachmentRules);
	// GTCapturer->AddToRoot();
	GTCapturer->RegisterComponentWithWorld(World);
	GTCapturer->SetTickableWhenPaused(true);

	for (FString Mode : Modes)
	{
		// DEPRECATED_FORGAME(4.6, "CaptureComponent2D should not be accessed directly, please use GetCaptureComponent2D() function instead. CaptureComponent2D will soon be private and your code will not compile.")
		USceneCaptureComponent2D* CaptureComponent = NewObject<USceneCaptureComponent2D>();
		CaptureComponent->bIsActive = false; // Disable it by default for performance consideration
		GTCapturer->CaptureComponents.Add(Mode, CaptureComponent);

		// CaptureComponent needs to be attached to somewhere immediately, otherwise it will be gc-ed

		CaptureComponent->AttachToComponent(GTCapturer, AttachmentRules);
		GTCapturer->InitCaptureComponent(CaptureComponent);

		UMaterial* Material = GetMaterial(Mode);
		if (Mode == "lit") // For rendered images
		{
			// FViewMode::Lit(CaptureComponent->ShowFlags);
			CaptureComponent->TextureTarget->TargetGamma = GEngine->GetDisplayGamma();
			// float DisplayGamma = SceneViewport->GetDisplayGamma();
		}
		else if (Mode == "default")
		{
			continue;
		}
		else // for ground truth
		{
			CaptureComponent->TextureTarget->TargetGamma = 1;
			if (Mode == "object_mask") // For object mask
			{
				//FViewMode::Lit(CaptureComponent->ShowFlags);
				FViewMode::VertexColor(CaptureComponent->ShowFlags);
			}
			else if (Mode == "wireframe") // For object mask
			{
				FViewMode::Wireframe(CaptureComponent->ShowFlags);
			}
			else
			{
				check(Material);
				// GEngine->GetDisplayGamma(), the default gamma is 2.2
				// CaptureComponent->TextureTarget->TargetGamma = 2.2;
				FViewMode::PostProcess(CaptureComponent->ShowFlags);

				CaptureComponent->PostProcessSettings.AddBlendable(Material, 1);
				// Instead of switching post-process materials, we create several SceneCaptureComponent, so that we can capture different GT within the same frame.
			}
		}
	}
	return GTCapturer;
}

UGTCaptureComponents::UGTCaptureComponents()
{
	GetMaterial(); // Initialize the TMap
	PrimaryComponentTick.bCanEverTick = true;
	// bIsTicking = false;

	// Create USceneCaptureComponent2D
	// Design Choice: do I need one capture component with changing post-process materials or multiple components?
	// USceneCaptureComponent2D* CaptureComponent = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("GTCaptureComponent"));
}

// Each GTCapturer can serve as one camera of the scene

void UGTCaptureComponents::SetFOVAngle(float FOV)
{
	for (auto Iterator = CaptureComponents.CreateIterator(); Iterator; ++Iterator)
	{
		Iterator.Value()->FOVAngle = FOV;
	}
}

void UGTCaptureComponents::Capture(FString Mode, FString InFilename)
{
	// Flush location and rotation

	check(CaptureComponents.Num() != 0);
	USceneCaptureComponent2D* CaptureComponent = CaptureComponents.FindRef(Mode);

	FGTCaptureTask GTCaptureTask = FGTCaptureTask(Mode, InFilename, GFrameCounter);
	this->PendingTasks.Enqueue(GTCaptureTask);

}

TArray<uint8> UGTCaptureComponents::CapturePng(FString Mode)
{
	// Flush location and rotation
	check(CaptureComponents.Num() != 0);
	USceneCaptureComponent2D* CaptureComponent = CaptureComponents.FindRef(Mode);

	TArray<uint8> ImgData;
	if (CaptureComponent == nullptr)
		return ImgData;

	UTextureRenderTarget2D* RenderTarget = CaptureComponent->TextureTarget;
	static IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
	static TSharedPtr<IImageWrapper> ImageWrapper = ImageWrapperModule.CreateImageWrapper(EImageFormat::PNG);
	int32 Width = RenderTarget->SizeX, Height = RenderTarget->SizeY;
	TArray<FColor> Image;
	FTextureRenderTargetResource* RenderTargetResource;
	Image.AddZeroed(Width * Height);
	RenderTargetResource = RenderTarget->GameThread_GetRenderTargetResource();

	FReadSurfaceDataFlags ReadSurfaceDataFlags;
	ReadSurfaceDataFlags.SetLinearToGamma(false); // This is super important to disable this!
												  // Instead of using this flag, we will set the gamma to the correct value directly
	RenderTargetResource->ReadPixels(Image, ReadSurfaceDataFlags);
	//ImageWrapper->SetRaw(Image.GetData(), Image.GetAllocatedSize(), Width, Height, ERGBFormat::BGRA, 8);
	//ImgData = ImageWrapper->GetCompressed();

	ImgData = SerializationUtils::Image2Png(Image, Width, Height);
	return ImgData;
}

TArray<uint8> UGTCaptureComponents::CaptureExr(FString Mode)
{
	check(CaptureComponents.Num() != 0);
	USceneCaptureComponent2D* CaptureComponent = CaptureComponents.FindRef(Mode);

	TArray<uint8> ImgData;
	if (CaptureComponent == nullptr)
		return ImgData;

	UTextureRenderTarget2D* RenderTarget = CaptureComponent->TextureTarget;
	static IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
	static TSharedPtr<IImageWrapper> ImageWrapper = ImageWrapperModule.CreateImageWrapper(EImageFormat::PNG);
	int32 Width = RenderTarget->SizeX, Height = RenderTarget->SizeY;
	TArray<FFloat16Color> FloatImage;
	FloatImage.AddZeroed(Width * Height);
	FTextureRenderTargetResource* RenderTargetResource = RenderTarget->GameThread_GetRenderTargetResource();
	RenderTargetResource->ReadFloat16Pixels(FloatImage);

	TArray<uint8> ExrData = SerializationUtils::Image2Exr(FloatImage, Width, Height);
	return ExrData;
}

TArray<uint8> UGTCaptureComponents::CaptureNpyUint8(FString Mode, int32 Channels)
{

	// Flush location and rotation
	check(CaptureComponents.Num() != 0);
	USceneCaptureComponent2D* CaptureComponent = CaptureComponents.FindRef(Mode);

	TArray<uint8> NpyData;
	if (CaptureComponent == nullptr) {
		UE_LOG(LogTemp, Error, TEXT("Component for mode %s not found. Returning empty array."), *Mode);
		return NpyData;
	}

	UTextureRenderTarget2D* RenderTarget = CaptureComponent->TextureTarget;
	int32 Width = RenderTarget->SizeX, Height = RenderTarget->SizeY;
	TArray<FColor> ImageData;
	FTextureRenderTargetResource* RenderTargetResource;
	ImageData.AddUninitialized(Width * Height);
	RenderTargetResource = RenderTarget->GameThread_GetRenderTargetResource();
	FReadSurfaceDataFlags ReadSurfaceDataFlags;
	ReadSurfaceDataFlags.SetLinearToGamma(false); // This is super important to disable this!
												  // Instead of using this flag, we will set the gamma to the correct value directly
	RenderTargetResource->ReadPixels(ImageData, ReadSurfaceDataFlags);

	// Check the byte order of data
	// Compress image data to npy array
	// Generate a header for the numpy array
	NpyData = NpySerialization(ImageData, Width, Height, Channels);

	return NpyData;
}

TArray<uint8> UGTCaptureComponents::CaptureNpyFloat16(FString Mode, int32 Channels)
{
	// Flush location and rotation
	check(CaptureComponents.Num() != 0);
	USceneCaptureComponent2D* CaptureComponent = CaptureComponents.FindRef(Mode);

	TArray<uint8> NpyData;
	if (CaptureComponent == nullptr) {
		UE_LOG(LogTemp, Error, TEXT("Component for mode %s not found. Returning empty array."), *Mode);
		return NpyData;
	}

	UTextureRenderTarget2D* RenderTarget = CaptureComponent->TextureTarget;
	int32 Width = RenderTarget->SizeX, Height = RenderTarget->SizeY;
	TArray<FFloat16Color> ImageData;
	FTextureRenderTargetResource* RenderTargetResource;
	ImageData.AddUninitialized(Width * Height);
	RenderTargetResource = RenderTarget->GameThread_GetRenderTargetResource();
	RenderTargetResource->ReadFloat16Pixels(ImageData);

	// Check the byte order of data
	// Compress image data to npy array
	// Generate a header for the numpy array
	NpyData = NpySerialization(ImageData, Width, Height, Channels);

	return NpyData;
}

TArray<uint8> UGTCaptureComponents::NpySerialization(TArray<FColor> ImageData, int32 Width, int32 Height, int32 Channel)
{
	uint8 *TypePointer = nullptr; // Only used for determing the type

	std::vector<int> Shape;
	Shape.push_back(Height);
	Shape.push_back(Width);
	if (Channel != 1) Shape.push_back(Channel);

	std::vector<char> NpyHeader = cnpy::create_npy_header<uint8>(TypePointer, Shape);

	// Append the actual data
	// FIXME: A slow implementation to convert TArray<FFloat16Color> to binary.
	/* A small size test
	std::vector<char> NpyData;
	for (int i = 0; i < 3 * 3 * 3; i++)
	{
	NpyData.push_back(i);
	}
	*/
	// std::vector<char> NpyData;
	std::vector<uint8> Uint8Data;
	Uint8Data.reserve(ImageData.Num() * Channel);

	for (int i = 0; i < ImageData.Num(); i++)
	{
		if (Channel == 1)
		{
			uint8 v = ImageData[i].R;
			Uint8Data.push_back(ImageData[i].R);
		}
		else if (Channel == 3)
		{
			Uint8Data.push_back(ImageData[i].R);
			Uint8Data.push_back(ImageData[i].G);
			Uint8Data.push_back(ImageData[i].B);
		}
		else if (Channel == 4)
		{
			Uint8Data.push_back(ImageData[i].R);
			Uint8Data.push_back(ImageData[i].G);
			Uint8Data.push_back(ImageData[i].B);
			Uint8Data.push_back(ImageData[i].A);
		}
	}
	check(Uint8Data.size() == Width * Height * Channel);
	// Convert to binary array
	const unsigned char* bytes = reinterpret_cast<const unsigned char*>(&Uint8Data[0]);

	// https://stackoverflow.com/questions/22629728/what-is-the-difference-between-char-and-unsigned-char
	// https://stackoverflow.com/questions/11022099/convert-float-vector-to-byte-vector-and-back
	std::vector<unsigned char> NpyData(bytes, bytes + sizeof(uint8) * Uint8Data.size());

	NpyHeader.insert(NpyHeader.end(), NpyData.begin(), NpyData.end());

	// FIXME: Find a more efficient implementation
	TArray<uint8> BinaryData;
	for (char Element : NpyHeader)
	{
		BinaryData.Add(Element);
	}
	return BinaryData;
}

TArray<uint8> UGTCaptureComponents::NpySerialization(TArray<FFloat16Color> ImageData, int32 Width, int32 Height, int32 Channel)
{
	float *TypePointer = nullptr; // Only used for determing the type

	std::vector<int> Shape;
	Shape.push_back(Height);
	Shape.push_back(Width);
	if (Channel != 1) Shape.push_back(Channel);

	std::vector<char> NpyHeader = cnpy::create_npy_header<float>(TypePointer, Shape);

	// Append the actual data
	// FIXME: A slow implementation to convert TArray<FFloat16Color> to binary.
	/* A small size test
	std::vector<char> NpyData;
	for (int i = 0; i < 3 * 3 * 3; i++)
	{
	NpyData.push_back(i);
	}
	*/
	// std::vector<char> NpyData;
	std::vector<float> FloatData;
	FloatData.reserve(ImageData.Num() * Channel);
	for (int i = 0; i < ImageData.Num(); i++)
	{
		if (Channel == 1)
		{
			const float Value = ImageData[i].R;
			FloatData.push_back(Value);
		}
		if (Channel == 3)
		{
			FloatData.push_back(ImageData[i].R);
			FloatData.push_back(ImageData[i].G);
			FloatData.push_back(ImageData[i].B); // TODO: Is this a correct order in numpy?
		}
	}
	check(FloatData.size() == Width * Height * Channel);
	// Convert to binary array
	const unsigned char* bytes = reinterpret_cast<const unsigned char*>(&FloatData[0]);

	// https://stackoverflow.com/questions/22629728/what-is-the-difference-between-char-and-unsigned-char
	// https://stackoverflow.com/questions/11022099/convert-float-vector-to-byte-vector-and-back
	std::vector<unsigned char> NpyData(bytes, bytes + sizeof(float) * FloatData.size());

	NpyHeader.insert(NpyHeader.end(), NpyData.begin(), NpyData.end());

	// FIXME: Find a more efficient implementation
	TArray<uint8> BinaryData;
	for (char Element : NpyHeader)
	{
		BinaryData.Add(Element);
	}
	return BinaryData;
}

void UGTCaptureComponents::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
// void UGTCaptureComponent::Tick(float DeltaTime) // This tick function should be called by the scene instead of been
{
	// Render pixels out in the next tick. To allow time to render images out.

	// Update rotation of each frame
	// from ab237f46dc0eee40263acbacbe938312eb0dffbb:CameraComponent.cpp:232
	check(this->Actor); // this GTCapturer should be released, if the actor is deleted.

	while (!PendingTasks.IsEmpty())
	{
		FGTCaptureTask Task;
		PendingTasks.Peek(Task);
		uint64 CurrentFrame = GFrameCounter;

		int32 SkipFrame = 1;
		if (!(CurrentFrame > Task.CurrentFrame + SkipFrame)) // TODO: This is not an elegant solution, fix it later.
		{ // Wait for the rendering thread to catch up game thread.
			break;
		}

		PendingTasks.Dequeue(Task);
		USceneCaptureComponent2D* CaptureComponent = this->CaptureComponents.FindRef(Task.Mode);
		if (CaptureComponent == nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("Unrecognized capture mode %s"), *Task.Mode);
		}
		else
		{
			FString LowerCaseFilename = Task.Filename.ToLower();
			if (LowerCaseFilename.EndsWith("png"))
			{
				SavePng(CaptureComponent->TextureTarget, Task.Filename);
				UE_LOG(LogTemp, Warning, TEXT("image captured!!!"));
			}
			else if (LowerCaseFilename.EndsWith("exr"))
			{
				SaveExr(CaptureComponent->TextureTarget, Task.Filename);
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Unrecognized image file extension %s"), *LowerCaseFilename);
			}
		}
	}
}

USceneCaptureComponent2D* UGTCaptureComponents::GetCaptureComponent(FString Mode)
{
	check(CaptureComponents.Num() != 0);
	USceneCaptureComponent2D* CaptureComponent = CaptureComponents.FindRef(Mode);
	return CaptureComponent;
}