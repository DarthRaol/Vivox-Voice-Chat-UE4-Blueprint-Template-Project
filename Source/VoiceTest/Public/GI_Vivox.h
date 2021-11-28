// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "VivoxCore.h"


#include "GI_Vivox.generated.h"

/**
 * 
 */
DECLARE_DYNAMIC_DELEGATE_OneParam(FResult,FString,ResultOut);

UCLASS()
class VOICETEST_API UGI_Vivox : public UGameInstance
{
	GENERATED_BODY()
	
	UGI_Vivox();
	//Vivox Starts Here
    	
	private:
	FString Issuer = "";
	FString Domain = "";
	FString TokenKey = "";
	FString ChannelTokenKey = "";
	FString SecretKey = "";
	FString Server = "";
	FTimespan ExpirationTime = FTimespan(0,1,0);
	FVivoxCoreModule* vModule;
	IClient* MyVoiceClient;
	FString UserName = "";
	AccountId Account;
	ChannelId Channel;
	TArray<IAudioDevice*> InpAudioDevices;
	TArray<IAudioDevice*> OutpAudioDevices;


    
	public:

	UPROPERTY()
	FResult ResultOut; //For Text Send deligate test
	UPROPERTY(BlueprintReadOnly)
	bool IsLoggedIn;
	UPROPERTY(BlueprintReadOnly)
	bool IsChannelDone;

	
	UFUNCTION(BlueprintCallable, Category= "Custom|Vivox|Vivox")
           void VivoxLogin(const FString Account_Name, const FResult Result);
	
	//Vivox Initialize Function that needs to be called at the start of your game.
	UFUNCTION(BlueprintCallable, Category= "Custom|Vivox|Vivox")
         void VivoxInitialize();
	
	// LogOut From Vivox Server
	UFUNCTION(BlueprintCallable, Category= "Custom|Vivox|Vivox")
         void VivoxLogOut();
	
	// Must Be called When User Quits to remove User from session
	UFUNCTION(BlueprintCallable, Category= "Custom|Vivox|Vivox")
         void VivoxOnGameQuit();

	//Function To Join a Vivox Channel
	UFUNCTION(BlueprintCallable, Category= "Custom|Vivox|Voice")
         void VivoxJoinChannel(const FString Channel_Name, const bool bIsAudio,const bool bIsText, const FResult Result);

	//Function To Leave Current Channel
	UFUNCTION(BlueprintCallable, Category= "Custom|Vivox|Voice")
         void VivoxLeaveChannel() const;

	//Function To set Vivox Credentials (Must be Called before Initialize Function or at the start of game)
	UFUNCTION(BlueprintCallable, Category= "Custom|Vivox|Settings")
         void VivoxSetSettings( FString Issuer_value, FString Domain_value, FString SecretKey_value, FString Server_value,  FTimespan ExpirationTimeValue);

	// Used To Set Channel Transmission Mode (Set to All If channeled is joined)
	UFUNCTION(BlueprintCallable, Category= "Custom|Vivox|Voice")
     void VivoxSetChannelTransmission(const TransmissionMode TransmissionMode) const;

	//Get Current Active Device Name
	UFUNCTION(BlueprintCallable, Category= "Custom|Vivox|Device")
     void VivoxGetCurrentActiveDevice(FString& InputDevice, FString& OutputDevice) const;

	//Get name of all input and output devices
	UFUNCTION(BlueprintCallable, Category= "Custom|Vivox|Device")
     void VivoxGetAllDevice(TArray<FString>& InputDevices, TArray<FString>& OutputDevices);
	
	UFUNCTION(BlueprintCallable, Category= "Custom|Vivox|Device")
	void VivoxSetInputDevice(const FString InputDevice);
	UFUNCTION(BlueprintCallable, Category= "Custom|Vivox|Device")
	void VivoxSetOutputDevice(const FString OutputDevice);

	//Get All Participant in the channel
	UFUNCTION(BlueprintCallable, Category= "Custom|Vivox|Participants")
    void VivoxGetParticipants(TArray<FString>& Participants) const;

	UFUNCTION(BlueprintImplementableEvent, Category="Custom|Vivox|Participants|Events")
	void OnParticipantAdded(const FString& Participant_Name);

	UFUNCTION(BlueprintImplementableEvent, Category="Custom|Vivox|Participants|Events")
	void OnParticipantRemoved(const FString& Participant_Name);

	UFUNCTION(BlueprintImplementableEvent, Category="Custom|Vivox|Participants|Events")
	void OnParticipantUpdated(const FString& Participant_Name);

	UFUNCTION(BlueprintCallable,Category="Custom|Vivox|Text_Chat")
	void VivoxSendTextMessage(const FString Message, const FResult Result);

	UFUNCTION(BlueprintImplementableEvent, Category="Custom|Vivox|Text_Chat")
    void OnTextMessageReceived(const FString& Sender_Name, const FString& Message, const bool bIsSelf);
	
	void OnChannelParticipantAdded(const IParticipant &Participant);
	void OnChannelParticipantRemoved(const IParticipant &Participant);
	void OnChannelParticipantUpdated(const IParticipant &Participant);
	void OnChannelTextMessageReceived(const IChannelTextMessage &Message);


	
};
