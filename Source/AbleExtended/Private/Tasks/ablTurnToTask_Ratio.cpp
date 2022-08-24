// Copyright (c) Extra Life Studios, LLC. All rights reserved.

#include "Tasks/ablTurnToTask_Ratio.h"

#include "ablAbility.h"
#include "ablSubSystem.h"
#include "AbleCorePrivate.h"

#if (!UE_BUILD_SHIPPING)
#include "ablAbilityUtilities.h"
#endif

#define LOCTEXT_NAMESPACE "AblAbilityTask"

UAblTurnToRatioTaskScratchPad::UAblTurnToRatioTaskScratchPad()
{

}

UAblTurnToRatioTaskScratchPad::~UAblTurnToRatioTaskScratchPad()
{

}

UAblTurnToTask_Ratio::UAblTurnToTask_Ratio(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer),
	m_RotationTarget(EAblAbilityTargetType::ATT_TargetActor),
	m_UseRotationVector(false),
	m_RotationVector(ForceInitToZero),
	m_TrackTarget(false),
	m_SetYaw(true),
	m_SetPitch(false),
	m_TaskRealm(EAblAbilityTaskRealm::ATR_ClientAndServer)
{

}

UAblTurnToTask_Ratio::~UAblTurnToTask_Ratio()
{

}

void UAblTurnToTask_Ratio::OnTaskStart(const TWeakObjectPtr<const UAblAbilityContext>& Context) const
{
	Super::OnTaskStart(Context);

	UAblTurnToRatioTaskScratchPad* ScratchPad = Cast<UAblTurnToRatioTaskScratchPad>(Context->GetScratchPadForTask(this));
	check(ScratchPad);
	ScratchPad->InProgressTurn.Empty();
	ScratchPad->TurningBlend = m_Blend;
	ScratchPad->StartRotation = Context->GetOwner()->GetActorRotation();

	AActor* TargetActor = GetSingleActorFromTargetType(Context, m_RotationTarget.GetValue());

	TArray<TWeakObjectPtr<AActor>> TaskTargets;
	GetActorsForTask(Context, TaskTargets);

	for (TWeakObjectPtr<AActor>& TurnTarget : TaskTargets)
	{
		FRotator TargetRotation = GetTargetRotation(Context, TurnTarget.Get(), TargetActor);
#if !(UE_BUILD_SHIPPING)
		if (IsVerbose())
		{
			PrintVerbose(Context, FString::Printf(TEXT("Setting up turning for Actor %s with a Target turn of %s."), *TurnTarget->GetName(), *TargetRotation.ToCompactString()));
		}
#endif
		ScratchPad->InProgressTurn.Add(FTurnToTaskRatioEntry(TurnTarget.Get(), TargetRotation));
	}

}

void UAblTurnToTask_Ratio::OnTaskTick(const TWeakObjectPtr<const UAblAbilityContext>& Context, float deltaTime) const
{
	Super::OnTaskTick(Context, deltaTime);
	
	UAblTurnToRatioTaskScratchPad* ScratchPad = Cast<UAblTurnToRatioTaskScratchPad>(Context->GetScratchPadForTask(this));
	check(ScratchPad);
	
	const float BlendingValue = (Context->GetCurrentTime() - GetStartTime()) / (GetEndTime() - GetStartTime());
	
	AActor* TargetActor = GetSingleActorFromTargetType(Context, m_RotationTarget.GetValue());

	for (FTurnToTaskRatioEntry& Entry : ScratchPad->InProgressTurn)
	{
		if (Entry.Actor.IsValid())
		{
			if (m_TrackTarget)
			{
				// Update our Target rotation.
				Entry.Target = GetTargetRotation(Context, Entry.Actor.Get(), TargetActor);
			}

			FRotator LerpedRotation = FMath::Lerp(ScratchPad->StartRotation, Entry.Target, BlendingValue);
#if !(UE_BUILD_SHIPPING)
			if (IsVerbose())
			{
				PrintVerbose(Context, FString::Printf(TEXT("Setting Actor %s rotation to %s ."), *Entry.Actor->GetName(), *LerpedRotation.ToCompactString()));
			}
#endif
			Entry.Actor->SetActorRotation(LerpedRotation);
		}
	}
}

void UAblTurnToTask_Ratio::OnTaskEnd(const TWeakObjectPtr<const UAblAbilityContext>& Context, const EAblAbilityTaskResult result) const
{
	Super::OnTaskEnd(Context, result);

	if (!Context.IsValid())
	{
		return;
	}

	UAblTurnToRatioTaskScratchPad* ScratchPad = Cast<UAblTurnToRatioTaskScratchPad>(Context->GetScratchPadForTask(this));
	check(ScratchPad);

	for (const FTurnToTaskRatioEntry& Entry : ScratchPad->InProgressTurn)
	{
		if (Entry.Actor.IsValid())
		{
#if !(UE_BUILD_SHIPPING)
			if (IsVerbose())
			{
				PrintVerbose(Context, FString::Printf(TEXT("Setting Actor %s rotation to %s ."), *Entry.Actor->GetName(), *Entry.Target.ToCompactString()));
			}
#endif
			Entry.Actor->SetActorRotation(Entry.Target);
		}
	}
}

UAblAbilityTaskScratchPad* UAblTurnToTask_Ratio::CreateScratchPad(const TWeakObjectPtr<UAblAbilityContext>& Context) const
{
	if (UAblAbilityUtilitySubsystem* Subsystem = Context->GetUtilitySubsystem())
	{
		static TSubclassOf<UAblAbilityTaskScratchPad> ScratchPadClass = UAblTurnToRatioTaskScratchPad::StaticClass();
		return Subsystem->FindOrConstructTaskScratchPad(ScratchPadClass);
	}

	return NewObject<UAblTurnToRatioTaskScratchPad>(Context.Get());
}

TStatId UAblTurnToTask_Ratio::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UAblTurnToTask, STATGROUP_Able);
}

void UAblTurnToTask_Ratio::BindDynamicDelegates(UAblAbility* Ability)
{
	Super::BindDynamicDelegates(Ability);

	ABL_BIND_DYNAMIC_PROPERTY(Ability, m_UseRotationVector, "Use Rotation Vector");
	ABL_BIND_DYNAMIC_PROPERTY(Ability, m_RotationVector, "Rotation Vector");
}

FRotator UAblTurnToTask_Ratio::GetTargetRotation(const TWeakObjectPtr<const UAblAbilityContext>& Context, const AActor* Source, const AActor* Destination) const
{
	float Yaw = 0.0f;
	float Pitch = 0.0f;

	FVector ToTarget = GetTargetVector(Context, Source, Destination);
	ToTarget.Normalize();

	FVector2D YawPitch = ToTarget.UnitCartesianToSpherical();

	if (m_SetYaw)
	{
		Yaw = FMath::RadiansToDegrees(YawPitch.Y);
	}

	if (m_SetPitch)
	{
		Pitch = FMath::RadiansToDegrees(YawPitch.X);
	}

	FRotator OutRotator(Pitch, Yaw, 0.0f);

	return OutRotator + m_RotationOffset;
}

FVector UAblTurnToTask_Ratio::GetTargetVector(const TWeakObjectPtr<const UAblAbilityContext>& Context, const AActor* Source, const AActor* Destination) const
{
	bool useVector = ABL_GET_DYNAMIC_PROPERTY_VALUE(Context, m_UseRotationVector);

	if (useVector)
	{
		return ABL_GET_DYNAMIC_PROPERTY_VALUE(Context, m_RotationVector);
	}

	if (Source && Destination)
	{
		return Destination->GetActorLocation() - Source->GetActorLocation();
	}

	return FVector::ZeroVector;
}

#if WITH_EDITOR

FText UAblTurnToTask_Ratio::GetDescriptiveTaskName() const
{
	const FText FormatText = LOCTEXT("AblTurnToTaskFormat", "{0}: {1}");
	FString TargetName = FAbleLogHelper::GetTargetTargetEnumAsString(m_RotationTarget);
	return FText::FormatOrdered(FormatText, GetTaskName(), FText::FromString(TargetName));
}

void UAblTurnToTask_Ratio::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	if (GetDuration() < m_Blend.GetBlendTime())
	{
		m_EndTime = GetStartTime() + m_Blend.GetBlendTime();
	}
}

#endif

#undef LOCTEXT_NAMESPACE