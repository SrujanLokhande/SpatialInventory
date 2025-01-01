#pragma once
#include "CoreMinimal.h"
#include "Point2D.generated.h"

// For the slot based Inventory Array
USTRUCT(BlueprintType)
struct FPoint2D
{
	GENERATED_BODY()

	FPoint2D()
	{
		X = 0;
		Y = 0;
	}

	FPoint2D(const int32 InX, const int32 InY)
	{
		X = InX;
		Y = InY;
	}
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = 0, UIMin = 0))
	int32 X;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = 0, UIMin = 0))
	int32 Y;

	bool operator == (const FPoint2D& Other) const
	{
		return Other.X == X && Other.Y == Y;
	}

	FPoint2D operator + (const FPoint2D& Other) const
	{
		return FPoint2D(Other.X + X, Other.Y + Y);
	}

	bool operator > (const FPoint2D& Other) const
	{
		return Other.X > X && Other.Y > Y;
	}

	bool operator >= (const FPoint2D& Other) const
	{
		return Other.X >= X && Other.Y >= Y;
	}

	bool operator < (const FPoint2D& Other) const
	{
		return (Other.X < X && Other.Y < Y);
	}

	bool operator <= (const FPoint2D& Other) const
	{
		return (Other.X <= X && Other.Y <= Y);
	}
};

