// Copyright Jeremy Speer 2022

#include "OpenDoor.h"

#include "Components/AudioComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Actor.h"

// Sets default values for this component's properties
UOpenDoor::UOpenDoor()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}

// Called when the game starts
void UOpenDoor::BeginPlay()
{
	Super::BeginPlay();

	// Define vars
	InitialYaw = GetOwner()->GetActorRotation().Yaw;
	OpenAngle += InitialYaw;
	OpenAngle = (float)fmod(OpenAngle, 360);
	if (OpenAngle > 180.f)
		OpenAngle -= 360.f;

	// Check pressure plate
	CheckPressurePlate();

	// Find the audio component
	FindAudioComponent();
}

// Called every frame
void UOpenDoor::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Open the door
	if (PressurePlate && CalculateTotalMassOfActors() >= TriggerMassRequired) {
		if (PressurePlateFirstUsed == 0.f) {
			PressurePlateFirstUsed = GetWorld()->GetTimeSeconds();
		} else {
			if (PressurePlateFirstUsed + DoorOpenDelay <= GetWorld()->GetTimeSeconds()) {
				if (AudioComponent != nullptr && bOpenSoundPlayed == false) {
					bOpenSoundPlayed = true; bCloseSoundPlayed = false;
					AudioComponent->Play();
				}
				DoorLastOpened = GetWorld()->GetTimeSeconds();
				OpenDoor(DeltaTime);
			}
		}
	}

	// Close the door
	else {
		PressurePlateFirstUsed = 0.f;
		if (DoorLastOpened + DoorCloseDelay <= GetWorld()->GetTimeSeconds())
		{
			if (AudioComponent != nullptr && bCloseSoundPlayed == false) {
				bOpenSoundPlayed = false; bCloseSoundPlayed = true;
				AudioComponent->Play();
			}
			CloseDoor(DeltaTime);
		}
	}
}

void UOpenDoor::OpenDoor(float DeltaTime)
{
	FRotator Rotation = GetOwner()->GetActorRotation();
	Rotation.Yaw = UOpenDoor::CalculateDoorYaw(Rotation.Yaw, OpenAngle, DeltaTime, DoorOpenVelocity, (int32)DoorOpenYawScaler);
	GetOwner()->SetActorRotation(Rotation);
	//UE_LOG(LogTemp, Warning, TEXT("%s yaw position: %f"), *GetOwner()->GetName(), Rotation.Yaw);
}

void UOpenDoor::CloseDoor(float DeltaTime)
{
	FRotator Rotation = GetOwner()->GetActorRotation();
	Rotation.Yaw = UOpenDoor::CalculateDoorYaw(Rotation.Yaw, InitialYaw, DeltaTime, DoorCloseVelocity, (int32)DoorCloseYawScaler);
	GetOwner()->SetActorRotation(Rotation);
	//UE_LOG(LogTemp, Warning, TEXT("%s yaw position: %f"), *GetOwner()->GetName(), Rotation.Yaw);
}

float UOpenDoor::CalculateTotalMassOfActors() const
{
	// If pressure plate is nullptr, bail out before we crash
	if (!PressurePlate) { return 0.f; }

	float TotalMass = 0.f;
	// find all overlapping actors
	TArray<AActor*> OverlappingActors;
	PressurePlate->GetOverlappingActors(OUT OverlappingActors);

	// add up their masses
	for (AActor* OverlappingActor : OverlappingActors) {
		TotalMass += OverlappingActor->FindComponentByClass<UPrimitiveComponent>()->GetMass();
		//UE_LOG(LogTemp, Warning, TEXT("%s is on the pressure plate"), *OverlappingActor->GetName());
	}

	return TotalMass;
}

void UOpenDoor::CheckPressurePlate()
{
	// If there is no pressure plate, that's a problem. REPORT IT.
	if (!PressurePlate) {
		UE_LOG(LogTemp, Error, TEXT("%s has an OpenDoor component, but no PressurePlate set!"), *GetOwner()->GetName());
	}
}

void UOpenDoor::FindAudioComponent()
{
	AudioComponent = GetOwner()->FindComponentByClass<UAudioComponent>();
	if (!AudioComponent) {
		UE_LOG(LogTemp, Error, TEXT("%s is missing an audio component"), *GetOwner()->GetName());
	}
}

float UOpenDoor::CalculateDoorYaw(float StartYaw, float EndYaw, float DeltaTime, float Speed, int32 Method)
{
	float CalculatedYaw;
	switch (Method) {
		case OPEN_DOOR_YAW_SCALER_DISTANCE:
			CalculatedYaw = FMath::FInterpTo(StartYaw, EndYaw, DeltaTime, Speed);         // Distance Scaled Interpolation
			break;
		case OPEN_DOOR_YAW_SCALER_LINEAR:
			CalculatedYaw = FMath::FInterpConstantTo(StartYaw, EndYaw, DeltaTime, Speed); // Linear Interpolation
			break;
		case OPEN_DOOR_YAW_SCALER_EXPONENTIAL:
			CalculatedYaw = FMath::Lerp(StartYaw, EndYaw, DeltaTime * Speed);             // Exponential Interpolation
			break;
		case OPEN_DOOR_YAW_SCALER_NONE:
		default:
			CalculatedYaw = EndYaw;
	}
	return CalculatedYaw;
}
