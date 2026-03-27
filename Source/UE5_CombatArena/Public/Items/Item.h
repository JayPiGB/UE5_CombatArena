#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Item.generated.h"

enum class EItemState
{
	EIS_Hovering,
	EIS_Equipped
};

class UStaticMeshComponent;
class USphereComponent;

UCLASS()
class UE5_COMBATARENA_API AItem : public AActor
{
	GENERATED_BODY()
	
public:	
	AItem();

	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	virtual void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* Mesh;

	EItemState State = EItemState::EIS_Hovering;

private:
	float GetHoverOffset(float DeltaTime) const;

	float runningTime;

	float hoverAmplitude;

	UPROPERTY(VisibleAnywhere)
	USphereComponent* Sphere;
};
