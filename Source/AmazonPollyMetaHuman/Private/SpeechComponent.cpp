/*
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: MIT-0
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this
 * software and associated documentation files (the "Software"), to deal in the Software
 * without restriction, including without limitation the rights to use, copy, modify,
 * merge, publish, distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include "SpeechComponent.h"
#include <fstream>
#include "Json.h"
#include "UObject/Class.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"
#include <aws/core/auth/AWSCredentialsProviderChain.h>
#include <aws/core/client/AWSClient.h>
#include "UnrealAWSUtils.h"
#include "GenerateSpeechAction.h"

using UnrealAWSUtils::AwsStringToFString;
using UnrealAWSUtils::FStringToAwsString;

DEFINE_LOG_CATEGORY(LogPollyMsg);

USpeechComponent::USpeechComponent() {
    // Set this component to be initialized when the game starts, and to be ticked every frame. 
    // You can turn these features off to improve performance if you don't need them.
    PrimaryComponentTick.bCanEverTick = false;
    bWantsInitializeComponent = true;
}

void USpeechComponent::InitializeComponent() {
    Super::InitializeComponent();
    InitializePollyClient();
}

void USpeechComponent::GenerateSpeech(
    UObject* WorldContextObject,
    const FString Text,
    const EVoiceId VoiceId,
    struct FLatentActionInfo LatentActionInfo,
    EGenerateSpeechExecPins& GenerateSpeechExecPins
) {
    if (UWorld* World = GEngine->GetWorldFromContextObjectChecked(WorldContextObject)) {
        FLatentActionManager& LatentActionManager = World->GetLatentActionManager();
        UObject* CallbackTarget = LatentActionInfo.CallbackTarget;
        int32 UUID = LatentActionInfo.UUID;
        if (LatentActionManager.FindExistingAction<FPendingLatentAction>(CallbackTarget, UUID) == NULL) {
            // LatentActionManager takes ownership of FGenerateSpeechAction (it calls delete).
            LatentActionManager.AddNewAction(CallbackTarget, UUID, new FGenerateSpeechAction(LatentActionInfo, this, Text, VoiceId, GenerateSpeechExecPins));
        }
    }
}

USoundWaveProcedural* USpeechComponent::StartSpeech() {
    FScopeLock lock(&Mutex);
    if (VisemeEventArray.Num() == 0) {
        UE_LOG(LogPollyMsg, Error, TEXT("Failed to start speech. GenerateSpeech must be invoked before StartSpeech."));
        return nullptr;
    }
    else {
        bIsSpeaking = true;
        CurrentVisemeIndex = 0;
        CurrentViseme = VisemeEventArray[CurrentVisemeIndex].Viseme;
        StartTimePoint = std::chrono::steady_clock::now();
        float CurrentVisemeDurationSeconds = VisemeEventArray[CurrentVisemeIndex].TimeMilliseconds / 1000.0f;
        SetTimer(CurrentVisemeDurationSeconds);
        return QueuePollyAudio();
    }
}

EViseme USpeechComponent::GetCurrentViseme() {
    FScopeLock lock(&Mutex);
    return CurrentViseme;
}

bool USpeechComponent::IsSpeaking() {
    FScopeLock lock(&Mutex);
    return bIsSpeaking;
}

void USpeechComponent::PlayNextViseme() {
    FScopeLock lock(&Mutex);
    CurrentVisemeIndex++;
    ClearTimer();
    if (CurrentVisemeIndex == VisemeEventArray.Num() || VisemeEventArray.Num() == 0) {
        bIsSpeaking = false;
        return;
    }
    else {
        CurrentViseme = VisemeEventArray[CurrentVisemeIndex].Viseme;
        auto CurrentTimePoint = std::chrono::steady_clock::now();
        float SecondsSinceStart = std::chrono::duration<float, std::milli>(CurrentTimePoint - StartTimePoint).count() / 1000.0f;
        float CurrentVisemeDurationSeconds = fmaxf(VisemeEventArray[CurrentVisemeIndex].TimeMilliseconds / 1000.0f - SecondsSinceStart, 0);
        SetTimer(CurrentVisemeDurationSeconds);
    }
}

void USpeechComponent::GenerateSpeechSync(const FString Text, const EVoiceId VoiceId) {
    if (Text.IsEmpty()) {
        UE_LOG(LogPollyMsg, Error, TEXT("Cannot generate speech (check input text)."));
        return;
    }
    if (IsSpeaking()) {
        UE_LOG(LogPollyMsg, Error, TEXT("Cannot generate speech during playback."));
        return;
    }
    if (SynthesizeAudio(Text, VoiceId) && SynthesizeVisemes(Text, VoiceId)) {
        UE_LOG(LogPollyMsg, Display, TEXT("Polly called successfully!"));
    }
}

bool USpeechComponent::SynthesizeAudio(const FString& Text, const EVoiceId VoiceId) {
    PollyOutcome PollyAudioOutcome = MyPollyClient->SynthesizeSpeech(CreatePollyAudioRequest(Text, VoiceId));
    if (PollyAudioOutcome.IsSuccess) {
        FScopeLock lock(&Mutex);
        Audiobuffer = PollyAudioOutcome.StreamBuffer;
    }
    else {
        UE_LOG(LogPollyMsg, Error, TEXT("Polly failed to generate audio file. Error: %s"), *AwsStringToFString(PollyAudioOutcome.PollyErrorMsg));
    }
    return PollyAudioOutcome.IsSuccess;
}

bool USpeechComponent::SynthesizeVisemes(const FString& Text, const EVoiceId VoiceId) {
    PollyOutcome PollyVisemeOutcome = MyPollyClient->SynthesizeSpeech(CreatePollyVisemeRequest(Text, VoiceId));
    if (PollyVisemeOutcome.IsSuccess) {
        FScopeLock lock(&Mutex);
        FString VisemeJson;
        FFileHelper::BufferToString(VisemeJson, PollyVisemeOutcome.StreamBuffer.GetData(), PollyVisemeOutcome.StreamBuffer.Num());
        GenerateVisemeEvents(VisemeJson);
    }
    else {
        UE_LOG(LogPollyMsg, Error, TEXT("Polly failed to generate visemes. Error: %s"), *AwsStringToFString(PollyVisemeOutcome.PollyErrorMsg));
    }
    return PollyVisemeOutcome.IsSuccess;
}

USoundWaveProcedural* USpeechComponent::QueuePollyAudio() {
    USoundWaveProcedural* PollyAudio = NewObject<USoundWaveProcedural>();
    PollyAudio->SetSampleRate(16000);
    PollyAudio->NumChannels = 1;
    PollyAudio->DecompressionType = DTYPE_Procedural;
    int32 BitRate = 16 * PollyAudio->NumChannels * PollyAudio->GetSampleRateForCurrentPlatform();
    PollyAudio->Duration = Audiobuffer.Num() * 8.0f / BitRate;
    PollyAudio->QueueAudio(Audiobuffer.GetData(), Audiobuffer.Num());
    return PollyAudio;
}

Aws::Polly::Model::SynthesizeSpeechRequest USpeechComponent::CreatePollyAudioRequest(const FString& Text, const EVoiceId VoiceId) const {
    Aws::Polly::Model::SynthesizeSpeechRequest PollyRequest;
    PollyRequest.SetText(FStringToAwsString(Text));
    PollyRequest.SetVoiceId(ToPollyVoiceId(VoiceId));
    PollyRequest.SetEngine(ToPollyVoiceEngine(VoiceId));
    PollyRequest.SetOutputFormat(Aws::Polly::Model::OutputFormat::pcm); 
    return PollyRequest;
}

Aws::Polly::Model::SynthesizeSpeechRequest USpeechComponent::CreatePollyVisemeRequest(const FString& Text, const EVoiceId VoiceId) const {
    Aws::Polly::Model::SynthesizeSpeechRequest PollyRequest;
    PollyRequest.SetText(FStringToAwsString(Text));
    PollyRequest.SetVoiceId(ToPollyVoiceId(VoiceId));
    PollyRequest.SetEngine(ToPollyVoiceEngine(VoiceId));
    PollyRequest.SetOutputFormat(Aws::Polly::Model::OutputFormat::json);
    PollyRequest.AddSpeechMarkTypes(Aws::Polly::Model::SpeechMarkType::viseme);
    return PollyRequest;
}

void USpeechComponent::GenerateVisemeEvents(FString VisemeJson) {
    VisemeEventArray = {};
    TArray<FString> VisemeStrings;
    VisemeJson.ParseIntoArray(VisemeStrings, TEXT("\n"), true);
    for (FString VisemeSet : VisemeStrings) {
        TSharedPtr<FJsonObject> JsonParsed = MakeShareable(new FJsonObject);
        TSharedRef<TJsonReader<TCHAR>> JsonReader = TJsonReaderFactory<TCHAR>::Create(VisemeSet);
        FString OutString;
        double OutNumber;
        if (FJsonSerializer::Deserialize(JsonReader, JsonParsed) && JsonParsed->TryGetStringField("value", OutString) && JsonParsed->TryGetNumberField("time", OutNumber)) {
            VisemeEvent CurrentVisemeEvent;
            CurrentVisemeEvent.Viseme = GetVisemeValueFromString(JsonParsed->GetStringField("value"));
            CurrentVisemeEvent.TimeMilliseconds = JsonParsed->GetIntegerField("time");
            VisemeEventArray.Add(CurrentVisemeEvent);
        }
        else {
            UE_LOG(LogPollyMsg, Error, TEXT("Failed to parse json formatted viseme sequence returned by Amazon Polly."));
            VisemeEventArray = {};
            Audiobuffer.Empty();
            break;
        }
    }
}

void USpeechComponent::InitializePollyClient() {
    MyPollyClient = MakeUnique<PollyClient>();
}

void USpeechComponent::SetTimer(float CurrentVisemeDurationSeconds) {
    GetWorld()->GetTimerManager().SetTimer(CountdownTimerHandle, this, &USpeechComponent::PlayNextViseme, 1.0f, true, CurrentVisemeDurationSeconds);
}

void USpeechComponent::ClearTimer() {
    GetWorld()->GetTimerManager().ClearTimer(CountdownTimerHandle);
}