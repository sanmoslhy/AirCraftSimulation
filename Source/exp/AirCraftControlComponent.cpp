// Fill out your copyright notice in the Description page of Project Settings.


#include "AirCraftControlComponent.h"
#include"Components/StaticMeshComponent.h"
#if __has_include("EasyFlightModelComponent.h")
#include "EasyFlightModelComponent.h"
#endif

// Sets default values for this component's properties
UAirCraftControlComponent::UAirCraftControlComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	ThrottleInput = 0.f;
	MixtureInput = 0.f;
	BrakeInput = 0.f;

	CurrentThrottle = 0.f;
	CurrentMixture = 0.f;
	CurrentBrake = 0.f;
}


// Called when the game starts
void UAirCraftControlComponent::BeginPlay()
{
	Super::BeginPlay();
	AActor* Owner = GetOwner();
	if (!Owner) return;
	// ===== Find Meshes By Tag =====
	TArray<UActorComponent*> Components = Owner->GetComponentsByClass(UStaticMeshComponent::StaticClass());

	for (UActorComponent* Comp : Components)
	{
		UStaticMeshComponent* Mesh = Cast<UStaticMeshComponent>(Comp);
		if (!Mesh) continue;

		if (Mesh->ComponentHasTag(FName("Throttle")))
		{
			ThrottleMesh = Mesh;
		}
		else if (Mesh->ComponentHasTag(FName("Mixture")))
		{
			MixtureMesh = Mesh;
		}
		else if (Mesh->ComponentHasTag(FName("Brake")))
		{
			BrakeMesh = Mesh;
		}
	}
	// ===== EasyFM =====
	//EasyFM = Owner->FindComponentByClass<UEasyFlightModelComponent>();
	APlayerController* PC = Cast<APlayerController>(Owner->GetInstigatorController());
	if (!PC)
	{
		PC = GetWorld()->GetFirstPlayerController();
	}

	if (PC)
	{
		// Add Mapping Context
		if (ULocalPlayer* LocalPlayer = PC->GetLocalPlayer())
		{
			if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
				LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
			{
				Subsystem->AddMappingContext(InputMapping, 0);
			}
		}

		// Bind Input
		if (UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(PC->InputComponent))
		{
			if (ThrottleAction)
			{
				EIC->BindAction(ThrottleAction, ETriggerEvent::Triggered, this, &UAirCraftControlComponent::OnThrottleInput);
			}

			if (PitchAction)
			{
				EIC->BindAction(PitchAction, ETriggerEvent::Triggered, this, &UAirCraftControlComponent::OnPitchInput);
			}

			if (RollAction)
			{
				EIC->BindAction(RollAction, ETriggerEvent::Triggered, this, &UAirCraftControlComponent::OnRollInput);
			}

			if (YawAction)
			{
				EIC->BindAction(YawAction, ETriggerEvent::Triggered, this, &UAirCraftControlComponent::OnYawInput);
			}
			if (MixtureAction)
			{
				EIC->BindAction(MixtureAction, ETriggerEvent::Triggered, this, &UAirCraftControlComponent::OnMixtureInput);
			}

			if (BrakeAction)
			{
				EIC->BindAction(BrakeAction, ETriggerEvent::Triggered, this, &UAirCraftControlComponent::OnBrakeInput);
			}
		}
	}
	}


// Called every frame
void UAirCraftControlComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	AActor* Owner = GetOwner();
	if (!Owner) return;

	// ===== Movement =====
	FVector Forward = Owner->GetActorForwardVector();
	//Owner->AddActorWorldOffset(Forward * Throttle * MoveSpeed * DeltaTime);

	// ===== Rotation =====
	FRotator RotationDelta;
	RotationDelta.Pitch = Pitch * RotationSpeed * DeltaTime;
	RotationDelta.Roll = Roll * RotationSpeed * DeltaTime;
	RotationDelta.Yaw = Yaw * RotationSpeed * DeltaTime;
	// ===== EASY FM =====
	/*if (EasyFM)
	{
		float Throttle01 = (Throttle + 1.f) * 0.5f; 
		EasyFM->SetThrottleInput(Throttle01);
	}*/
	//Owner->AddActorLocalRotation(RotationDelta);
	// ===== VISUAL CONTROLS =====

	if (ThrottleMesh)
	{
		float Offset = Throttle * 20.f;

		FVector Current = ThrottleMesh->GetRelativeLocation();
		FVector Target = FVector(Offset, Current.Y, Current.Z);
		FVector Smooth = FMath::VInterpTo(Current, Target, DeltaTime, 5.0f);

		ThrottleMesh->SetRelativeLocation(Smooth);
	}

	if (MixtureMesh)
	{
		float Offset = MixtureInput * 20.f;

		FVector Current = MixtureMesh->GetRelativeLocation();
		FVector Target = FVector(Offset, Current.Y, Current.Z);

		FVector Smooth = FMath::VInterpTo(Current, Target, DeltaTime, 6.f);
		MixtureMesh->SetRelativeLocation(Smooth);
	}

	if (BrakeMesh)
	{
		float Offset = BrakeInput * 10.f;

		FVector Current = BrakeMesh->GetRelativeLocation();
		FVector Target = FVector(Current.X, Current.Y, -Offset);

		BrakeMesh->SetRelativeLocation(Target);
	}

	// ===== Broadcast =====
	OnThrottleChanged.Broadcast(Throttle);
	// ...
}

void UAirCraftControlComponent::OnPitchInput(const FInputActionValue& Value)
{
	Pitch = NormalizeAxisForAirCraft(Value.Get<float>());
}

void UAirCraftControlComponent::OnRollInput(const FInputActionValue& Value)
{
	Roll = NormalizeAxisForAirCraft(Value.Get<float>());
}

void UAirCraftControlComponent::OnYawInput(const FInputActionValue& Value)
{
	Yaw = NormalizeAxisForAirCraft(Value.Get<float>());
}

float UAirCraftControlComponent::NormalizeAxisForAirCraft(float Value)
{
	Value = FMath::Clamp(Value, 0.f, 1.f);

	// 
	float Fixed = (Value * 2.0f) - 1.0f;

	// DeadZone
	if (FMath::Abs(Fixed) < DeadZone)
		return 0.0f;

	return Fixed;
}

void UAirCraftControlComponent::SetMixture(float Value)
{
	MixtureInput = FMath::Clamp(ApplyDeadZone(Value), 0.f, 1.f);
}

void UAirCraftControlComponent::SetBrake(float Value)
{
	BrakeInput = FMath::Clamp(ApplyDeadZone(Value), 0.f, 1.f);
}

void UAirCraftControlComponent::OnThrottleInput(const FInputActionValue& Value)
{

	Throttle = NormalizeAxisForAirCraft(Value.Get<float>());

	UE_LOG(LogTemp, Warning, TEXT("Throttle: %f"), Throttle);

	
}

void UAirCraftControlComponent::OnMixtureInput(const FInputActionValue& Value)
{
	MixtureInput = FMath::Clamp(Value.Get<float>(), 0.f, 1.f);
}

void UAirCraftControlComponent::OnBrakeInput(const FInputActionValue& Value)
{
	BrakeInput = FMath::Clamp(Value.Get<float>(), 0.f, 1.f);
}

float UAirCraftControlComponent::ApplyDeadZone(float Value)
{
	return (FMath::Abs(Value) < DeadZone) ? 0.f : Value;
}

