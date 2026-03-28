// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InputActionValue.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "InputAction.h"
#include "AirCraftControlComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAxisChanged, float, Value);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class EXP_API UAirCraftControlComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UAirCraftControlComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	// ================= INPUT VALUES =================
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aircraft|Input")
	float ThrottleInput;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aircraft|Input")
	float MixtureInput;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aircraft|Input")
	float BrakeInput;
	// ===== AXIS VALUES =====
	UPROPERTY(BlueprintReadOnly)
	float Throttle;

	UPROPERTY(BlueprintReadOnly)
	float Pitch;

	UPROPERTY(BlueprintReadOnly)
	float Roll;

	UPROPERTY(BlueprintReadOnly)
	float Yaw;

	// ===== SETTINGS =====
	UPROPERTY(EditAnywhere)
	float MoveSpeed = 500.0f;

	UPROPERTY(EditAnywhere)
	float RotationSpeed = 100.f;

	// ===== INPUT ACTIONS =====
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* PitchAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* RollAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* YawAction;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	class UStaticMeshComponent* ThrottleMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UStaticMeshComponent* MixtureMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UStaticMeshComponent* BrakeMesh;

	// ===== FUNCTIONS =====
	void OnPitchInput(const FInputActionValue& Value);
	void OnRollInput(const FInputActionValue& Value);
	void OnYawInput(const FInputActionValue& Value);

	float NormalizeAxisForAirCraft(float Value);

	// ================= CURRENT VALUES =================
	UPROPERTY(BlueprintReadOnly, Category = "Aircraft|State")
	float CurrentThrottle;

	UPROPERTY(BlueprintReadOnly, Category = "Aircraft|State")
	float CurrentMixture;

	UPROPERTY(BlueprintReadOnly, Category = "Aircraft|State")
	float CurrentBrake;

	// ================= SETTINGS =================
	UPROPERTY(EditAnywhere, Category = "Aircraft|Settings")
	float InterpSpeed = 3.0f;

	UPROPERTY(EditAnywhere, Category = "Aircraft|Settings")
	float DeadZone = 0.02f;
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputMappingContext* InputMapping;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* ThrottleAction;
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* MixtureAction;
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* BrakeAction;

	// ================= EVENTS =================
	UPROPERTY(BlueprintAssignable)
	FOnAxisChanged OnThrottleChanged;

	UPROPERTY(BlueprintAssignable)
	FOnAxisChanged OnMixtureChanged;

	UPROPERTY(BlueprintAssignable)
	FOnAxisChanged OnBrakeChanged;

	// ================= FUNCTIONS =================

	UFUNCTION(BlueprintCallable)
	void SetMixture(float Value);

	UFUNCTION(BlueprintCallable)
	void SetBrake(float Value);
	void OnThrottleInput(const FInputActionValue& Value);
	void OnMixtureInput(const FInputActionValue& Value);
	void OnBrakeInput(const FInputActionValue& Value);
private:
	
	float ApplyDeadZone(float Value);
		
};
