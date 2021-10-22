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

#include "SpeechComponent.h"
#include "MockPollyClient.h"
#include "TestableSpeechComponent.generated.h"

/**
* Testable version of SpeechComponent that overrides SpeechComponent methods to avoid use of dependencies during unit testing. 
*/
UCLASS(ClassGroup = (Custom))
class UTestableSpeechComponent : public USpeechComponent {

    GENERATED_BODY()

public:
    /*
    * Initializes the MockPollyClient
    */
    virtual void InitializePollyClient() override;
    /*
    * Overrides GenerateSpeechSync to change accessibility to public so it can be invoked
    * from the spec tests. See USpeechComponent::GenerateSpeechSync for details.
    */
    virtual void GenerateSpeechSync(const FString text, const EVoiceId VoiceId) override;
    /**
    * Overrides PlayNextViseme to change accessibility to public so it can be invoked
    * from the spec tests. See USpeechComponent::GenerateSpeechSync for details.
    */
    virtual void PlayNextViseme() override;
    /**
    * Overrides SetTimer to avoid creation of actual timer to avoid use of FTimerManager dependencies and crashes during unit testing 
    */
    virtual void SetTimer(float CurrentVisemeDurationSeconds) override;
    /**
    * Does nothing, no actual timers created during unit testing 
    */
    virtual void ClearTimer() override;
    /**
    * Gets the PollyClient state, downcasting it to a MockPollyClient 
    */
    MockPollyClient* GetPollyClient();
    /**
    * Getter for CurrentViseme
    */
    EViseme GetCurrentViseme();
    /**
    * Getter for VisemeEventArray
    */
    TArray<VisemeEvent> GetVisemeEventArray();
    /**
    * Getter for CurrentVisemeIndex
    */
    int GetCurrentVisemeIndex();
    /**
    * Getter for Audiobuffer
    */
    TArray<uint8> GetAudiobuffer();
    /**
    * Getter for bIsSpeaking 
    */
    bool IsSpeaking();
    /**
    * Setter for bIsSpeaking
    */
    void SetSpeaking(bool boolean);
};