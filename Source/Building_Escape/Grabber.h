// Copyright Jeremy Speer 2022

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"
#include "Grabber.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BUILDING_ESCAPE_API UGrabber : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UGrabber();

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

private:
	float Reach = 140.f;

	FVector PlayerViewPointLocation;
	FRotator PlayerViewPointRotation;

	UPROPERTY()
	UPhysicsHandleComponent* PhysicsHandle = nullptr;
	UPROPERTY()
	UInputComponent* InputComponent = nullptr;

	void Grab();
	void Release();
	void FindPhysicsHandle();
	void SetupInputComponent();

	// Set the player location and rotation
	void UpdatePlayerLocationRotation();

	// Get the player reach
	FVector GetPlayerReach() const;

	// Return the first Actor within reach with a physics body
	FHitResult GetFirstPhysicsBodyInReach() const;

	// Draw a debug line so we can see where we are pointed and our reach limit
	void DrawDebugTrace();
};
