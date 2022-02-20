// Copyright Jeremy Speer 2022

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/TriggerVolume.h"
#include "OpenDoor.generated.h"

#define OPEN_DOOR_YAW_SCALER_NONE 0
#define OPEN_DOOR_YAW_SCALER_DISTANCE 1
#define OPEN_DOOR_YAW_SCALER_LINEAR 2
#define OPEN_DOOR_YAW_SCALER_EXPONENTIAL 3

UENUM()
enum class OpenDoorYawScaler : int32
{
	YAW_SCALER_NONE			UMETA(DisplayName = "None"),
	YAW_SCALER_DISTANCE		UMETA(DisplayName = "Distance Scaler"),
	YAW_SCALER_LINEAR		UMETA(DisplayName = "Linear Scaler"),
	YAW_SCALER_EXPONENTIAL	UMETA(DisplayName = "Exponential Scaler")
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BUILDING_ESCAPE_API UOpenDoor : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UOpenDoor();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void OpenDoor(float DeltaTime);
	void CloseDoor(float DeltaTime);
	static float CalculateDoorYaw(float StartYaw, float EndYaw, float DeltaTime, float Speed=0.0f, int32 Method=0);
	float CalculateTotalMassOfActors() const;
	void CheckPressurePlate();
	void FindAudioComponent();

private:
	float InitialYaw;
	float PressurePlateFirstUsed = 0.f;
	float DoorLastOpened = 0.f;
	bool bOpenSoundPlayed = false;
	bool bCloseSoundPlayed = true;

	UPROPERTY()
	UAudioComponent* AudioComponent = nullptr;

	UPROPERTY(EditAnywhere) // The angle of the door in the open position
		float OpenAngle = 90.f;
	UPROPERTY(EditAnywhere) // The speed at which the door opens (this is reletive to the scaler)
		float DoorOpenVelocity = 2.f;
	UPROPERTY(EditAnywhere) // The scaler used to calculate the yaw change per tick
		OpenDoorYawScaler DoorOpenYawScaler;
	UPROPERTY(EditAnywhere) // The delay before the door moves to the open angle
		float DoorOpenDelay = 0.f;
	UPROPERTY(EditAnywhere) // The speed at which the door closes (this is reletive to the scaler)
		float DoorCloseVelocity = 600.f;
	UPROPERTY(EditAnywhere) // The scaler used to calculate the yaw change per tick
		OpenDoorYawScaler DoorCloseYawScaler;
	UPROPERTY(EditAnywhere) // The delay before the door returns to the start angle
		float DoorCloseDelay = 2.f;
	UPROPERTY(EditAnywhere) // The trigger actor used for the door open
		ATriggerVolume* PressurePlate = nullptr;
	UPROPERTY(EditAnywhere) // The mass required for the trigger to open the door
		float TriggerMassRequired = 0.f;
};
