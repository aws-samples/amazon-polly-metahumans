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

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Sound/SoundWaveProcedural.h"
#include <aws/core/Aws.h>
#include "PollyClient.h"
#include <chrono>
#include "Runtime/Engine/Public/LatentActions.h"
#include "Viseme.h"
#include "VoiceId.h"
#include "SpeechComponent.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogPollyMsg, Log, All);

/**
* Represents the multiple output pins that can be invoked
* on completion of the latent GenerateSpeech function.
*/
UENUM(BlueprintType)
enum class EGenerateSpeechExecPins : uint8 {
    Success UMETA(DisplayName = "Success"),
    Failure UMETA(DisplayName = "Failure")
};

/**
* Struct containing a single viseme and its corresponding timestamp returned by Polly
* to be used in setting CurrentViseme and CurrentVisemeDurationSeconds states 
*/
struct VisemeEvent {
    EViseme Viseme;
    int TimeMilliseconds;
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class AMAZONPOLLYMETAHUMAN_API USpeechComponent : public UActorComponent
{
    GENERATED_BODY()

public:
   /**
    * Default constructor.
    */
    USpeechComponent();
    /**
	 * Initializes the component. See UActorComponent::InitializeComponent for details.
	 */
    virtual void InitializeComponent() override;
    /**
    * Blueprint function for calling Polly API to generate Viseme/Audio data.
    * One of the GenerateSpeech* functions must be called before StartSpeech() function.
    * @param WorldContextObject description
    * @param Text - the text to be synthesized by Polly (the maximum length of input text can be up to 3000 characters)
    * @param VoiceId - enum for VoiceId for use in calling Polly
    * @param LatentInfo description
    * @param parameter-name description
    */
    UFUNCTION(
        BlueprintCallable,
        Category = "Amazon Polly", 
        Meta = (
            Latent,
            LatentInfo = "LatentInfo",
            HidePin = "WorldContextObject",
            DefaultToSelf = "WorldContextObject",
            ExpandEnumAsExecs = "EGenerateSpeechExecPins"
        )
    )
    void GenerateSpeech(
        UObject* WorldContextObject,
        const FString Text,
        const EVoiceId VoiceId,
        struct FLatentActionInfo LatentInfo,
        EGenerateSpeechExecPins& EGenerateSpeechExecPins
    );
    /**
    * Starts the Animation playback and returns an Audio object to be played in Blueprints.
    * GenerateSpeech function must be called beforehand.
    * @return A USoundWaveProcedural object containing the audio synthesized from Polly
    */
    UFUNCTION(BlueprintCallable, Category = "Amazon Polly")
    USoundWaveProcedural* StartSpeech();
    /**
    * Returns the current viseme corresponding to the last invocation of StartSpeech. Assuming
    * the sound returned by StartSpeech began playback immediately following its creation, the
    * current viseme returned by this function should be synchronized to the current sound
    * being played.
    * 
    * @return the current viseme
    */
    UFUNCTION(BlueprintPure, Category = "Amazon Polly")
    EViseme GetCurrentViseme();
    /**
    * Returns true if the speech component is still playing a viseme sequence
    * 
    * @return true if the speech component is still playing a viseme sequence
    */
    UFUNCTION(BlueprintPure, Category = "Amazon Polly")
    bool IsSpeaking();

protected:
    /**
    * Blueprint function for calling Polly API to generate Viseme/Audio data.
    * One of the GenerateSpeech* functions must be called before StartSpeech() function.
    * @param text - the text to be synthesized by Polly (the maximum length of input text can be up to 3000 characters)
    * @param VoiceId - enum for VoiceId for use in calling Polly
    */
    virtual void GenerateSpeechSync(const FString text, const EVoiceId VoiceId);
    /**
    * Each time this method is called it plays back the next viseme in the speech.
    */
    virtual void PlayNextViseme();
    /**
    * Enum representing a single viseme state, to be used in setting a Viseme variable in Blueprints
    */
    EViseme CurrentViseme;
    /**
    * Enum representing a VoiceId to set a voice when calling Polly
    */
    EVoiceId Voice_ID;
    /**
    * Boolean flag to keep GenerateSpeech from being invocated while the Actor is speaking
    */
    bool bIsSpeaking = false;
    /**
    * Index variable used to iterate through VisemeDataArray in UpdateVisemeAndDurationStates function
    */
    int CurrentVisemeIndex;
    /**
    * Buffer containing audio data from Polly stream
    */
    TArray<uint8> Audiobuffer;
    /**
    * PollyClient state for calling Polly SDK 
    */
    TUniquePtr<PollyClient> MyPollyClient;
    /**
    * Array containing custom data structure that holds viseme and time data for each Viseme from Polly
    */
    TArray<VisemeEvent> VisemeEventArray;

private:
    /**
    * Calls the PollyClient to generate Polly Audio data 
    * @param text - the text synthesized by Polly
    * @param VoiceId - the voice of the synthesized audio 
    * @return bool - boolean indicating success/failure of Polly call
    */
    bool SynthesizeAudio(const FString& text, const EVoiceId VoiceId);
    /**
    * Calls the PollyClient to generate Polly Viseme data
    * @param text - the text synthesized by Polly
    * @param VoiceId - the voice of the synthesized data 
    * @return bool - boolean indicating success/failure of Polly call
    */
    bool SynthesizeVisemes(const FString& text, const EVoiceId VoiceId);
    /**
    * Returns a PollyRequest that is configured to return pcm audio data with a given text and VoiceId 
    * @param text - the text to be synthesized (SetText)
    * @param VoiceId - the VoiceId for the synthesized audio 
    * @return PollyRequest - the configured PollyRequest
    */
    Aws::Polly::Model::SynthesizeSpeechRequest CreatePollyAudioRequest(const FString& text, const EVoiceId VoiceId) const;
    /**
    * Returns a PollyRequest that is configured to return viseme and timestamp data in a json format
    * @param text - the text to be synthesized (SetText)
    * @param VoiceId - the VoiceId for the synthesized audio 
    * @return PollyRequest - the configured PollyRequest
    */
    Aws::Polly::Model::SynthesizeSpeechRequest CreatePollyVisemeRequest(const FString& text, const EVoiceId VoiceId) const;
    /**
    * Fills the VisemeEventArray with VisemeEvent objects containing visemes and corresponding timestamps
    * @param VisemeJson - FString containing Polly json viseme data ( example: {"time":125,"type":"viseme","value":"k"} )
    */
    void GenerateVisemeEvents(FString VisemeJson);
    /**
    * Returns a USoundWave object containing the Polly Audio for playback in Blueprints
    * @return USoundWaveProcedural - Sound wave object containing Polly Audio 
    */
    USoundWaveProcedural* QueuePollyAudio();
    /**
    * Timer handle for use in creating/clearing timers 
    */
    FTimerHandle CountdownTimerHandle;
    /*
    * Clock representing the start time of the StartSpeech call
    */
    std::chrono::steady_clock::time_point StartTimePoint;
    /*
    * Sets a timer with a delay for next call to PlayNextViseme
    * @param CurrentVisemeDurationSeconds - the delay
    */
    virtual void SetTimer(float CurrentVisemeDurationSeconds);
    /*
    * Clears the timer 
    */
    virtual void ClearTimer();
    /*
    * Initializes the UnrealPollyClient  
    */
    virtual void InitializePollyClient();
    /*
    * Mutex for thread-safe mutation of internal state.
    */
    FCriticalSection Mutex;
    // FGenerateSpeechAction is a friend class so that it can invoke the
    // protected GenerateSpeechSync function in a separate thread, which
    // is required to make GenerateSpeech a non-blocking latent function.
    friend class FGenerateSpeechAction;
};
