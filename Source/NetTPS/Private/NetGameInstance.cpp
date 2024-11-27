// Fill out your copyright notice in the Description page of Project Settings.


#include "NetGameInstance.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "Online/OnlineSessionNames.h"

#include "Interfaces/OnlineSessionInterface.h"


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
	}
	else
	{
		UE_LOG(LogTemp,Warning, TEXT("[%s]Session Creation Failed"), *SessionName.ToString())
	}
}

void UNetGameInstance::DestroyMySession(FString SessionName)
{
	SessionInterface->DestroySession(FName(SessionName));
}

void UNetGameInstance::OnDestroySessionComplete(FName SessionName, bool bWasSuccessful)
{
	if(bWasSuccessful)
	{
		UE_LOG(LogTemp,Warning, TEXT("[%s]Session Destruction Success"), *SessionName.ToString());
	}
	else
	{
		UE_LOG(LogTemp,Warning, TEXT("[%s]Session Destruction Failed"), *SessionName.ToString())
	}
}

void UNetGameInstance::FindOtherSession()
{
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
		for(int i = 0; i < results.Num(); i++)
		{
			FOnlineSessionSearchResult SR = results[i];
			FString DisplayName;
			SR.Session.SessionSettings.Get(TEXT("DP_NAME"), DisplayName);
			//
			FString SessionCreator = SR.Session.OwningUserName;
			UE_LOG(LogTemp,Warning, TEXT("Session : %s, Creator : %s"),  *DisplayName, *SessionCreator);
		}
	}
	else
	{
		UE_LOG(LogTemp,Warning, TEXT("Session Search Complete"))
	}
}