// Fill out your copyright notice in the Description page of Project Settings.


#include "Tasks/AblNiagaraTrailTask.h"
#include "ablAbility.h"
#include "NiagaraComponent.h"
#include "AbleCore/Private/AbleCorePrivate.h"
#include "NiagaraFunctionLibrary.h"
#include "GameFramework/Character.h"
/* Create our scratchpad */
UAblNiagaraTrailTaskScratchPad::UAblNiagaraTrailTaskScratchPad()
{
}
UAblNiagaraTrailTaskScratchPad::~UAblNiagaraTrailTaskScratchPad()
{
}

/* Construct and deconstruct task and set default parameters */
UAblNiagaraTrailTask::UAblNiagaraTrailTask(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer),
m_TaskRealm(ATR_ClientAndServer)
{
}
UAblNiagaraTrailTask::~UAblNiagaraTrailTask()
{
}

/* MAIN FUNCTIONS */
void UAblNiagaraTrailTask::OnTaskStart(const TWeakObjectPtr<const UAblAbilityContext>& Context) const
{
	Super::OnTaskStart(Context);
	// getting our variables
	UNiagaraSystem* system = ABL_GET_DYNAMIC_PROPERTY_VALUE(Context,m_System);
	UAblNiagaraTrailTaskScratchPad* ScratchPad = CastChecked<UAblNiagaraTrailTaskScratchPad>(Context->GetScratchPadForTask(this));
	FName attachpoint = "root";
	FName start = "startLocation";
	FName end = "EndLocation";
	if(system != nullptr && Context->GetOwner() != nullptr)
	{
		// running the spawn system function and saving it to the scratchpad
		ScratchPad->component = UNiagaraFunctionLibrary::SpawnSystemAttached(system,Context->GetOwner()->GetRootComponent(),attachpoint,FVector::ZeroVector,FRotator::ZeroRotator,EAttachLocation::KeepRelativeOffset,true);

		if(m_UseCharacterMesh)
		{
			if(Cast<ACharacter>(Context->GetOwner())->GetMesh() != nullptr)
			{
				ScratchPad->skeleton = Cast<ACharacter>(Context->GetOwner())->GetMesh();
				ScratchPad->component->SetVectorParameter(start,ScratchPad->skeleton->GetSocketLocation(m_StartSocket));
				ScratchPad->component->SetVectorParameter(end,ScratchPad->skeleton->GetSocketLocation(m_EndSocket));
			}
		}
		else
		{
			//set our locations straight after creation, else the ribbon stretches from a 0,0,0 start location
			ScratchPad->component->SetVectorParameter(start,ABL_GET_DYNAMIC_PROPERTY_VALUE(Context,m_StartVector));
			ScratchPad->component->SetVectorParameter(end,ABL_GET_DYNAMIC_PROPERTY_VALUE(Context,m_EndVector));
		}
	}
}

void UAblNiagaraTrailTask::OnTaskTick(const TWeakObjectPtr<const UAblAbilityContext>& Context, float deltaTime) const
{
	Super::OnTaskTick(Context, deltaTime);
	//get scratchpad
	UAblNiagaraTrailTaskScratchPad* ScratchPad = CastChecked<UAblNiagaraTrailTaskScratchPad>(Context->GetScratchPadForTask(this));
	
	// update our locations 
	FName start = "startLocation";
	FName end = "EndLocation";
	if(ScratchPad->component != nullptr)
	{
		if(m_UseCharacterMesh == true )
		{
			if(ScratchPad->skeleton != nullptr)
			{
				ScratchPad->component->SetVectorParameter(start,ScratchPad->skeleton->GetSocketLocation(m_StartSocket));
				ScratchPad->component->SetVectorParameter(end,ScratchPad->skeleton->GetSocketLocation(m_EndSocket));
			}
		}
		else
		{
			//set our locations straight after creation, else the ribbon stretches from a 0,0,0 start location
			ScratchPad->component->SetVectorParameter(start,ABL_GET_DYNAMIC_PROPERTY_VALUE(Context,m_StartVector));
			ScratchPad->component->SetVectorParameter(end,ABL_GET_DYNAMIC_PROPERTY_VALUE(Context,m_EndVector));
		}
	}
}

void UAblNiagaraTrailTask::OnTaskEnd(const TWeakObjectPtr<const UAblAbilityContext>& Context,const EAblAbilityTaskResult Result) const
{
	Super::OnTaskEnd(Context, Result);
	//get our scratchpad
	UAblNiagaraTrailTaskScratchPad* ScratchPad = CastChecked<UAblNiagaraTrailTaskScratchPad>(Context->GetScratchPadForTask(this));
	//destroy component
	if(ScratchPad->component != nullptr){
		ScratchPad->component->DestroyComponent();
	}
}


/* Create our Scratchpad */
UAblAbilityTaskScratchPad* UAblNiagaraTrailTask::CreateScratchPad(const TWeakObjectPtr<UAblAbilityContext>& Context) const
{
	return NewObject<UAblNiagaraTrailTaskScratchPad>(Context.Get());
}

/* declare an ID, needs a unique ID, usually the name of your task,with the default STATGROUP_Able */
TStatId UAblNiagaraTrailTask::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UAblNiagaraTrailTask, STATGROUP_Able);
}

/* function that allows us to set parameters and dynamic.  */
void UAblNiagaraTrailTask::BindDynamicDelegates(UAblAbility* Ability)
{
	Super::BindDynamicDelegates(Ability);
	
	ABL_BIND_DYNAMIC_PROPERTY(Ability, m_StartVector, TEXT("StartVector"));
	ABL_BIND_DYNAMIC_PROPERTY(Ability, m_EndVector, TEXT("EndVector"));	
}

