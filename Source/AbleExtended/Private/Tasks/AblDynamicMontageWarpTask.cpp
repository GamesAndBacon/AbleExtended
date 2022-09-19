// Fill out your copyright notice in the Description page of Project Settings.

#include "Tasks/AblDynamicMontageWarpTask.h"

#include "CoreMinimal.h"
#include "MotionWarping/Public/RootMotionModifier.h"
#include "MotionWarping/Public/RootMotionModifier_SkewWarp.h"
#include "MotionWarping/Public/MotionWarpingComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "ablAbility.h"
#include "AbleCore/Private/AbleCorePrivate.h"
#include "ablAbilityComponent.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimInstanceProxy.h"
#include "Animation/AnimMontage.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Character.h"


//we have to declare the scratchpad constuctor and destructor
UAblDynamicMontageScratchPad::UAblDynamicMontageScratchPad()
{
}

UAblDynamicMontageScratchPad::~UAblDynamicMontageScratchPad()
{
}

//init
UAblDynamicMontageWarpTask::UAblDynamicMontageWarpTask(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer),
m_TaskRealm(EAblAbilityTaskRealm::ATR_ClientAndServer),
m_Animation(nullptr),
m_SlotName("Ability"),
m_LoopCount(1),
m_BlendOutTriggerTime(-1),
m_BlendInTriggerTime(0),
m_UseRootMotionScaling(false),
m_RMScale_end_Time(1),
m_RMScale_Value(1,1,1),
m_WarpTargetName("Target"),
m_DistanceFromTarget(1),
m_RMSkew_end_Time(1),
m_RMSkew_rot_time(1),
m_RMSkew_rotation_type(EMotionWarpRotationType::Facing)\
{
}

// need the destructor
UAblDynamicMontageWarpTask::~UAblDynamicMontageWarpTask()
{
}

// on task start
void UAblDynamicMontageWarpTask::OnTaskStart(const TWeakObjectPtr<const UAblAbilityContext>& Context) const
{
	Super::OnTaskStart(Context);
	if(Cast<ACharacter>(Context->GetOwner()))
	{
		// we get our dynamic properties and save them.
		const UAnimationAsset* AnimationAsset = ABL_GET_DYNAMIC_PROPERTY_VALUE(Context, m_Animation);
		FVector d_Skewloc = ABL_GET_DYNAMIC_PROPERTY_VALUE(Context, m_RMSkew_Vector);
		FVector d_Skewscale = ABL_GET_DYNAMIC_PROPERTY_VALUE(Context, m_RMScale_Value);
		

		//local var
		FVector ownerloc = Context->GetOwner()->GetActorLocation();
		bool bUsedDistanceFromEnemy = false;
	
		// we get our scratchpad and set the mesh in it. 
		UAblDynamicMontageScratchPad* ScratchPad = CastChecked<UAblDynamicMontageScratchPad>(Context->GetScratchPadForTask(this));
		ScratchPad->Mesh  = Cast<ACharacter>(Context->GetOwner())->GetMesh();

		//our resulting montage.
		const UAnimMontage* CurrentAnim;
	
		//check if its a montage or a sequence to convert
		if(Cast<UAnimMontage>(AnimationAsset))
		{
			CurrentAnim = Cast<UAnimMontage>(AnimationAsset);
			ScratchPad->Mesh->GetAnimInstance()->Montage_Play(const_cast<UAnimMontage*>(CurrentAnim),Context->GetAbility()->GetPlayRate(Context.Get()));
		}
		else
		{
			//set local var of for animation sequence
			const UAnimSequenceBase* SequenceAsset = Cast<UAnimSequenceBase>(AnimationAsset);
			//play the animation
			//ScratchPad->Mesh->GetAnimInstance()->PlaySlotAnimationAsDynamicMontage_WithBlendSettings(const_cast<UAnimSequenceBase*>(SequenceAsset), m_SlotName, m_Blend_in_Settings, m_Blend_out_Settings, Context->GetAbility()->GetPlayRate(Context.Get()), m_LoopCount, m_BlendOutTriggerTime, m_BlendInTriggerTime );
			// save the returned montage locally
			CurrentAnim = ScratchPad->Mesh->GetAnimInstance()->PlaySlotAnimationAsDynamicMontage_WithBlendSettings(const_cast<UAnimSequenceBase*>(SequenceAsset), m_SlotName, m_Blend_in_Settings, m_Blend_out_Settings, Context->GetAbility()->GetPlayRate(Context.Get()), m_LoopCount, m_BlendOutTriggerTime, m_BlendInTriggerTime );
		}

	
		// get the warp component before logic to test against
		UMotionWarpingComponent* warpcomp = Context->GetOwner()->FindComponentByClass<UMotionWarpingComponent>();
		if(warpcomp != nullptr)
		{
			// MOTION WARPING LOGIC
			if(m_UseRootMotionScaling == true)
			{
				// create the modifier
				URootMotionModifier* mod =	URootMotionModifier_Scale::AddRootMotionModifierScale(warpcomp, CurrentAnim, m_RMScale_Start_Time, m_RMScale_end_Time, d_Skewscale);
				// apply the modifier
				warpcomp->AddModifier(mod);
			}
			// MOTION WARPING LOGIC
			if(m_UseRootMotionSkew == true)
			{
				// get the warp component
				// local transform to split the transform and add our location
				if(!m_UseOutterTarget)
				{
					FRotator rot = FRotator::ZeroRotator;
					rot.Yaw = UKismetMathLibrary::FindLookAtRotation(ownerloc,d_Skewloc).Yaw;
					warpcomp->AddOrUpdateWarpTargetFromLocationAndRotation(m_WarpTargetName, d_Skewloc, rot);
				}
				if(m_DistanceFromTarget && warpcomp->FindWarpTarget(m_WarpTargetName))
				{
					//save our original target location
					FVector targetlocation = warpcomp->FindWarpTarget(m_WarpTargetName)->GetLocation();
					//create our rotator
					FRotator rot = FRotator::ZeroRotator;
					rot.Yaw = UKismetMathLibrary::FindLookAtRotation(ownerloc,targetlocation).Yaw;
					
					// get the distance we want from out dynamic property 
					float d_distancefromtarget = ABL_GET_DYNAMIC_PROPERTY_VALUE(Context, m_DistanceFromTarget);
					// get the current vector
					FVector travelVector = ownerloc - targetlocation;
					// get the length
					float fullLength = travelVector.Length();
					// normalise it to 1
					travelVector.Normalize();
					// multiply it to get new length
					FVector newVector =  travelVector * (fullLength - d_distancefromtarget);
					// apply new location to a different targetname so we dont override the original
					warpcomp->AddOrUpdateWarpTargetFromLocationAndRotation("distanceTarget", ownerloc - newVector,rot);

					bUsedDistanceFromEnemy = true;
				}
				
				// create the modifier
				URootMotionModifier_SkewWarp* mod;
				if(bUsedDistanceFromEnemy)
				{
					mod = URootMotionModifier_SkewWarp::AddRootMotionModifierSkewWarp(warpcomp,CurrentAnim, m_RMSkew_Start_Time,m_RMSkew_end_Time,"distanceTarget",EWarpPointAnimProvider::None,FTransform(), FName(""),true,true,true,EMotionWarpRotationType::Default,m_RMSkew_rot_time);
				}
				else
				{
					mod = URootMotionModifier_SkewWarp::AddRootMotionModifierSkewWarp(warpcomp,CurrentAnim, m_RMSkew_Start_Time,m_RMSkew_end_Time,m_WarpTargetName,EWarpPointAnimProvider::None,FTransform(), FName(""),true,true,true,m_RMSkew_rotation_type,m_RMSkew_rot_time);
				}
				
			// apply the modifier
			warpcomp->AddModifier(mod);
			}
		}
	}
}

// on task end
void UAblDynamicMontageWarpTask::OnTaskEnd(const TWeakObjectPtr<const UAblAbilityContext>& Context,
	const EAblAbilityTaskResult Result) const
{
	Super::OnTaskEnd(Context, Result);
	//if we stop at end, we get our scratchpad, and we stop with blend settings on the saved mesh. 
	if(m_StopAtEnd)
	{
		UAblDynamicMontageScratchPad* ScratchPad = CastChecked<UAblDynamicMontageScratchPad>(Context->GetScratchPadForTask(this));
		if(ScratchPad->Mesh != nullptr)
		{
			ScratchPad->Mesh->GetAnimInstance()->Montage_StopWithBlendSettings(m_Blend_out_Settings,ScratchPad->Mesh->GetAnimInstance()->GetCurrentActiveMontage());
		}
	}
}


// you HAVE to create a scratchpad. even if its just empty. THIS HAS TO BE ADDED
UAblAbilityTaskScratchPad* UAblDynamicMontageWarpTask::CreateScratchPad(const TWeakObjectPtr<UAblAbilityContext>& Context) const
{
	return NewObject<UAblDynamicMontageScratchPad>(Context.Get());
}

TStatId UAblDynamicMontageWarpTask::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UAblDynamicMontageWarpTask, STATGROUP_Able);
}

// this is where you create your bindings. #include "ablAbility.h" <- must be included
void UAblDynamicMontageWarpTask::BindDynamicDelegates(UAblAbility* Ability)
{
	Super::BindDynamicDelegates(Ability);
	ABL_BIND_DYNAMIC_PROPERTY(Ability, m_DistanceFromTarget, TEXT("Distance From Target"));
	ABL_BIND_DYNAMIC_PROPERTY(Ability, m_Animation, TEXT("Animation"));
	ABL_BIND_DYNAMIC_PROPERTY(Ability, m_RMScale_Value, TEXT("Scale Value"));
	ABL_BIND_DYNAMIC_PROPERTY(Ability, m_RMSkew_Vector, TEXT("Skew Vector"));
}

