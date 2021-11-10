// Nathaniel Richards 2021


#include "RPlayerDrone.h"

#include "DrawDebugHelpers.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/AudioComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"


ARPlayerDrone::ARPlayerDrone()
{
	PrimaryActorTick.bCanEverTick = true;

	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	RootComponent = SphereComp;

	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArmComp->bUsePawnControlRotation = true;
	SpringArmComp->SetupAttachment(SphereComp);

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	CameraComp->SetupAttachment(SpringArmComp, USpringArmComponent::SocketName);

	PlayerMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("PlayerMesh"));
	PlayerMesh->SetupAttachment(RootComponent);

	HoveringSound = CreateDefaultSubobject<UAudioComponent>(TEXT("HoveringSound"));
	HoveringSound->bAutoActivate = false;

	AimStartLocation = CreateDefaultSubobject<USceneComponent>(TEXT("AimStartLocation"));
	AimStartLocation->SetupAttachment(PlayerMesh);

	bUseControllerRotationYaw = false;
}

void ARPlayerDrone::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (HoveringDown)
	{
		HoverDown();
	}
	else if (HoveringUp)
	{
		HoverUp();
	}
}

void ARPlayerDrone::BeginPlay()
{
	Super::BeginPlay();
}

void ARPlayerDrone::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction(TEXT("HoverUp"), IE_Pressed, this, &ARPlayerDrone::SetHoveringUp);
	PlayerInputComponent->BindAction(TEXT("HoverUp"), IE_Released, this, &ARPlayerDrone::SetHoveringUp);

	PlayerInputComponent->BindAction(TEXT("HoverDown"), IE_Pressed, this, &ARPlayerDrone::SetHoveringDown);
	PlayerInputComponent->BindAction(TEXT("HoverDown"), IE_Released, this, &ARPlayerDrone::SetHoveringDown);

	PlayerInputComponent->BindAction(TEXT("FireBug"), IE_Pressed, this, &ARPlayerDrone::FireBug);
	PlayerInputComponent->BindAction(TEXT("CloseFireBug"), IE_Pressed, this, &ARPlayerDrone::CloseFireBug);


	PlayerInputComponent->BindAxis(TEXT("MoveVertical"), this, &ARPlayerDrone::MoveVertical);
	PlayerInputComponent->BindAxis(TEXT("MoveHorizontal"), this, &ARPlayerDrone::MoveHorizontal);

	PlayerInputComponent->BindAxis(TEXT("Turn"), this, &ARPlayerDrone::Turn);
	PlayerInputComponent->BindAxis(TEXT("LookVertical"), this, &ARPlayerDrone::LookVertical);
}

void ARPlayerDrone::MoveVertical(float Value)
{
	FVector DeltaLocation(0.f);

	DeltaLocation.X = Value * GetWorld()->GetDeltaSeconds() * Speed;
	AddActorLocalOffset(DeltaLocation, true);
}

void ARPlayerDrone::MoveHorizontal(float Value)
{
	FVector DeltaLocation(0.f);
	DeltaLocation.Y = Value * GetWorld()->GetDeltaSeconds() * Speed;
	AddActorLocalOffset(DeltaLocation, true);
}

void ARPlayerDrone::HoverUp()
{
	FVector DeltaLocation(0.f);
	DeltaLocation.Z = 1 * GetWorld()->GetDeltaSeconds() * Speed;
	AddActorLocalOffset(DeltaLocation, true);
}

void ARPlayerDrone::HoverDown()
{
	FVector DeltaLocation(0.f);
	DeltaLocation.Z = -1 * GetWorld()->GetDeltaSeconds() * Speed;
	AddActorLocalOffset(DeltaLocation, true);
}

void ARPlayerDrone::Turn(float Value)
{
	AddControllerYawInput
	(
		Value
	);
}

void ARPlayerDrone::LookVertical(float Value)
{
	AddControllerPitchInput
	(
		Value
	);
}

void ARPlayerDrone::SetHoveringUp()
{
	if (HoveringUp == true)
	{
		HoveringUp = false;
	}
	else
	{
		HoveringUp = true;
	}
}

void ARPlayerDrone::SetHoveringDown()
{
	HoveringDown = !HoveringDown;
}

void ARPlayerDrone::PredictProjectilePath()
{
	FHitResult Hit;
	TArray<FVector> HitPathPositions;
	FVector OutLastTraceDestination;

	UGameplayStatics::Blueprint_PredictProjectilePath_ByTraceChannel
	(
		GetWorld(),
		Hit,
		HitPathPositions,
		OutLastTraceDestination,
		AimStartLocation->GetComponentLocation(),
		GetActorRotation().Vector() * VelocityModifier,
		true,
		0.f,
		ECC_WorldDynamic,
		false,
		{},
		EDrawDebugTrace::ForDuration,
		0.02,
		30.f,
		2,
		0
	);

	if (((GetActorLocation() - OutLastTraceDestination).Size() * 0.5) <= Range)
	{
		UE_LOG(LogTemp, Warning, TEXT("PredictionPath is valid to throw!"));
	} else
	{
		UE_LOG(LogTemp, Error, TEXT("PredictionPath is NOT valid to throw!"));

	}
}

void ARPlayerDrone::FireBug()
{
	GetWorld()->GetTimerManager().SetTimer
	(
		BugProjectileTimerHandle,
		this,
		&ARPlayerDrone::PredictProjectilePath,
		0.02,
		true
	);
}

void ARPlayerDrone::CloseFireBug()
{
	GetWorldTimerManager().ClearTimer(BugProjectileTimerHandle);
}
