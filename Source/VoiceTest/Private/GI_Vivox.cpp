// Fill out your copyright notice in the Description page of Project Settings.


#include "GI_Vivox.h"
#include "VivoxCore.h"



UGI_Vivox::UGI_Vivox()
{
	vModule = static_cast<FVivoxCoreModule *>(&FModuleManager::Get().LoadModuleChecked(TEXT("VivoxCore")));
}

void UGI_Vivox::VivoxSetSettings( FString Issuer_value,  FString Domain_value,  FString SecretKey_value,  FString Server_value,  FTimespan ExpirationTimeValue)
{
	Issuer = Issuer_value;
	Domain = Domain_value;
	SecretKey = SecretKey_value;
	Server = Server_value;
	ExpirationTime = ExpirationTimeValue;
}

void UGI_Vivox::VivoxInitialize()
{

	if(vModule)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, "Vivox Module Loaded");
		MyVoiceClient = &static_cast<FVivoxCoreModule *>(&FModuleManager::Get().LoadModuleChecked(TEXT("VivoxCore")))->VoiceClient();
		MyVoiceClient->Initialize();

	}
}

void UGI_Vivox::VivoxLogin(const FString Account_Name, const FResult Result)
{
	GEngine->AddOnScreenDebugMessage(-1,3.0f,FColor::Emerald,"Login Started");
	Account = AccountId(Issuer, Account_Name, Domain);
	UserName = Account_Name;
	ILoginSession &MyLoginSession = MyVoiceClient->GetLoginSession(Account);
	ResultOut = Result;
	bool Success = false;
	TokenKey = MyLoginSession.GetLoginToken(SecretKey,ExpirationTime);
	// Setup the delegate to execute when login completes
	GEngine->AddOnScreenDebugMessage(-1,6.0f,FColor::Emerald,"Token Reached");
	ILoginSession::FOnBeginLoginCompletedDelegate OnBeginLoginCompleted;
	OnBeginLoginCompleted.BindLambda([this, &Success](VivoxCoreError Error)
    {
		ResultOut.ExecuteIfBound(FVivoxCoreModule::ErrorToString(Error));
        if (VX_E_SUCCESS == Error)
        {
            Success = true;
        	GEngine->AddOnScreenDebugMessage(-1,6.0f,FColor::Emerald,"LoggedIN");
            // This bool is only illustrative. The user is now logged in.
        }
    });
	// Request the user to login to Vivox
	MyLoginSession.BeginLogin(Server, TokenKey, OnBeginLoginCompleted);
	
}

void UGI_Vivox::VivoxLogOut()
{
		MyVoiceClient->GetLoginSession(Account).Logout();
}

void UGI_Vivox::VivoxOnGameQuit()
{
		MyVoiceClient->Uninitialize();
}

void UGI_Vivox::VivoxJoinChannel(const FString Channel_Name, const bool bIsAudio, const bool bIsText, const FResult Result)
{
	ResultOut = Result;
	Channel = ChannelId(Issuer,Channel_Name,Domain,ChannelType::NonPositional);
	IChannelSession &ChannelSession = MyVoiceClient->GetLoginSession(Account).GetChannelSession(Channel);
	ChannelTokenKey = ChannelSession.GetConnectToken(SecretKey,ExpirationTime);
	GEngine->AddOnScreenDebugMessage(-1,2.0f,FColor::Emerald,"Channel Join Start");
	bool Success = false;
	IChannelSession::FOnBeginConnectCompletedDelegate OnChannelJoinCompleted;
	ChannelSession.EventTextMessageReceived.AddUObject(this,&UGI_Vivox::OnChannelTextMessageReceived);
	ChannelSession.EventAfterParticipantAdded.AddUObject(this,&UGI_Vivox::OnChannelParticipantAdded);
	ChannelSession.EventAfterParticipantUpdated.AddUObject(this,&UGI_Vivox::OnChannelParticipantUpdated);
	ChannelSession.EventBeforeParticipantRemoved.AddUObject(this,&UGI_Vivox::OnChannelParticipantRemoved);
	OnChannelJoinCompleted.BindLambda([this,&Success](VivoxCoreError Error)
    {
		ResultOut.ExecuteIfBound(FVivoxCoreModule::ErrorToString(Error));
        if (VX_E_SUCCESS == Error)
        {
        	Success = true;
        	//GEngine->AddOnScreenDebugMessage(-1,6.0f,FColor::Emerald,"Chanel Joined");
        	MyVoiceClient->GetLoginSession(Account).SetTransmissionMode(TransmissionMode::All,Channel);
        	
            // This bool is only illustrative. The user is now logged in.
        }
    });
	// Request the use
	ChannelSession.BeginConnect(bIsAudio,bIsText,true,ChannelTokenKey,OnChannelJoinCompleted);
}

void UGI_Vivox::VivoxLeaveChannel() const
{
	// ChannelIdToLeave is the ChannelId of the channel that is being disconnected from
	MyVoiceClient->GetLoginSession(Account).GetChannelSession(Channel).Disconnect();
	// Optionally remove disconnected channel session from list of user’s channel sessions
	MyVoiceClient->GetLoginSession(Account).DeleteChannelSession(Channel);
}

void UGI_Vivox::VivoxSetChannelTransmission(const TransmissionMode TransmissionMode) const
{
	MyVoiceClient->GetLoginSession(Account).SetTransmissionMode(TransmissionMode,Channel);
}

void UGI_Vivox::VivoxGetCurrentActiveDevice(FString& InputDevice, FString& OutputDevice) const
{
	InputDevice = MyVoiceClient->AudioInputDevices().ActiveDevice().Name();
	OutputDevice = MyVoiceClient->AudioOutputDevices().ActiveDevice().Name();
}

void UGI_Vivox::VivoxGetAllDevice(TArray<FString>& InputDevices, TArray<FString>& OutputDevices)
{
	if(InputDevices.Num() == 0||OutputDevices.Num() == 0)
	{
		MyVoiceClient->AudioInputDevices().AvailableDevices().GenerateValueArray(InpAudioDevices);
		MyVoiceClient->AudioOutputDevices().AvailableDevices().GenerateValueArray(OutpAudioDevices);
		for(int i = 0; i < InpAudioDevices.Num(); i++ )
		{
			InputDevices.Add(InpAudioDevices[i]->Name());
		}
		for(int i = 0; i < OutpAudioDevices.Num(); i++ )
		{
			OutputDevices.Add(OutpAudioDevices[i]->Name());
		}
	}
}

void UGI_Vivox::VivoxSetInputDevice(const FString InputDevice)
{
	if(InpAudioDevices.Num() > 0)
	{
		for(int i = 0; i < InpAudioDevices.Num(); i++ )
		{
			if(InpAudioDevices[i]->Name() == InputDevice)
			{
				MyVoiceClient->AudioInputDevices().SetActiveDevice(*InpAudioDevices[i]);
			}

		}
	}
	else
	{
		UE_LOG(LogTemp,Warning,TEXT("USE VivoxGetAllDevice to get device details"));
	}
}

void UGI_Vivox::VivoxSetOutputDevice(const FString OutputDevice)
{
	if(OutpAudioDevices.Num() > 0)
	{
		for(int i = 0; i < OutpAudioDevices.Num(); i++ )
		{
			if(OutpAudioDevices[i]->Name() == OutputDevice)
			{
				MyVoiceClient->AudioInputDevices().SetActiveDevice(*OutpAudioDevices[i]);
			}

		}
	}
	else
	{
		UE_LOG(LogTemp,Warning,TEXT("USE VivoxGetAllDevice to get device details"));
	}
}

void UGI_Vivox::VivoxGetParticipants(TArray<FString>& Participants) const
{
	IChannelSession &ChannelSession = MyVoiceClient->GetLoginSession(Account).GetChannelSession(Channel);
	ChannelSession.Participants().GetKeys(Participants);
}

void UGI_Vivox::OnChannelParticipantAdded(const IParticipant &Participant)
{
	const ChannelId LogChannel = Participant.ParentChannelSession().Channel();
	UE_LOG(LogTemp, Log, TEXT("%s has been added to %s\n"), *Participant.Account().Name(), *LogChannel.Name());
	FString ParticipantName = Participant.Account().DisplayName();
	OnParticipantAdded(ParticipantName);
}

void UGI_Vivox::OnChannelParticipantRemoved(const IParticipant &Participant)
{
	const ChannelId LogChannel = Participant.ParentChannelSession().Channel();
	UE_LOG(LogTemp, Log, TEXT("%s has been removed from %s\n"), *Participant.Account().Name(), *LogChannel.Name());
	FString ParticipantName = Participant.Account().DisplayName();
	OnParticipantRemoved(ParticipantName);
}

void UGI_Vivox::OnChannelParticipantUpdated(const IParticipant &Participant)
{
	const ChannelId LogChannel = Participant.ParentChannelSession().Channel();
	UE_LOG(LogTemp, Log, TEXT("%s has been updated in %s\n"), *Participant.Account().Name(), *LogChannel.Name());
	FString ParticipantName = Participant.Account().DisplayName();
	OnParticipantUpdated(ParticipantName);
}

void UGI_Vivox::VivoxSendTextMessage(const FString Message, const FResult Result)
{
	ResultOut = Result;
	ChannelId Channel1 = Channel;
	IChannelSession::FOnBeginSendTextCompletedDelegate SendChannelMessageCallback;

	SendChannelMessageCallback.BindLambda([this, Channel1, Message](VivoxCoreError Error)
    {
		
		ResultOut.ExecuteIfBound(FVivoxCoreModule::ErrorToString(Error));
        if(VxErrorSuccess == Error)
        {
            UE_LOG(LogTemp, Log, TEXT("Message sent to %s: %s\n"), *Channel1.Name(), *Message);
        }
    });
	MyVoiceClient->GetLoginSession(Account).GetChannelSession(Channel).BeginSendText(Message, SendChannelMessageCallback);
}

void UGI_Vivox::OnChannelTextMessageReceived(const IChannelTextMessage& Message)
{
	const bool SelfCheck = Message.Sender().Name()==UserName;
	OnTextMessageReceived(Message.Sender().Name(),Message.Message(),SelfCheck);


}

