// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ablAbilityTypes.h"
#include "RootMotionModifier.h"
#include "Components/SkeletalMeshComponent.h"
#include "Tasks/IAblAbilityTask.h"
#include "UObject/ObjectMacros.h"

#include "AblDynamicMontageWarpTask.generated.h"

#define LOCTEXT_NAMESPACE "AblAbilityTask"

class UAnimationAsset;
class UAblAbilityComponent;
class UAblAbilityContext;


// WE CREATE A SCRATCHPAD AT RUNTIME
UCLASS(Transient)
class UAblDynamicMontageScratchPad : public UAblAbilityTaskScratchPad
{
	GENERATED_BODY()
public:
	UAblDynamicMontageScratchPad();
	virtual ~UAblDynamicMontageScratchPad();
	
	// The Skeletal Mesh Component saved into our scratchpad to use globally across the task
	UPROPERTY(transient)
	USkeletalMeshComponent* Mesh;
};

UCLASS()
class ABLEEXTENDED_API UAblDynamicMontageWarpTask : public UAblAbilityTask
{
	GENERATED_BODY()
public:	
	UAblDynamicMontageWarpTask(const FObjectInitializer& ObjectInitializer);
	virtual ~UAblDynamicMontageWarpTask();

// MAIN FUNCTIONS
	
	/* Start our Task. */
	virtual void OnTaskStart(const TWeakObjectPtr<const UAblAbilityContext>& Context) const override;
	
	/* End our Task. */
	virtual void OnTaskEnd(const TWeakObjectPtr<const UAblAbilityContext>& Context, const EAblAbilityTaskResult Result) const override;
	
	/* Returns true if our Task only lasts a single frame. */
	virtual bool IsSingleFrame() const override { return false; }
	
	/* Returns the realm this Task belongs to. */
	virtual EAblAbilityTaskRealm GetTaskRealm() const override { return m_TaskRealm; }

	/* Creates the Scratchpad for our Task. */
	virtual UAblAbilityTaskScratchPad* CreateScratchPad(const TWeakObjectPtr<UAblAbilityContext>& Context) const;

	/* Returns the Profiler Stat ID for our Task. */
	virtual TStatId GetStatId() const override;

	/* Setup Dynamic Binding. */
	virtual void BindDynamicDelegates( UAblAbility* Ability ) override;

	
	// EDITOR FUNCTIONS
#if WITH_EDITOR
	/* Returns the category for our Task. */
	virtual FText GetTaskCategory() const override { return LOCTEXT("AblDynamicMontageCategory", "Animation"); }
	
	/* Returns the name of our Task. */
	virtual FText GetTaskName() const override { return LOCTEXT("AblDynamicMontageTask", "Montage with Warping"); }

	virtual FLinearColor GetTaskColor() const override {return FLinearColor(255.0f / 255.0f, 0.0 / 255.0f, 0.0f / 255.0f);};

#endif

	
	// TASK VARIABLES
protected:

	UPROPERTY(EditAnywhere, Category = "Realm", meta = (DisplayName = "Realm"))
	TEnumAsByte<EAblAbilityTaskRealm> m_TaskRealm;

	
//ANIMATION VARIABLES 
	UPROPERTY(EditAnywhere, Category="Animation", meta = (DisplayName = "Animation", AblBindableProperty, AblDefaultBinding = "OnGetAnimationAssetBP", AllowedClasses = "AnimMontage,AnimSequence"))
	UAnimationAsset* m_Animation;
// delegate for dynamic property
	UPROPERTY()
	FGetAblAnimation m_AnimationDelegate;
	
	UPROPERTY(EditAnywhere, Category="Animation", meta = (DisplayName = "slot Name"))
	FName m_SlotName;

	UPROPERTY(EditAnywhere, Category="Animation", meta = (DisplayName = "Loop Count"))
	float m_LoopCount;
	
	UPROPERTY(EditAnywhere, Category="Animation", meta = (DisplayName = "Stop at End"))
	bool m_StopAtEnd;

	
// BLENDING VARIABLES
	UPROPERTY(EditAnywhere, Category="Blending", meta = (DisplayName = "Montage Blend in "))
	FMontageBlendSettings m_Blend_in_Settings;

	UPROPERTY(EditAnywhere, Category="Blending", meta = (DisplayName = "Montage Blend Out"))
	FMontageBlendSettings m_Blend_out_Settings;
	
	UPROPERTY(EditAnywhere, Category="Blending", meta = (DisplayName = "Blend Out time"))
	float m_BlendOutTriggerTime;
	
	UPROPERTY(EditAnywhere, Category="Blending", meta = (DisplayName = "Blend In Time"))
	float m_BlendInTriggerTime;

	
// MOTION WARPING SCALE VARIABLES
	UPROPERTY(EditAnywhere, Category="RootMotionScale", meta = (DisplayName = "Use Root Motion Scaling"))
	bool m_UseRootMotionScaling;

	UPROPERTY(EditAnywhere, Category="RootMotionScale", meta = (DisplayName = "start Time"))
	float m_RMScale_Start_Time;

	UPROPERTY(EditAnywhere, Category="RootMotionScale", meta = (DisplayName = "end Time"))
	float m_RMScale_end_Time;
	
	UPROPERTY(EditAnywhere, Category="RootMotionScale", meta = (DisplayName = "Scale Value", AblBindableProperty))
	FVector m_RMScale_Value;
// the delegate for dynamic property
	UPROPERTY()
	FGetAblVector m_RMScale_ValueDelegate;

	
// MOTION WARPING SKEW VARIABLES
	UPROPERTY(EditAnywhere, Category="RootMotionSkew", meta = (DisplayName = "Use Outter Target"))
	bool m_UseOutterTarget;
	
	UPROPERTY(EditAnywhere, Category="RootMotionSkew", meta = (DisplayName = "Use Root Motion Skew"))
	bool m_UseRootMotionSkew;
	
	UPROPERTY(EditAnywhere, Category="RootMotionSkew", meta = (DisplayName = "Warp Target Name"))
	FName m_WarpTargetName;

	UPROPERTY(EditAnywhere, Category="RootMotionSkew", meta = (DisplayName = "start Time"))
	float m_RMSkew_Start_Time;

	UPROPERTY(EditAnywhere, Category="RootMotionSkew", meta = (DisplayName = "end Time"))
	float m_RMSkew_end_Time
	;
	UPROPERTY(EditAnywhere, Category="RootMotionSkew", meta = (DisplayName = "Use distance from target"))
	bool m_UseDistanceFromTarget;

	UPROPERTY(EditAnywhere, Category="RootMotionSkew", meta = (DisplayName = "Distance From Target", AblBindableProperty))
	float m_DistanceFromTarget;
// the delegate for dynamic property
	UPROPERTY()
	FGetAblFloat m_DistanceFromTargetDelegate;
	
	UPROPERTY(EditAnywhere, Category = "RootMotionSkew", meta = (DisplayName = "Skew Vector", AblBindableProperty))
	FVector m_RMSkew_Vector;
// the delegate for dynamic property
	UPROPERTY()
	FGetAblVector m_RMSkew_VectorDelegate;

	UPROPERTY(EditAnywhere, Category="RootMotionSkew", meta = (DisplayName = "Rotation Time"))
	float m_RMSkew_rot_time;
	
	UPROPERTY(EditAnywhere, Category="RootMotionSkew", meta = (DisplayName = "Rotation Type"))
	EMotionWarpRotationType m_RMSkew_rotation_type;
};
