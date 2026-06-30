#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MinimapCaptureActor.generated.h"

UCLASS()
class FPS_API AMinimapCaptureActor : public AActor
{
    GENERATED_BODY()

public:
    AMinimapCaptureActor();

protected:
    virtual void BeginPlay() override;
   

    

private:

    UPROPERTY(VisibleAnywhere)
    class USceneCaptureComponent2D* Capture;

public:
    
    UPROPERTY(EditAnywhere, Category = "Minimap")
    class UTextureRenderTarget2D* MinimapRT;
    
    UTextureRenderTarget2D* GetRenderTarget();
    


    void SetRenderTarget(UTextureRenderTarget2D* RT);

};