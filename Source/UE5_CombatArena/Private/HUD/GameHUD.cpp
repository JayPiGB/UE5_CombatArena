#include "HUD/GameHUD.h"
#include "HUD/PlayerHUD.h"


void AGameHUD::BeginPlay()
{
	Super::BeginPlay();

	UWorld* world = GetWorld();
	if (world)
	{
		APlayerController* controller = world->GetFirstPlayerController();
		if (controller && PlayerHUDClass)
		{
			PlayerHUD = CreateWidget<UPlayerHUD>(controller, PlayerHUDClass);
			PlayerHUD->AddToViewport();
		}
	}
}
