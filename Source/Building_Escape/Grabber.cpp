// Copyright Jeremy Speer 2022

#include "Grabber.h"

#include "DrawDebugHelpers.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"

#define OUT

// Sets default values for this component's properties
UGrabber::UGrabber()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}

// Called when the game starts
void UGrabber::BeginPlay()
{
	Super::BeginPlay();

	FindPhysicsHandle();
	SetupInputComponent();

	// Just in case there is an access attempt for PlayerViewPointLocation or PlayerViewPointRotation before 1 frame has passed
	UpdatePlayerLocationRotation();
}

void UGrabber::FindPhysicsHandle()
{
	// Check for the physics handle component
	PhysicsHandle = GetOwner()->FindComponentByClass<UPhysicsHandleComponent>();
	if (!PhysicsHandle) {
		UE_LOG(LogTemp, Error, TEXT("%s has an grabber component, but no physics handle!"), *GetOwner()->GetName());
	}
}

void UGrabber::SetupInputComponent()
{
	// Attach the input component
	InputComponent = GetOwner()->FindComponentByClass<UInputComponent>();
	if (InputComponent) {
		//UE_LOG(LogTemp, Warning, TEXT("Bound %s input controller %s."), *InputComponent->GetOwner()->GetName(), *InputComponent->GetName());
		InputComponent->BindAction("Grab", IE_Pressed, this, &UGrabber::Grab);
		InputComponent->BindAction("Grab", IE_Released, this, &UGrabber::Release);
		//UE_LOG(LogTemp, Warning, TEXT("Action \"Grab\" bound on input controller %s."), *InputComponent->GetName());
	}
}

void UGrabber::Grab()
{
	UE_LOG(LogTemp, Warning, TEXT("Grabber Pressed"));

	// If physics handle is nullptr, bail out before we crash
	if (!PhysicsHandle) { return; }

	// If the input bind is pressed, we need to check if the object has a physics body
	FHitResult HitResult = GetFirstPhysicsBodyInReach();
	UPrimitiveComponent* ComponentToGrab = HitResult.GetComponent();
	AActor* ActorHit = HitResult.GetActor();

	// We're all good, go ahead and grab the component
	if (ActorHit) {
		PhysicsHandle->GrabComponentAtLocation(
			ComponentToGrab,
			NAME_None,
			GetPlayerReach()
		);
	}
}

void UGrabber::Release()
{
	UE_LOG(LogTemp, Warning, TEXT("Grabber Released"));

	// If physics handle is nullptr, bail out before we crash
	if (!PhysicsHandle) { return; }

	// Release the object when input bind is released
	PhysicsHandle->ReleaseComponent();
}

// Called every frame
void UGrabber::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Update the players location every tick
	UpdatePlayerLocationRotation();

	// Draw the debug line so we can see where we are pointed and our reach limit
	DrawDebugTrace();

	// If physics handle is nullptr, bail out before we crash
	if (!PhysicsHandle) { return; }

	// If the physics handle is attached to a component
	if (PhysicsHandle->GrabbedComponent) {
		// set target location
		PhysicsHandle->SetTargetLocation(GetPlayerReach());
	}
}

void UGrabber::UpdatePlayerLocationRotation()
{
	// Update the PlayerViewPointLocation and PlayerViewPointRotation every frame
	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(OUT PlayerViewPointLocation, OUT PlayerViewPointRotation);
}

FVector UGrabber::GetPlayerReach() const
{
	return PlayerViewPointLocation + (PlayerViewPointRotation.Vector() * Reach);
}

FHitResult UGrabber::GetFirstPhysicsBodyInReach() const
{
	FHitResult Hit;
	FCollisionQueryParams TraceParams(FName(TEXT("")), false, GetOwner());

	bool bActorHit = false;

	bActorHit = GetWorld()->LineTraceSingleByObjectType (
		OUT Hit,
		PlayerViewPointLocation,
		GetPlayerReach(),
		FCollisionObjectQueryParams(ECollisionChannel::ECC_PhysicsBody),
		TraceParams
	);

	// Log it out to LogTemp
	if (bActorHit) {
		UE_LOG(LogTemp, Warning, TEXT("Line trace has hit: %s"), *Hit.GetActor()->GetName());
	}

	return Hit;
}

void UGrabber::DrawDebugTrace()
{
	// Draw a debug line so we can see where we are pointed
	DrawDebugLine (
		GetWorld(),
		PlayerViewPointLocation,
		GetPlayerReach(),
		FColor(0, 255, 0),
		false,
		0.f,
		(uint8)'\000',
		5.f
	);
}
