// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/InteractionInterface.h"
#include "Pickup.generated.h"


struct FInteractableData;
class UItemBase;
class UStaticMeshComponent;

UCLASS()
class RESIDENTINVENTORY_API APickup : public AActor, public IInteractionInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	APickup();

	// getter for the Item
	FORCEINLINE UItemBase* GetItemData() const { return ItemReference;}

	virtual void BeginFocus() override;
	virtual void EndFocus() override;

	UFUNCTION(BlueprintCallable, Category = "Pickup")
	void SetPickupData(UItemBase* InItemInstance, int32 InQuantity);	
	
	UPROPERTY(EditInstanceOnly, Category = "Pickup | Item Initialization")
	int32 PickupItemQuantity;
	
	UPROPERTY(VisibleAnywhere, Category = "Pickup | Item Reference")
	UItemBase* ItemReference;
	
protected:
	
	UPROPERTY(VisibleAnywhere, Category = "Pickup | Components")
	UStaticMeshComponent* PickupStaticMesh;	


	
	virtual void BeginPlay() override;

	virtual void Interact(AResidentInventoryCharacter* PlayerCharacter) override;

	void TakePickup(const AResidentInventoryCharacter* PickupTaker);

	void OnPickupDataReceived() const;

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "OnPickupDataReceived"), Category = "Pickup")
	void K2_OnPickupDataReceived();

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;	
#endif
};