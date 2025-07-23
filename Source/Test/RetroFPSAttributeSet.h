// Fill out your copyright notice in the Description page of Project Settings.

// RestroFPSAttributeSet.h

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffectExtension.h"
#include "RetroFPSAttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
  GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
  GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName)       \
  GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName)       \
  GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

UCLASS()
class TEST_API URetroFPSAttributeSet : public UAttributeSet
{
    GENERATED_BODY()

public:
    URetroFPSAttributeSet();

    /** Current Health (0–100) */
    UPROPERTY(BlueprintReadOnly, Category = "Attributes")
    FGameplayAttributeData Health;
    ATTRIBUTE_ACCESSORS(URetroFPSAttributeSet, Health);

    /** Current Armor (0–100) */
    UPROPERTY(BlueprintReadOnly, Category = "Attributes")
    FGameplayAttributeData Armor;
    ATTRIBUTE_ACCESSORS(URetroFPSAttributeSet, Armor);

    /** Clamp any incoming change to [0,100] */
    virtual void PreAttributeChange(
        const FGameplayAttribute& Attribute,
        float& NewValue
    ) override;

    /** Called *before* each GameplayEffect modifier is applied */
    virtual bool PreGameplayEffectExecute(
        FGameplayEffectModCallbackData& Data
    ) override;

    /** Called *after* modifiers have been applied */
    virtual void PostGameplayEffectExecute(
        const FGameplayEffectModCallbackData& Data
    ) override;

private:
    /** Armor value just before a damage hit */
    float PreHitArmor;
};