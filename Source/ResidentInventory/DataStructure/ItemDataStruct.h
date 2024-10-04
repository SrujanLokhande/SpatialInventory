#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "ItemDataStruct.generated.h"


// using the uint8 for enums for memory saving and not allocating the full int bytes for enums
UENUM()
enum class EItemRarity : uint8
{
	Common UMETA(DisplayName = "Common"),
	Rare UMETA(DisplayName = "Rare"),
	Epic UMETA(DisplayName = "Epic"),
	Legendary UMETA(DisplayName = "Legendary")	
};


// various types of items that can be picked up
UENUM()
enum class EItemType : uint8
{
	Potions UMETA(DisplayName = "Potions"),
	Herbs UMETA(DisplayName = "Herbs"),
	Weapons UMETA(DisplayName = "Weapons"),
	QuestItems UMETA(DisplayName = "QuestItems"),
	Mundane UMETA(DisplayName = "Mundane")
	
};


// the information of the items
USTRUCT()
struct FITemStats
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere)
	float DamageValue;
	
	UPROPERTY(EditAnywhere)
	float HealthRestorationAmount;

	UPROPERTY(EditAnywhere)
	float SellValue;	
};


// the item information to be displayed to user
USTRUCT()
struct FItemTextData
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere)
	FText Name;
	
	UPROPERTY(EditAnywhere)
	FText Description;
	
	UPROPERTY(EditAnywhere)
	FText InteractionText;

	UPROPERTY(EditAnywhere)
	FText UsageText;
};

// the data for the items like the weight, size
USTRUCT()
struct FItemNumericData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	float ItemWeight;

	UPROPERTY(EditAnywhere)
	int32 maxStackSize;

	UPROPERTY(EditAnywhere)
	bool bIsStackable;

	UPROPERTY(EditAnywhere)
	FVector2D ItemDimensions;
};


// the icons for the assets
USTRUCT()
struct FItemAssetData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	UTexture2D* Icon;

	UPROPERTY(EditAnywhere)
	UStaticMesh* ItemMesh;	
};

// FTableRow - base class for data tables 
USTRUCT()
struct FItemData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "ItemData")
	FName ID;

	UPROPERTY(EditAnywhere, Category = "ItemData")
	EItemType ItemType;

	UPROPERTY(EditAnywhere, Category = "ItemData")
	EItemRarity ItemRarity;

	UPROPERTY(EditAnywhere, Category = "ItemData")
	FITemStats ItemStatistics;

	UPROPERTY(EditAnywhere, Category = "ItemData")
	FItemTextData TextData;

	UPROPERTY(EditAnywhere, Category = "ItemData")
	FItemNumericData NumericData;

	UPROPERTY(EditAnywhere, Category = "ItemData")
	FItemAssetData AssetData;
};