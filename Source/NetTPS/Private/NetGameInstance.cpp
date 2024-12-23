// Fill out your copyright notice in the Description page of Project Settings.


#include "NetGameInstance.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "Online/OnlineSessionNames.h"

#include "Interfaces/OnlineSessionInterface.h"
#include "Kismet/GameplayStatics.h"


void UNetGameInstance::Init()
{
	Super::Init();

	//get current subsystem
	IOnlineSubsystem* Subsys =  IOnlineSubsystem::Get();
	if(Subsys)
	{
		SessionInterface = Subsys->GetSessionInterface();

		SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UNetGameInstance::OnCreateSessionComplete);
		SessionInterface->OnDestroySessionCompleteDelegates.AddUObject(this, &UNetGameInstance::OnDestroySessionComplete);
		SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &UNetGameInstance::OnFindSessionComplete);
		SessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this, &UNetGameInstance::OnJoinSessionComplete);
	}
}

void UNetGameInstance::CreateMySession(FString DisplayName, int32 PlayerCount)
{
	FOnlineSessionSettings SessionSettings;

	FName SubsysName = IOnlineSubsystem::Get()->GetSubsystemName();

	//Check to use Lan or not
	SessionSettings.bIsLANMatch = SubsysName.IsEqual(FName(TEXT("NULL")));

	//Check to use Lobby or not
	SessionSettings.bUseLobbiesIfAvailable = true;

	//Check to allow session search
	SessionSettings.bShouldAdvertise = true;

	//Check to allow joining while session is in progress
	SessionSettings.bAllowJoinInProgress = true;

	//Max Session connection count
	SessionSettings.NumPublicConnections = PlayerCount;

	//Custom Info
	SessionSettings.Set(TEXT("DP_NAME"), DisplayName, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

	//Create Session
	FUniqueNetIdPtr NetId = GetWorld()->GetFirstLocalPlayerFromController()->GetUniqueNetIdForPlatformUser().GetUniqueNetId();
	SessionInterface->CreateSession(*NetId, FName(DisplayName), SessionSettings);
}

void UNetGameInstance::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
	if(bWasSuccessful)
	{
		UE_LOG(LogTemp,Warning, TEXT("[%s]Session Creation Success"), *SessionName.ToString());
		//if succeeded, creator moves to session
		CurrentSessionName = SessionName;
		GetWorld()->ServerTravel(TEXT("/Game/Net/Maps/BattleMap?listen"));
	}
	else
	{
		UE_LOG(LogTemp,Warning, TEXT("[%s]Session Creation Failed"), *SessionName.ToString())
	}
}

void UNetGameInstance::DestroyMySession()
{
	SessionInterface->DestroySession(CurrentSessionName);
}

void UNetGameInstance::OnDestroySessionComplete(FName SessionName, bool bWasSuccessful)
{
	if(bWasSuccessful)
	{
		UE_LOG(LogTemp,Warning, TEXT("[%s]Session Destruction Success"), *SessionName.ToString());
		UGameplayStatics::OpenLevel(GetWorld(), TEXT("Lv_Lobby"));
	}
	else
	{
		UE_LOG(LogTemp,Warning, TEXT("[%s]Session Destruction Failed"), *SessionName.ToString())
	}
}

void UNetGameInstance::FindOtherSession()
{
	OnFindComplete.ExecuteIfBound(false);
	SessionSearch = MakeShared<FOnlineSessionSearch>();
	FName SubsysName = IOnlineSubsystem::Get()->GetSubsystemName();
	SessionSearch->bIsLanQuery = SubsysName.IsEqual(FName(TEXT("NULL")));
	SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);
	
	SessionSearch->MaxSearchResults = 100;
	SessionInterface->FindSessions(0, SessionSearch.ToSharedRef());
}

void UNetGameInstance::OnFindSessionComplete(bool bWasSuccessful)
{
	if(bWasSuccessful)
	{
		auto results = SessionSearch->SearchResults;
		for(int32 i = 0; i < results.Num(); i++)
		{
			FOnlineSessionSearchResult SR = results[i];
			FString DisplayName;
			SR.Session.SessionSettings.Get(TEXT("DP_NAME"), DisplayName);
			//
			FString SessionCreator = SR.Session.OwningUserName;
			UE_LOG(LogTemp,Warning, TEXT("Session : %s, Creator : %s"),  *DisplayName, *SessionCreator);

			FString SessionInfo = FString::Printf(TEXT("%s : %s"),  *DisplayName, *SessionCreator);
			OnAddSession.ExecuteIfBound(i, SessionInfo);
		}
	}

	UE_LOG(LogTemp,Warning, TEXT("Session Search Complete"));
	OnFindComplete.ExecuteIfBound(true);
	
}

void UNetGameInstance::JoinOtherSession(int32 index)
{
	auto results = SessionSearch->SearchResults;

	FString DisplayName;
	results[index].Session.SessionSettings.Get(TEXT("DP_NAME"), DisplayName);

	SessionInterface->JoinSession(0, FName(DisplayName), results[index]);
}

void UNetGameInstance::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	if(Result == EOnJoinSessionCompleteResult::Success)
	{
		FString URL;
		CurrentSessionName = SessionName;
		SessionInterface->GetResolvedConnectString(SessionName, URL);
		APlayerController* PC = GetWorld()->GetFirstPlayerController();
		PC->ClientTravel(URL,TRAVEL_Absolute);
	}
}