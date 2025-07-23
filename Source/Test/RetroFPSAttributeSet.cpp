// Fill out your copyright notice in the Description page of Project Settings.

// RestroFPSAttributeSet.cpp

#include "RetroFPSAttributeSet.h"
#include "Engine/Engine.h"            // for GEngine->AddOnScreenDebugMessage
#include "GameplayEffectExtension.h"  // for FGameplayEffectModCallbackData
#include "Math/UnrealMathUtility.h"   // for FMath

URetroFPSAttributeSet::URetroFPSAttributeSet()
    : PreHitArmor(0.f)
{
}

void URetroFPSAttributeSet::PreAttributeChange(
    const FGameplayAttribute& Attribute,
    float& NewValue
)
{
    // Ensure Health and Armor always stay within [0,100]
    NewValue = FMath::Clamp(NewValue, 0.f, 100.f);
}

bool URetroFPSAttributeSet::PreGameplayEffectExecute(
    FGameplayEffectModCallbackData& Data
)
{
    // Only record the old armor value if this GameplayEffect is dealing damage to Armor
    if (Data.EvaluatedData.Attribute == GetArmorAttribute()
        && Data.EvaluatedData.Magnitude < 0.f)
    {
        PreHitArmor = Armor.GetCurrentValue();
    }
    return true;  // allow the effect to proceed
}

void URetroFPSAttributeSet::PostGameplayEffectExecute(
    const FGameplayEffectModCallbackData& Data
)
{
    // 1) Handle Armor damage soak
    if (Data.EvaluatedData.Attribute == GetArmorAttribute())
    {
        const float DeltaArmor = Data.EvaluatedData.Magnitude;
        if (DeltaArmor < 0.f)  // only on damage
        {
            const float IncomingDamage = -DeltaArmor;

            // Determine how much damage Armor actually absorbs
            const float DamageToArmor = FMath::Min(PreHitArmor, IncomingDamage);

            // Compute new armor value (never below zero)
            const float NewArmor = PreHitArmor - DamageToArmor;
            SetArmor(FMath::Clamp(NewArmor, 0.f, 100.f));

            // Any remaining damage spills over into Health
            const float Spillover = IncomingDamage - DamageToArmor;
            if (Spillover > 0.f)
            {
                const float NewHealth = Health.GetCurrentValue() - Spillover;
                SetHealth(FMath::Clamp(NewHealth, 0.f, 100.f));
            }

            // Debug print to screen for verification
            if (GEngine)
            {
                GEngine->AddOnScreenDebugMessage(
                    -1, 2.5f, FColor::Yellow,
                    FString::Printf(
                        TEXT("[Soak] Pre=%.1f  NewA=%.1f  Spill=%.1f  Health=%.1f"),
                        PreHitArmor,
                        Armor.GetCurrentValue(),
                        Spillover,
                        Health.GetCurrentValue()
                    )
                );
            }

            return;  // done handling armor hits
        }
    }

    // 2) Handle direct Health modifications (if any)
    if (Data.EvaluatedData.Attribute == GetHealthAttribute())
    {
        const float DeltaHealth = Data.EvaluatedData.Magnitude;
        if (DeltaHealth < 0.f)  // only clamp on damage
        {
            SetHealth(FMath::Clamp(Health.GetCurrentValue(), 0.f, 100.f));
        }
    }
}