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

#include <aws/polly/PollyClient.h>

DECLARE_LOG_CATEGORY_EXTERN(LogAmazonPollyVoiceId, Log, All);

/**
* Enum representing a VoiceId to set a voice when calling Polly 
* to allow selection of a VoiceId within Blueprints
* when calling GenerateSpeech.
* Visit https://docs.aws.amazon.com/polly/latest/dg/voicelist.html for more voice options.
*/
UENUM(BlueprintType)
enum class EVoiceId : uint8
{
    Nicole UMETA(DisplayName = "Nicole (Standard, Female, English, Australian)"),
    Russell UMETA(DisplayName = "Russell (Standard, Male, English, Australian)"), 
    Amy UMETA(DisplayName = "Amy (Standard, Female, English, British)"),
    AmyNeural UMETA(DisplayName = "Amy (Neural, Female, English, British)"),
    Emma UMETA(DisplayName = "Emma (Standard, Female, English, British)"),
    EmmaNeural UMETA(DisplayName = "Emma (Neural, Female, English, British)"),
    Brian UMETA(DisplayName = "Brian (Standard, Male, English, British)"),
    BrianNeural UMETA(DisplayName = "Brian (Neural, Male, English, British)"),
    Aditi UMETA(DisplayName = "Aditi (Standard, Female, English, Indian)"),
    Raveena UMETA(DisplayName = "Raveena (Standard, Female, English, Indian)"),
    Ivy UMETA(DisplayName = "Ivy (Standard, Female Child, English, US)"),
    IvyNeural UMETA(DisplayName = "Ivy (Neural, Female Child, English, US)"),
    Joanna UMETA(DisplayName = "Joanna (Standard, Female, English, US)"),
    JoannaNeural UMETA(DisplayName = "Joanna (Neural, Female, English, US)"),
    Kendra UMETA(DisplayName = "Kendra (Standard, Female, English, US)"),
    KendraNeural UMETA(DisplayName = "Kendra (Neural, Female, English, US)"),
    Kimberly UMETA(DisplayName = "Kimberly (Standard, Female, English, US)"),
    KimberlyNeural UMETA(DisplayName = "Kimberly (Neural, Female, English, US)"),
    Salli UMETA(DisplayName = "Salli (Standard, Female, English, US)"),
    SalliNeural UMETA(DisplayName = "Salli (Neural, Female, English, US)"),
    Joey UMETA(DisplayName = "Joey (Standard, Male, English, US)"),
    JoeyNeural UMETA(DisplayName = "Joey (Neural, Male, English, US)"),
    Justin UMETA(DisplayName = "Justin (Standard, Male Child, English, US)"),
    JustinNeural UMETA(DisplayName = "Justin (Neural, Male Child, English, US)"),
    Matthew UMETA(DisplayName = "Matthew (Standard, Male, English, US)"),
    MatthewNeural UMETA(DisplayName = "Matthew (Neural, Male, English, US)"),
    Geraint UMETA(DisplayName = "Geraint (Standard, Male, English, Welsh)")
};

/**
 * Returns the Amazon Polly VoiceId enum corresponding to the
 * given module specific EVoiceId enum.
 * 
 * @param VoiceId the module specific EVoiceId enum
 * @return Aws::Polly::Model::VoiceId the Amazon Polly VoiceId enum
 */
Aws::Polly::Model::VoiceId ToPollyVoiceId(const EVoiceId VoiceId);

/**
 * Returns neural or standard voice engine depending on the voiceId
 * @param VoiceId - enum to be mapped to the voice engine
 * @return Aws::Polly::Model::Engine - Engine object to be used in configuring PollyRequest 
 */
Aws::Polly::Model::Engine ToPollyVoiceEngine(const EVoiceId VoiceId);