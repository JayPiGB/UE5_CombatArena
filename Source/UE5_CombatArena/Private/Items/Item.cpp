#include "Items/Item.h"
#include "Components/SphereComponent.h"

AItem::AItem() : runningTime(0.0f), hoverAmplitude(2.0f)
{
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMesh"));
	SetRootComponent(Mesh);
	Mesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);

	Sphere = CreateDefaultSubobject<USphereComponent>(TEXT("ItemSphere"));
	Sphere->SetupAttachment(GetRootComponent());
}

void AItem::BeginPlay()
{
	Super::BeginPlay();
	
	Sphere->OnComponentBeginOverlap.AddDynamic(this, &AItem::OnSphereBeginOverlap);
	Sphere->OnComponentEndOverlap.AddDynamic(this, &AItem::OnSphereEndOverlap);
}

void AItem::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
}

void AItem::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
}

void AItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	runningTime += DeltaTime;

	if (State == EItemState::EIS_Hovering)
	{
		AddActorWorldOffset(FVector(0.0f, 0.0f, GetHoverOffset(DeltaTime)));
	}
}

float AItem::GetHoverOffset(float DeltaTime) const
{
	return hoverAmplitude * FMath::Sin(runningTime * 5.0f);
}
