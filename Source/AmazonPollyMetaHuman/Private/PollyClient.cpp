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

#include "PollyClient.h" 
#include <aws/polly/model/SynthesizeSpeechResult.h>
#include <aws/polly/PollyRequest.h>
#include <aws/core/utils/Outcome.h>
#include <iostream>

PollyClient::PollyClient() {
    Aws::Client::ClientConfiguration configuration;
    configuration.userAgent = "request-source/AmazonPollyMetaHuman";
    AwsPollyClient = MakeUnique<Aws::Polly::PollyClient>(configuration);
}

PollyClient::~PollyClient() {};

PollyOutcome PollyClient::SynthesizeSpeech(const Aws::Polly::Model::SynthesizeSpeechRequest& SpeechRequest) {
    PollyOutcome Outcome;
    Aws::Polly::Model::SynthesizeSpeechOutcome SpeechOutcome = AwsPollyClient->SynthesizeSpeech(SpeechRequest);
    if (SpeechOutcome.IsSuccess()) {
        Outcome.StreamBuffer = UnrealAWSUtils::PreparePollyData(SpeechOutcome.GetResult().GetAudioStream());
        Outcome.IsSuccess = true;
    }
    else {
        Outcome.IsSuccess = false;
        Outcome.PollyErrorMsg = SpeechOutcome.GetError().GetMessage();
    }
    return Outcome;
}