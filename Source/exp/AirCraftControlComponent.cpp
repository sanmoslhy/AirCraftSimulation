// Fill out your copyright notice in the Description page of Project Settings.


#include "AirCraftControlComponent.h"

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
	Owner->AddActorWorldOffset(Forward * Throttle * MoveSpeed * DeltaTime);

	// ===== Rotation =====
	FRotator RotationDelta;
	RotationDelta.Pitch = Pitch * RotationSpeed * DeltaTime;
	RotationDelta.Roll = Roll * RotationSpeed * DeltaTime;
	RotationDelta.Yaw = Yaw * RotationSpeed * DeltaTime;

	Owner->AddActorLocalRotation(RotationDelta);

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
	float Fixed = (Value - 0.5f) * 2.0f;

	if (FMath::Abs(Fixed) < 0.1f)
		return 0.0f;

	return Fixed;
}

void UAirCraftControlComponent::SetThrottleInAriCraft(float Value)
{
	
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

float UAirCraftControlComponent::ApplyDeadZone(float Value)
{
	return (FMath::Abs(Value) < DeadZone) ? 0.f : Value;
}

