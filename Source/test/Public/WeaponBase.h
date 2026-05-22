#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeaponBase.generated.h"

UENUM(BlueprintType)
enum class EFireMode : uint8
{
	SemiAuto    UMETA(DisplayName = "Semi-Automatic"),
	FullAuto    UMETA(DisplayName = "Full-Automatic")
};

UCLASS(Abstract, Blueprintable)
class TEST_API AWeaponBase : public AActor
{
	GENERATED_BODY()
	
public:	
	AWeaponBase();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UStaticMeshComponent* WeaponMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon Stats")
	float BaseDamage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon Stats")
	int32 MaxAmmo;

	UPROPERTY(BlueprintReadOnly, Category = "Weapon Stats")
	int32 CurrentAmmo;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon Stats|Spread")
	float MaxSpread;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon Stats|Spread")
	float SpreadIncrement;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon Stats|Spread")
	float SpreadRecoveryRate;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon Stats")
	float RecoilMultiplier;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon Stats")
	float MaxRange;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon Stats")
	float ProjectileSpeed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon Stats|Fire Mode")
	TArray<EFireMode> AvailableFireModes;

	UPROPERTY(BlueprintReadOnly, Category = "Weapon Stats|Fire Mode")
	EFireMode CurrentFireMode;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon Stats|Fire Mode")
	float FireRate;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon Stats")
	float ReloadTime;

	UPROPERTY(BlueprintReadOnly, Category = "Weapon Logic")
	bool bIsReloading;

	UFUNCTION(BlueprintCallable, Category = "Weapon Logic")
	void Reload();

	UFUNCTION(BlueprintCallable, Category = "Weapon Logic")
	void StartFiring();

	UFUNCTION(BlueprintCallable, Category = "Weapon Logic")
	void StopFiring();

	UFUNCTION(BlueprintCallable, Category = "Weapon Logic")
	void SwitchFireMode();

	UPROPERTY(EditDefaultsOnly, Category = "Weapon Setup")
	TSubclassOf<class AProjectileBase> ProjectileClass;


protected:
	void FireShot();
	void ResetFireCooldown();
	void FinishReload();

	FTimerHandle FireTimerHandle;
	bool bCanFire;
	int32 CurrentFireModeIndex;

	UPROPERTY(BlueprintReadOnly, Category = "Weapon Stats|Spread")
	float CurrentSpreadPercentage;
private:
	float LastRecoilValue;
};

