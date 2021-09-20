#pragma once

#include "CoreMinimal.h"

#include "Engine/DataTable.h"
#include "CustomStructs.generated.h"

USTRUCT(BlueprintType)
struct FPlayerStats : public FTableRowBase
{
	GENERATED_BODY()
	
	// Test
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FString Test;

	// Test 1
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FString Test1;
};