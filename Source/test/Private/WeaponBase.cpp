#include "WeaponBase.h"
#include "Components/StaticMeshComponent.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"
#include "ProjectileBase.h"

AWeaponBase::AWeaponBase()
{

	PrimaryActorTick.bCanEverTick = true;

	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));
	RootComponent = WeaponMesh;

	BaseDamage = 20.f;
	MaxAmmo = 30;
	MaxSpread = 5.0f;
	SpreadIncrement = 0.15f;
	SpreadRecoveryRate = 1.0f;
	MaxRange = 5000.f;
	ProjectileSpeed = 7000.f;
	FireRate = 600.f;
	bCanFire = true;
	CurrentSpreadPercentage = 0.0f;
	CurrentFireModeIndex = 0;
	ReloadTime = 2.0f;
	bIsReloading = false;
	RecoilMultiplier = 1.5f;
	LastRecoilValue = 0.0f;
}

void AWeaponBase::BeginPlay()
{
	Super::BeginPlay();
	CurrentAmmo = MaxAmmo;

	if (AvailableFireModes.Num() > 0)
	{
		CurrentFireMode = AvailableFireModes[0];
	}
}

void AWeaponBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (CurrentSpreadPercentage > 0.0f)
	{
		CurrentSpreadPercentage -= SpreadRecoveryRate * DeltaTime;
		CurrentSpreadPercentage = FMath::Clamp(CurrentSpreadPercentage, 0.0f, 1.0f);
	}

	if (WeaponMesh)
	{
		float TargetRecoil = CurrentSpreadPercentage * MaxSpread * RecoilMultiplier;

		float DeltaRecoil = TargetRecoil - LastRecoilValue;

		WeaponMesh->AddLocalRotation(FRotator(0.0f, 0.0f, -DeltaRecoil));
		LastRecoilValue = TargetRecoil;
	}
}

void AWeaponBase::StartFiring()
{

	if (!bCanFire || bIsReloading) return;

	if (CurrentAmmo <= 0)
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Orange, TEXT("brak amunicji"));
		return;
	}

	if (CurrentFireMode == EFireMode::FullAuto)
	{
		float DelayBetweenShots = 60.0f / FireRate;
		GetWorldTimerManager().SetTimer(FireTimerHandle, this, &AWeaponBase::FireShot, DelayBetweenShots, true, 0.0f);
	}
	else
	{
		FireShot();
	}
}

void AWeaponBase::StopFiring()
{
	GetWorldTimerManager().ClearTimer(FireTimerHandle);
}
void AWeaponBase::FireShot()
{
	if (CurrentAmmo <= 0)
	{
		StopFiring();
		return;
	}

	if (CurrentFireMode == EFireMode::SemiAuto && !bCanFire) return;

	CurrentAmmo--;

	if (WeaponMesh && ProjectileClass)
	{
		FVector MuzzleLocation = WeaponMesh->GetSocketLocation(TEXT("MuzzleFlash"));

		FVector ViewLocation;
		FRotator ViewRotation;
		APawn* InstigatorPawn = GetInstigator();

		if (InstigatorPawn)
		{
			InstigatorPawn->GetActorEyesViewPoint(ViewLocation, ViewRotation);
		}
		else
		{
			ViewLocation = MuzzleLocation;
			ViewRotation = WeaponMesh->GetSocketRotation(TEXT("MuzzleFlash"));
		}

		FVector TraceEnd = ViewLocation + (ViewRotation.Vector() * MaxRange);
		FHitResult HitResult;

		FCollisionQueryParams TraceParams;
		TraceParams.AddIgnoredActor(this);
		TraceParams.AddIgnoredActor(InstigatorPawn);

		bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, ViewLocation, TraceEnd, ECC_Visibility, TraceParams);

		FVector TargetPoint = bHit ? HitResult.ImpactPoint : TraceEnd;

		FVector FireDirection = (TargetPoint - MuzzleLocation).GetSafeNormal();

		float CurrentSpreadDegrees = CurrentSpreadPercentage * MaxSpread;
		FVector SpreadDirection = FMath::VRandCone(FireDirection, FMath::DegreesToRadians(CurrentSpreadDegrees));
		FRotator FinalRotation = SpreadDirection.Rotation();

		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.Instigator = InstigatorPawn;

		AProjectileBase* SpawnedProjectile = GetWorld()->SpawnActor<AProjectileBase>(ProjectileClass, MuzzleLocation, FinalRotation, SpawnParams);
		if (SpawnedProjectile)
		{
			SpawnedProjectile->InitializeProjectile(BaseDamage, MaxRange, ProjectileSpeed);
		}
	}

	CurrentSpreadPercentage = FMath::Clamp(CurrentSpreadPercentage + SpreadIncrement, 0.0f, 1.0f);

	if (CurrentFireMode == EFireMode::SemiAuto)
	{
		bCanFire = false;
		float DelayBetweenShots = 60.0f / FireRate;
		FTimerHandle CooldownTimer;
		GetWorldTimerManager().SetTimer(CooldownTimer, this, &AWeaponBase::ResetFireCooldown, DelayBetweenShots, false);
	}

}

void AWeaponBase::ResetFireCooldown()
{
	bCanFire = true;
}

void AWeaponBase::SwitchFireMode()
{
	if (AvailableFireModes.Num() <= 1) return;

	CurrentFireModeIndex = (CurrentFireModeIndex + 1) % AvailableFireModes.Num();
	CurrentFireMode = AvailableFireModes[CurrentFireModeIndex];

	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Cyan, FString::Printf(TEXT("Zmieniono tryb strzelania na: %s"),
		CurrentFireMode == EFireMode::FullAuto ? TEXT("Full-Auto") : TEXT("Semi-Auto")));
}

void AWeaponBase::Reload()
{
	if (bIsReloading || CurrentAmmo == MaxAmmo) return;

	bIsReloading = true;
	StopFiring();

	GEngine->AddOnScreenDebugMessage(-1, ReloadTime, FColor::Yellow, TEXT("Przeladowywanie"));

	FTimerHandle ReloadTimerHandle;
	GetWorldTimerManager().SetTimer(ReloadTimerHandle, this, &AWeaponBase::FinishReload, ReloadTime, false);
}

void AWeaponBase::FinishReload()
{
	bIsReloading = false;
	CurrentAmmo = MaxAmmo;

	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, TEXT("Przeladowano"));
}