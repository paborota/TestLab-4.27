// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class TESTLAB_API CustomEnums
{

};

UENUM(BlueprintType)
enum class EObjectType : uint8 // Used only in ProtoCharacter to determine objects that are interactable
{
 Interactable	UMETA(DisplayName="Interactable"),
 None			UMETA(DisplayName="NONE")
};

UENUM(BlueprintType)
enum class EMantleType : uint8
{
 Mantle2M       UMETA(DisplayName="Mantle2M"),
 Mantle1M       UMETA(DisplayName="Mantle1M"),
 None			        UMETA(DisplayName="NONE"),
 MantleDown1M   UMETA(DisplayName="MantleDown1M"),
 MantleDown2M   UMETA(DisplayName="MantleDown2M"),
 MantleOver     UMETA(DisplayName="MantleOver")
};