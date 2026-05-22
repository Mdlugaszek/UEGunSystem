#include "ProjectileBase.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"

AProjectileBase::AProjectileBase()
{
	PrimaryActorTick.bCanEverTick = true;

	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(5.0f);
	CollisionComp->BodyInstance.SetCollisionProfileName("Projectile");
	CollisionComp->OnComponentHit.AddDynamic(this, &AProjectileBase::OnHit);
	RootComponent = CollisionComp;

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = false;
	ProjectileMovement->ProjectileGravityScale = 0.0f;
}

void AProjectileBase::BeginPlay()
{
	if (ProjectileMovement && InitialProjectileSpeed > 0.0f)
	{
		ProjectileMovement->InitialSpeed = InitialProjectileSpeed;
		ProjectileMovement->MaxSpeed = InitialProjectileSpeed;
	}

	Super::BeginPlay();
	StartLocation = GetActorLocation();

	if (CollisionComp)
	{
		CollisionComp->SetCollisionResponseToChannel(CollisionComp->GetCollisionObjectType(), ECR_Ignore);

		APawn* MyInstigator = GetInstigator();
		if (MyInstigator)
		{
			CollisionComp->IgnoreActorWhenMoving(MyInstigator, true);
			MyInstigator->MoveIgnoreActorAdd(this);
		}
	}
}

void AProjectileBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (MaxRange > 0.0f)
	{
		float DistanceTraveledSq = FVector::DistSquared(StartLocation, GetActorLocation());
		if (DistanceTraveledSq >= FMath::Square(MaxRange))
		{
			GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Yellow, TEXT("pocisk przelecial max zasieg"));
			Destroy();
		}
	}
}

void AProjectileBase::InitializeProjectile(float Damage, float Range, float Speed)
{
	DamageToDeal = Damage;
	MaxRange = Range;

	if (ProjectileMovement && Speed > 0.0f)
	{
		ProjectileMovement->InitialSpeed = Speed;
		ProjectileMovement->MaxSpeed = Speed;

		ProjectileMovement->Velocity = GetActorForwardVector() * Speed;
		ProjectileMovement->UpdateComponentVelocity();
	}
}

void AProjectileBase::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if ((OtherActor == nullptr) || (OtherActor == this) || (OtherComp == nullptr))
	{
		return;
	}

	if (OtherActor->IsA(AProjectileBase::StaticClass()))
	{
		return;
	}

	if (OtherActor == GetInstigator())
	{
		return;
	}

	UMaterialInterface* HitMaterial = OtherComp->GetMaterial(0);

	bool bHitEnemyMaterial = HitMaterial && HitMaterial->GetName().Contains(TEXT("M_Enemy"));

	if (OtherActor->IsA(ACharacter::StaticClass()) || bHitEnemyMaterial)
	{
		UGameplayStatics::ApplyDamage(OtherActor, DamageToDeal, GetInstigatorController(), this, UDamageType::StaticClass());

		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, FString::Printf(TEXT("Trafiono %s Zadano %f obrazen"),
			bHitEnemyMaterial ? TEXT("Material Enemy") : TEXT("Character"), DamageToDeal));
	}
	else
	{
		FString SurfaceName = HitMaterial ? HitMaterial->GetName() : TEXT("Nieznana");
		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::White, FString::Printf(TEXT("Trafiono powierzchnie: %s"), *SurfaceName));
	}

	Destroy();
}