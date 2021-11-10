// Nathaniel Richards 2021

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "RPlayerDrone.generated.h"

class UCameraComponent;
class USpringArmComponent;
class USphereComponent;

UCLASS()
class ROBSAVESTHINGS_API ARPlayerDrone : public APawn
{
	GENERATED_BODY()

public:
	ARPlayerDrone();

protected:
	virtual void BeginPlay() override;

	void MoveVertical(float Value);
	void MoveHorizontal(float Value);

	void HoverUp();
	void HoverDown();

	void Turn(float Value);

	void LookVertical(float Value);

	void FireBug();

	void CloseFireBug();

public:
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(EditAnywhere)
	float Speed = 300.f;

	UPROPERTY(EditAnywhere)
	float VelocityModifier = 1000.f;

	UPROPERTY(EditAnywhere)
	float Range = 100.f;
private:
	UPROPERTY(EditAnywhere)
	USkeletalMeshComponent* PlayerMesh;

	UPROPERTY(EditAnywhere)
	UAudioComponent* HoveringSound;

	UPROPERTY(EditAnywhere)
	USceneComponent* AimStartLocation;

	UPROPERTY(EditAnywhere)
	UCameraComponent* CameraComp;

	UPROPERTY(EditAnywhere)
	USpringArmComponent* SpringArmComp;

	UPROPERTY(EditAnywhere)
	USphereComponent* SphereComp;

	bool HoveringUp = false;
	bool HoveringDown = false;

	void SetHoveringUp();
	void SetHoveringDown();

	FTimerHandle BugProjectileTimerHandle;

	void PredictProjectilePath();
};
