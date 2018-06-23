// Fill out your copyright notice in the Description page of Project Settings.

#include "ChooseNextWaypoint.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "PatrolRouteComponent.h"
#include "AIController.h"



EBTNodeResult::Type UChooseNextWaypoint::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	int32 Index = BlackboardComp->GetValueAsInt(IndexKey.SelectedKeyName);

	APawn* ControlledPawn = OwnerComp.GetAIOwner()->GetPawn();
	UPatrolRouteComponent* PatrolRouteComponent = ControlledPawn->FindComponentByClass<UPatrolRouteComponent>();
	if (!ensure(PatrolRouteComponent)) { return EBTNodeResult::Failed; }

	//Get PatrolPoints 
	TArray<AActor*> PatrolPoints = PatrolRouteComponent->GetPatrolPoints();

	if (PatrolPoints.Num() == 0) { UE_LOG(LogTemp, Warning, TEXT("Guard is missing Patrol Points!")); return EBTNodeResult::Failed; }

	//Set Waypoint Object to the specified Index
	BlackboardComp->SetValueAsObject(Waypoint.SelectedKeyName, PatrolPoints[Index]);

	//Cycle Through Index and Max Lenght Array
	uint32 NewIndex = (Index + 1) % (PatrolPoints.Num());
	BlackboardComp->SetValueAsInt(IndexKey.SelectedKeyName, NewIndex);

	//UE_LOG(LogTemp, Warning, TEXT("Selected Key Index: %i"), Index);
	return EBTNodeResult::Succeeded;
}


