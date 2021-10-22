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

#include "Misc/AutomationTest.h"
#include "TestableSpeechComponent.h"
#include <strstream>

/**
* Creates a lambda function that returns a failed PollyOutcome 
* @return - the lambda function 
*/
TFunction<PollyOutcome()> CreatePollyErrorOutcome() {
    auto ErrorOutcomeLambda = []() {
        PollyOutcome Outcome;
        Outcome.IsSuccess = false;
        Outcome.PollyErrorMsg = "error";
        return Outcome;
    };
    return ErrorOutcomeLambda;
}

/**
* Creates a lambda function that returns a successful PollyOutcome 
* @param text - text to be passed into a stream used to fill Streambuffer 
* @return - the lambda function 
*/
TFunction<PollyOutcome()> CreatePollySuccessfulOutcome(Aws::String text) {
    auto SuccessfulOutcomeLambda = [text]() {
        PollyOutcome Outcome;
        Outcome.IsSuccess = true;
        Aws::StringStream PollyStream(text);
        Outcome.StreamBuffer = UnrealAWSUtils::PreparePollyData(PollyStream);
        return Outcome;
    };
    return SuccessfulOutcomeLambda;
}

BEGIN_DEFINE_SPEC(AmazonPollySpec, "AmazonPolly.Unit Tests", EAutomationTestFlags::ClientContext | EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)
UTestableSpeechComponent* TestableSpeechComponent;
END_DEFINE_SPEC(AmazonPollySpec)

void::AmazonPollySpec::Define() {

    Describe("UnrealAWSUtils tests", [this]() {

        Describe("FStringToAwsString(UnrealString)", [this]() {
            It("should convert to equivalent Aws String 'Polly'", [this]() {
                // given an FString "Polly"
                FString UnrealString = TEXT("Polly");
                Aws::String TestString = "Polly";
                // when FStringToAwsString is called on the FString
                Aws::String Result = UnrealAWSUtils::FStringToAwsString(UnrealString);
                // then the result should be an AwsString "Polly" 
                TestEqual("Conversion to Aws String 'Polly'", Result, TestString);
            });

            It("should convert to equivalent Aws String 'ABC123!@#'", [this]() {
                // given an FString "ABC123!@#"
                FString UnrealString = TEXT("ABC123!@#");
                Aws::String TestString = "ABC123!@#";
                // when FStringToAwsString is called on the FString
                Aws::String Result = UnrealAWSUtils::FStringToAwsString(UnrealString);
                // then the result should be an AwsString "ABC123!@#"
                TestEqual("Conversion to Aws String 'ABC123!@#'", Result, TestString);
            });

            It("should convert to equivalent Aws String ''", [this]() {
                // given an empty FString ""
                FString UnrealString = TEXT("");
                Aws::String TestString = "";
                // when FStringToAwsString is called on the FString
                Aws::String Result = UnrealAWSUtils::FStringToAwsString(UnrealString);
                // then the result should be an empty AwsString ""
                TestEqual("Conversion to Aws String ''", Result, TestString);
            });

            It("Move constructor on strings should not cause deallocation failures", [this]() {
                // given a string long enough to cause dynamic allocations (> ~16 byte Small String Optimization)
                FString UnrealString = TEXT("This is a string which will cause a dynamic allocation.");
                Aws::String TestString = "This is a string which will cause a dynamic allocation.";
                // when SetText is called on the PollyRequest with an RValue 
                {
                    Aws::Polly::Model::SynthesizeSpeechRequest PollyRequest;
                    PollyRequest.SetText(UnrealAWSUtils::FStringToAwsString(UnrealString));
                    TestEqual("Set PollyRequest text through move construction.", PollyRequest.GetText(), TestString);
                    // then the destruction of the PollyRequest should not cause a heap error
                }
                // PollyRequest deallocated 
                });
        });

        Describe("AwsStringToFString(AwsString)", [this]() {
            It("should convert to equivalent FString 'Polly'", [this]() {
                // given an AwsString "Polly"
                Aws::String AwsString = "Polly";
                FString TestString = TEXT("Polly");
                // when AwsStringToFString is called on the AwsString
                FString Result = UnrealAWSUtils::AwsStringToFString(AwsString);
                // then the result should be a FString "Polly"
                TestEqual("Conversion to FString 'Polly'", Result, TestString);
            });

            It("should convert to equivalent FString 'ABC123!@#'", [this]() {
                //given an AwsString "ABC123@!#"
                Aws::String AwsString = "ABC123!@#";
                FString TestString = TEXT("ABC123!@#");
                // when AwsStringToFString is called on the AwsString
                FString Result = UnrealAWSUtils::AwsStringToFString(AwsString);
                // then the result should be a FString "ABC123!@#"
                TestEqual("Conversion to FString 'ABC123!@#', and returned" + Result, Result, TestString);
            });

            It("should convert to equivalent FString ''", [this]() {
                // given an empty AwsString ""
                Aws::String AwsString = "";
                FString TestString = TEXT("");
                // when AwsStringToFString is called on the AwsString
                FString Result = UnrealAWSUtils::AwsStringToFString(AwsString);
                // then the result should be an empty FString ""
                TestEqual("Conversion to FString ''", Result, TestString);
            });
        });

        Describe("PreparePollyData(IOStream)", [this]() {
            It("should prepare a buffer from a stream", [this]() {
                // given a stream 
                Aws::StringStream ss("abc");
                // when PreparePollyData is called on the stream
                TArray<uint8> Buffer = UnrealAWSUtils::PreparePollyData(ss);
                // then the Buffer should be filled with data from the stream 
                TestTrue("Buffer is filled after call", Buffer.Num() > 0);
            });

            It("should prepare a valid audio buffer from an audiostream", [this]() {
                // given a valid audio stream
                Aws::StringStream ss("@#ABCDE12345");
                // when PreparePollyData is called on the stream
                TArray<uint8> Buffer = UnrealAWSUtils::PreparePollyData(ss);
                // then the Buffer should be filled with data from the stream
                TestTrue("Buffer is filled after call", Buffer.Num() > 0);
                // when a USoundWave object is created from data from the Buffer 
                USoundWaveProcedural* PollyAudio = NewObject<USoundWaveProcedural>();
                PollyAudio->SetSampleRate(16000);
                PollyAudio->NumChannels = 1;
                PollyAudio->DecompressionType = DTYPE_Procedural;
                PollyAudio->QueueAudio(Buffer.GetData(), Buffer.Num());
                // then the USoundWave object should be playable 
                TestTrue("PollyAudio->IsPlayable() should return true", PollyAudio->IsPlayable());
            });

            It("should prepare a valid viseme buffer from a stream of visemes", [this]() {
                // given a valid stream of visemes
                Aws::StringStream VisemeStream("{\"time\":125,\"type\":\"viseme\",\"value\":\"p\"}");
                // when PreparePollyData is called on the Stream
                TArray<uint8> Buffer = UnrealAWSUtils::PreparePollyData(VisemeStream);
                // then the Buffer should be filled with data from the stream
                TestTrue("Buffer is filled after call", Buffer.Num() > 0);
                // when the Buffer is converted to a string and parsed by Json utilities 
                FString VisemeJson;
                FFileHelper::BufferToString(VisemeJson, Buffer.GetData(), Buffer.Num());
                TSharedPtr<FJsonObject> JsonParsed = MakeShareable(new FJsonObject);
                TSharedRef<TJsonReader<TCHAR>> JsonReader = TJsonReaderFactory<TCHAR>::Create(VisemeJson);
                FJsonSerializer::Deserialize(JsonReader, JsonParsed);
                FString Viseme = JsonParsed->GetStringField("value");
                int Time = JsonParsed->GetIntegerField("time");
                // then the Json utilties should return the 'p' viseme and '125' time fields from the string 
                TestTrue("Returned viseme should be p", Viseme == "p");
                TestTrue("Returned time should be 125", Time == 125);
            });
        });
    });

    Describe("SpeechComponent tests", [this]() {

        Describe("Initializing a SpeechComponent", [this]() {

            It("should have an empty Audiobuffer and VisemeEventArray'", [this]() {
                // given no prior setup 
                // when a SpeechComponent is initialized
                TestableSpeechComponent = NewObject<UTestableSpeechComponent>();
                // then the Audiobuffer and VisemeEventArray should be empty 
                TestTrue("VisemeEventArray should be empty upon initialization", TestableSpeechComponent->GetVisemeEventArray().Num() == 0);
                TestTrue("Audiobuffer should be empty upon initialization", TestableSpeechComponent->GetAudiobuffer().Num() == 0);
            });

        });

        Describe("GenerateSpeechSync(text, VoiceId) ", [this]() {

            BeforeEach([this]() {
                TestableSpeechComponent = NewObject<UTestableSpeechComponent>();
                TestableSpeechComponent->InitializePollyClient();
            });

            It("should not synthesize any audio or visemes for an empty text input", [this]() {
                // given an empty text passed into GenerateSpeechSync
                AddExpectedError(TEXT("Cannot generate speech"), EAutomationExpectedErrorFlags::Contains);
                // when GenerateSpeechSync is invoked with the empty text
                TestableSpeechComponent->GenerateSpeechSync("", EVoiceId::Joanna);
                HasMetExpectedErrors();
                // then an error should be logged and the Audiobuffer and VisemeEventArray should be empty 
                TestTrue("Audiobuffer is empty after call", TestableSpeechComponent->GetAudiobuffer().Num() == 0);
                TestTrue("VisemeEventArray is empty after call", TestableSpeechComponent->GetVisemeEventArray().Num() == 0);
            });

            It("should not synthesize any audio or visemes when StartSpeech invoked (bIsSpeaking is true)", [this]() {
                AddExpectedError(TEXT("Cannot generate speech during playback"), EAutomationExpectedErrorFlags::Contains);
                // given a bool value of true for bIsSpeaking (StartSpeech invoked)
                TestableSpeechComponent->SetSpeaking(true);
                // when GenerateSpeechSync is invoked
                TestableSpeechComponent->GenerateSpeechSync("sampletext", EVoiceId::Joanna);
                HasMetExpectedErrors();
                // then an error should be logged and the Audiobuffer and VisemeEventArray should be empty 
                TestTrue("Audiobuffer is empty after call", TestableSpeechComponent->GetAudiobuffer().Num() == 0);
                TestTrue("VisemeEventArray is empty after call", TestableSpeechComponent->GetVisemeEventArray().Num() == 0);
            });

            It("should log an error when an error occurs while generating audio and log the error from Polly", [this]() {
                MockPollyClient* MockPollyClient = TestableSpeechComponent->GetPollyClient();
                // given an error while generating audio 
                // and thus a PollyOutcome object that has an IsSuccess value of false
                MockPollyClient->AddSynthesizeSpeechBehavior(CreatePollyErrorOutcome());
                MockPollyClient->AddSynthesizeSpeechBehavior(CreatePollyErrorOutcome());
                AddExpectedError(TEXT("Polly failed to generate audio file. Error: error"), EAutomationExpectedErrorFlags::Contains);
                // when GenerateSpeechSync is invoked
                TestableSpeechComponent->GenerateSpeechSync(TEXT("sampletext"), EVoiceId::Joanna);
                // then an error should be logged 
                HasMetExpectedErrors();
            });

            It("should log an error when an error occurs while generating visemes and log the error from Polly", [this]() {
                MockPollyClient* MockPollyClient = TestableSpeechComponent->GetPollyClient();
                // given an error while generating visemes 
                // and thus a PollyOutcome object that has an IsSuccess value of false
                MockPollyClient->AddSynthesizeSpeechBehavior(CreatePollySuccessfulOutcome("@#ABCDE12345"));
                MockPollyClient->AddSynthesizeSpeechBehavior(CreatePollyErrorOutcome());
                AddExpectedError(TEXT("Polly failed to generate visemes. Error: error"), EAutomationExpectedErrorFlags::Contains);
                // when GenerateSpeechSync is invoked
                TestableSpeechComponent->GenerateSpeechSync(TEXT("sampletext"), EVoiceId::Joanna);
                // then an error should be logged 
                HasMetExpectedErrors();
            });

            It("should populate VisemeEventArray with 1 visemes and timestamp for a viseme stream containing one viseme", [this]() {
                MockPollyClient* MockPollyClient = TestableSpeechComponent->GetPollyClient();
                MockPollyClient->AddSynthesizeSpeechBehavior(CreatePollySuccessfulOutcome("@#ABCDE12345"));
                // given proper calls to SynthesizeSpeech 
                // and thus a PollyOutcome object with IsSuccess = true and
                // a Buffer generated from a proper stream of visemes 
                MockPollyClient->AddSynthesizeSpeechBehavior(CreatePollySuccessfulOutcome("{\"time\":125,\"type\":\"viseme\",\"value\":\"p\"}"));
                // when GenerateSpeechSync is invoked
                TestableSpeechComponent->GenerateSpeechSync("sampletext", EVoiceId::Joanna);
                // then VisemeEventArray should be populated with 1 viseme and timestamp from the stream 
                TestEqual("VisemeEventArray is populated after call", TestableSpeechComponent->GetVisemeEventArray().Num(), 1);
                TestEqual("viseme1", TestableSpeechComponent->GetVisemeEventArray()[0].Viseme, EViseme::P);
                TestEqual("timestamp1", TestableSpeechComponent->GetVisemeEventArray()[0].TimeMilliseconds, 125);
                TestTrue("All lambdas invoked during GenerateSpeechSync", MockPollyClient->SynthesizeSpeechBehaviors.IsEmpty());
                });

            It("should populate VisemeEventArray with 16 visemes and timestamps for 'Hi! My name is Joanna.'", [this]() {
                MockPollyClient* MockPollyClient = TestableSpeechComponent->GetPollyClient();
                MockPollyClient->AddSynthesizeSpeechBehavior(CreatePollySuccessfulOutcome("@#ABCDE12345"));
                // given proper calls to SynthesizeSpeech 
                // and thus a PollyOutcome object with IsSuccess = true and
                // a Buffer generated from a proper stream of visemes 
                MockPollyClient->AddSynthesizeSpeechBehavior(CreatePollySuccessfulOutcome("{\"time\":125,\"type\":\"viseme\",\"value\":\"k\"}\n{\"time\":237,\"type\":\"viseme\",\"value\":\"a\"}\n{\"time\":562,\"type\":\"viseme\",\"value\":\"sil\"}\n{\"time\":1330,\"type\":\"viseme\",\"value\":\"p\"}\n{\"time\":1442,\"type\":\"viseme\",\"value\":\"a\"}\n{\"time\":1505,\"type\":\"viseme\",\"value\":\"t\"}\n{\"time\":1642,\"type\":\"viseme\",\"value\":\"e\"}\n{\"time\":1692,\"type\":\"viseme\",\"value\":\"p\"}\n{\"time\":1755,\"type\":\"viseme\",\"value\":\"i\"}\n{\"time\":1817,\"type\":\"viseme\",\"value\":\"s\"}\n{\"time\":1905,\"type\":\"viseme\",\"value\":\"S\"}\n{\"time\":2030,\"type\":\"viseme\",\"value\":\"o\"}\n{\"time\":2192,\"type\":\"viseme\",\"value\":\"a\"}\n{\"time\":2230,\"type\":\"viseme\",\"value\":\"t\"}\n{\"time\":2330,\"type\":\"viseme\",\"value\":\"@\"}\n{\"time\":2542,\"type\":\"viseme\",\"value\":\"sil\"}"));
                // when GenerateSpeechSync is invoked
                TestableSpeechComponent->GenerateSpeechSync("sampletext", EVoiceId::Joanna);
                // then VisemeEventArray should be populated with 16 visemes and timestamps from the stream 
                TestEqual("VisemeEventArray is populated after call", TestableSpeechComponent->GetVisemeEventArray().Num(), 16);
                TestEqual("viseme1", TestableSpeechComponent->GetVisemeEventArray()[0].Viseme, EViseme::K);
                TestEqual("viseme2", TestableSpeechComponent->GetVisemeEventArray()[1].Viseme, EViseme::A);
                TestEqual("viseme3", TestableSpeechComponent->GetVisemeEventArray()[2].Viseme, EViseme::Sil);
                TestEqual("viseme4", TestableSpeechComponent->GetVisemeEventArray()[3].Viseme, EViseme::P);
                TestEqual("viseme5", TestableSpeechComponent->GetVisemeEventArray()[4].Viseme, EViseme::A);
                TestEqual("viseme6", TestableSpeechComponent->GetVisemeEventArray()[5].Viseme, EViseme::LowerT);
                TestEqual("viseme7", TestableSpeechComponent->GetVisemeEventArray()[6].Viseme, EViseme::LowerE);
                TestEqual("viseme8", TestableSpeechComponent->GetVisemeEventArray()[7].Viseme, EViseme::P);
                TestEqual("viseme9", TestableSpeechComponent->GetVisemeEventArray()[8].Viseme, EViseme::I);
                TestEqual("viseme10", TestableSpeechComponent->GetVisemeEventArray()[9].Viseme, EViseme::LowerS);
                TestEqual("viseme11", TestableSpeechComponent->GetVisemeEventArray()[10].Viseme, EViseme::S);
                TestEqual("viseme12", TestableSpeechComponent->GetVisemeEventArray()[11].Viseme, EViseme::LowerO);
                TestEqual("viseme13", TestableSpeechComponent->GetVisemeEventArray()[12].Viseme, EViseme::A);
                TestEqual("viseme14", TestableSpeechComponent->GetVisemeEventArray()[13].Viseme, EViseme::LowerT);
                TestEqual("viseme15", TestableSpeechComponent->GetVisemeEventArray()[14].Viseme, EViseme::At);
                TestEqual("viseme16", TestableSpeechComponent->GetVisemeEventArray()[15].Viseme, EViseme::Sil);
                TestEqual("TimeMilliseconds1", TestableSpeechComponent->GetVisemeEventArray()[0].TimeMilliseconds, 125);
                TestEqual("TimeMilliseconds2", TestableSpeechComponent->GetVisemeEventArray()[1].TimeMilliseconds, 237);
                TestEqual("TimeMilliseconds3", TestableSpeechComponent->GetVisemeEventArray()[2].TimeMilliseconds, 562);
                TestEqual("TimeMilliseconds4", TestableSpeechComponent->GetVisemeEventArray()[3].TimeMilliseconds, 1330);
                TestEqual("TimeMilliseconds5", TestableSpeechComponent->GetVisemeEventArray()[4].TimeMilliseconds, 1442);
                TestEqual("TimeMilliseconds6", TestableSpeechComponent->GetVisemeEventArray()[5].TimeMilliseconds, 1505);
                TestEqual("TimeMilliseconds7", TestableSpeechComponent->GetVisemeEventArray()[6].TimeMilliseconds, 1642);
                TestEqual("TimeMilliseconds8", TestableSpeechComponent->GetVisemeEventArray()[7].TimeMilliseconds, 1692);
                TestEqual("TimeMilliseconds9", TestableSpeechComponent->GetVisemeEventArray()[8].TimeMilliseconds, 1755);
                TestEqual("TimeMilliseconds10", TestableSpeechComponent->GetVisemeEventArray()[9].TimeMilliseconds, 1817);
                TestEqual("TimeMilliseconds11", TestableSpeechComponent->GetVisemeEventArray()[10].TimeMilliseconds, 1905);
                TestEqual("TimeMilliseconds12", TestableSpeechComponent->GetVisemeEventArray()[11].TimeMilliseconds, 2030);
                TestEqual("TimeMilliseconds13", TestableSpeechComponent->GetVisemeEventArray()[12].TimeMilliseconds, 2192);
                TestEqual("TimeMilliseconds14", TestableSpeechComponent->GetVisemeEventArray()[13].TimeMilliseconds, 2230);
                TestEqual("TimeMilliseconds15", TestableSpeechComponent->GetVisemeEventArray()[14].TimeMilliseconds, 2330);
                TestEqual("TimeMilliseconds16", TestableSpeechComponent->GetVisemeEventArray()[15].TimeMilliseconds, 2542);
                TestTrue("All lambdas invoked during GenerateSpeechSync", MockPollyClient->SynthesizeSpeechBehaviors.IsEmpty());
            });

            It("should populate VisemeEventArray with 27 visemes and timestamps for '!%^&123'", [this]() {
                MockPollyClient* MockPollyClient = TestableSpeechComponent->GetPollyClient();
                MockPollyClient->AddSynthesizeSpeechBehavior(CreatePollySuccessfulOutcome("@#ABCDE12345"));
                // given proper calls to SynthesizeSpeech 
                // and thus a PollyOutcome object with IsSuccess = true and a 
                // Buffer generated from a proper stream of visemes generated from special characters 
                MockPollyClient->AddSynthesizeSpeechBehavior(CreatePollySuccessfulOutcome("{\"time\":62,\"type\":\"viseme\",\"value\":\"p\"}\n{\"time\":187,\"type\":\"viseme\",\"value\":\"E\"}\n{\"time\":237,\"type\":\"viseme\",\"value\":\"s\"}\n{\"time\":300,\"type\":\"viseme\",\"value\":\"E\"}\n{\"time\":375,\"type\":\"viseme\",\"value\":\"t\"}\n{\"time\":450,\"type\":\"viseme\",\"value\":\"t\"}\n{\"time\":562,\"type\":\"viseme\",\"value\":\"k\"}\n{\"time\":625,\"type\":\"viseme\",\"value\":\"a\"}\n{\"time\":712,\"type\":\"viseme\",\"value\":\"r\"}\n{\"time\":750,\"type\":\"viseme\",\"value\":\"@\"}\n{\"time\":937,\"type\":\"viseme\",\"value\":\"t\"}\n{\"time\":1037,\"type\":\"viseme\",\"value\":\"@\"}\n{\"time\":1062,\"type\":\"viseme\",\"value\":\"t\"}\n{\"time\":1125,\"type\":\"viseme\",\"value\":\"t\"}\n{\"time\":1200,\"type\":\"viseme\",\"value\":\"u\"}\n{\"time\":1250,\"type\":\"viseme\",\"value\":\"E\"}\n{\"time\":1312,\"type\":\"viseme\",\"value\":\"t\"}\n{\"time\":1487,\"type\":\"viseme\",\"value\":\"t\"}\n{\"time\":1562,\"type\":\"viseme\",\"value\":\"u\"}\n{\"time\":1587,\"type\":\"viseme\",\"value\":\"E\"}\n{\"time\":1625,\"type\":\"viseme\",\"value\":\"t\"}\n{\"time\":1700,\"type\":\"viseme\",\"value\":\"t\"}\n{\"time\":1750,\"type\":\"viseme\",\"value\":\"i\"}\n{\"time\":1875,\"type\":\"viseme\",\"value\":\"T\"}\n{\"time\":1937,\"type\":\"viseme\",\"value\":\"r\"}\n{\"time\":2087,\"type\":\"viseme\",\"value\":\"i\"}\n{\"time\":2275,\"type\":\"viseme\",\"value\":\"sil\"}"));
                // when GenerateSpeechSync is invoked 
                TestableSpeechComponent->GenerateSpeechSync("sampletext", EVoiceId::Joanna);
                // then VisemeEventArray should be populated with 27 visemes and timestamps from the stream 
                TestEqual("VisemeEventArray is populated after call", TestableSpeechComponent->GetVisemeEventArray().Num(), 27);
                TestEqual("viseme1", TestableSpeechComponent->GetVisemeEventArray()[0].Viseme, EViseme::P);
                TestEqual("viseme2", TestableSpeechComponent->GetVisemeEventArray()[1].Viseme, EViseme::E);
                TestEqual("viseme3", TestableSpeechComponent->GetVisemeEventArray()[2].Viseme, EViseme::LowerS);
                TestEqual("viseme4", TestableSpeechComponent->GetVisemeEventArray()[3].Viseme, EViseme::E);
                TestEqual("viseme5", TestableSpeechComponent->GetVisemeEventArray()[4].Viseme, EViseme::LowerT);
                TestEqual("viseme6", TestableSpeechComponent->GetVisemeEventArray()[5].Viseme, EViseme::LowerT);
                TestEqual("viseme7", TestableSpeechComponent->GetVisemeEventArray()[6].Viseme, EViseme::K);
                TestEqual("viseme8", TestableSpeechComponent->GetVisemeEventArray()[7].Viseme, EViseme::A);
                TestEqual("viseme9", TestableSpeechComponent->GetVisemeEventArray()[8].Viseme, EViseme::R);
                TestEqual("viseme10", TestableSpeechComponent->GetVisemeEventArray()[9].Viseme, EViseme::At);
                TestEqual("viseme11", TestableSpeechComponent->GetVisemeEventArray()[10].Viseme, EViseme::LowerT);
                TestEqual("viseme12", TestableSpeechComponent->GetVisemeEventArray()[11].Viseme, EViseme::At);
                TestEqual("viseme13", TestableSpeechComponent->GetVisemeEventArray()[12].Viseme, EViseme::LowerT);
                TestEqual("viseme14", TestableSpeechComponent->GetVisemeEventArray()[13].Viseme, EViseme::LowerT);
                TestEqual("viseme15", TestableSpeechComponent->GetVisemeEventArray()[14].Viseme, EViseme::U);
                TestEqual("viseme16", TestableSpeechComponent->GetVisemeEventArray()[15].Viseme, EViseme::E);
                TestEqual("viseme17", TestableSpeechComponent->GetVisemeEventArray()[16].Viseme, EViseme::LowerT);
                TestEqual("viseme18", TestableSpeechComponent->GetVisemeEventArray()[17].Viseme, EViseme::LowerT);
                TestEqual("viseme19", TestableSpeechComponent->GetVisemeEventArray()[18].Viseme, EViseme::U);
                TestEqual("viseme20", TestableSpeechComponent->GetVisemeEventArray()[19].Viseme, EViseme::E);
                TestEqual("viseme21", TestableSpeechComponent->GetVisemeEventArray()[20].Viseme, EViseme::LowerT);
                TestEqual("viseme22", TestableSpeechComponent->GetVisemeEventArray()[21].Viseme, EViseme::LowerT);
                TestEqual("viseme23", TestableSpeechComponent->GetVisemeEventArray()[22].Viseme, EViseme::I);
                TestEqual("viseme24", TestableSpeechComponent->GetVisemeEventArray()[23].Viseme, EViseme::T);
                TestEqual("viseme25", TestableSpeechComponent->GetVisemeEventArray()[24].Viseme, EViseme::R);
                TestEqual("viseme26", TestableSpeechComponent->GetVisemeEventArray()[25].Viseme, EViseme::I);
                TestEqual("viseme27", TestableSpeechComponent->GetVisemeEventArray()[26].Viseme, EViseme::Sil);
                TestEqual("TimeMilliseconds1", TestableSpeechComponent->GetVisemeEventArray()[0].TimeMilliseconds, 62);
                TestEqual("TimeMilliseconds2", TestableSpeechComponent->GetVisemeEventArray()[1].TimeMilliseconds, 187);
                TestEqual("TimeMilliseconds3", TestableSpeechComponent->GetVisemeEventArray()[2].TimeMilliseconds, 237);
                TestEqual("TimeMilliseconds4", TestableSpeechComponent->GetVisemeEventArray()[3].TimeMilliseconds, 300);
                TestEqual("TimeMilliseconds5", TestableSpeechComponent->GetVisemeEventArray()[4].TimeMilliseconds, 375);
                TestEqual("TimeMilliseconds6", TestableSpeechComponent->GetVisemeEventArray()[5].TimeMilliseconds, 450);
                TestEqual("TimeMilliseconds7", TestableSpeechComponent->GetVisemeEventArray()[6].TimeMilliseconds, 562);
                TestEqual("TimeMilliseconds8", TestableSpeechComponent->GetVisemeEventArray()[7].TimeMilliseconds, 625);
                TestEqual("TimeMilliseconds9", TestableSpeechComponent->GetVisemeEventArray()[8].TimeMilliseconds, 712);
                TestEqual("TimeMilliseconds10", TestableSpeechComponent->GetVisemeEventArray()[9].TimeMilliseconds, 750);
                TestEqual("TimeMilliseconds11", TestableSpeechComponent->GetVisemeEventArray()[10].TimeMilliseconds, 937);
                TestEqual("TimeMilliseconds12", TestableSpeechComponent->GetVisemeEventArray()[11].TimeMilliseconds, 1037);
                TestEqual("TimeMilliseconds13", TestableSpeechComponent->GetVisemeEventArray()[12].TimeMilliseconds, 1062);
                TestEqual("TimeMilliseconds14", TestableSpeechComponent->GetVisemeEventArray()[13].TimeMilliseconds, 1125);
                TestEqual("TimeMilliseconds15", TestableSpeechComponent->GetVisemeEventArray()[14].TimeMilliseconds, 1200);
                TestEqual("TimeMilliseconds16", TestableSpeechComponent->GetVisemeEventArray()[15].TimeMilliseconds, 1250);
                TestEqual("TimeMilliseconds17", TestableSpeechComponent->GetVisemeEventArray()[16].TimeMilliseconds, 1312);
                TestEqual("TimeMilliseconds18", TestableSpeechComponent->GetVisemeEventArray()[17].TimeMilliseconds, 1487);
                TestEqual("TimeMilliseconds19", TestableSpeechComponent->GetVisemeEventArray()[18].TimeMilliseconds, 1562);
                TestEqual("TimeMilliseconds20", TestableSpeechComponent->GetVisemeEventArray()[19].TimeMilliseconds, 1587);
                TestEqual("TimeMilliseconds21", TestableSpeechComponent->GetVisemeEventArray()[20].TimeMilliseconds, 1625);
                TestEqual("TimeMilliseconds22", TestableSpeechComponent->GetVisemeEventArray()[21].TimeMilliseconds, 1700);
                TestEqual("TimeMilliseconds23", TestableSpeechComponent->GetVisemeEventArray()[22].TimeMilliseconds, 1750);
                TestEqual("TimeMilliseconds24", TestableSpeechComponent->GetVisemeEventArray()[23].TimeMilliseconds, 1875);
                TestEqual("TimeMilliseconds25", TestableSpeechComponent->GetVisemeEventArray()[24].TimeMilliseconds, 1937);
                TestEqual("TimeMilliseconds26", TestableSpeechComponent->GetVisemeEventArray()[25].TimeMilliseconds, 2087);
                TestEqual("TimeMilliseconds27", TestableSpeechComponent->GetVisemeEventArray()[26].TimeMilliseconds, 2275);
                TestTrue("All lambdas invoked during GenerateSpeechSync", MockPollyClient->SynthesizeSpeechBehaviors.IsEmpty());
            });

            It("should populate VisemeEventArray with all possible visemes when JSON contains every possible viseme", [this]() {
                MockPollyClient* MockPollyClient = TestableSpeechComponent->GetPollyClient();
                MockPollyClient->AddSynthesizeSpeechBehavior(CreatePollySuccessfulOutcome("@#ABCDE12345"));
                // given proper calls to SynthesizeSpeech and thus a PollyOutcome object with IsSuccess = true and 
                // containing a Buffer generated from a stream with all possible visemes
                MockPollyClient->AddSynthesizeSpeechBehavior(CreatePollySuccessfulOutcome("{\"time\":125,\"type\":\"viseme\",\"value\":\"p\"}\n{\"time\":200,\"type\":\"viseme\",\"value\":\"E\"}\n{\"time\":237,\"type\":\"viseme\",\"value\":\"t\"}\n{\"time\":450,\"type\":\"viseme\",\"value\":\"i\"}\n{\"time\":500,\"type\":\"viseme\",\"value\":\"k\"}\n{\"time\":625,\"type\":\"viseme\",\"value\":\"S\"}\n{\"time\":875,\"type\":\"viseme\",\"value\":\"T\"}\n{\"time\":1062,\"type\":\"viseme\",\"value\":\"f\"}\n{\"time\":1200,\"type\":\"viseme\",\"value\":\"a\"}\n{\"time\":1487,\"type\":\"viseme\",\"value\":\"e\"}\n{\"time\":1875,\"type\":\"viseme\",\"value\":\"s\"}\n{\"time\":1962,\"type\":\"viseme\",\"value\":\"r\"}\n{\"time\":2787,\"type\":\"viseme\",\"value\":\"u\"}\n{\"time\":3312,\"type\":\"viseme\",\"value\":\"@\"}\n{\"time\":4625,\"type\":\"viseme\",\"value\":\"o\"}\n{\"time\":4937,\"type\":\"viseme\",\"value\":\"O\"}\n{\"time\":5212,\"type\":\"viseme\",\"value\":\"sil\"}"));
                // when GenerateSpeechSync is invoked
                TestableSpeechComponent->GenerateSpeechSync("sampletext", EVoiceId::Joanna);
                // then VisemeEventArray should be populated with 17 visemes and contain all possible visemes 
                TestEqual("VisemeEventArray is populated after call", TestableSpeechComponent->GetVisemeEventArray().Num(), 17);
                TestEqual("p", TestableSpeechComponent->GetVisemeEventArray()[0].Viseme, EViseme::P);
                TestEqual("E", TestableSpeechComponent->GetVisemeEventArray()[1].Viseme, EViseme::E);
                TestEqual("t", TestableSpeechComponent->GetVisemeEventArray()[2].Viseme, EViseme::LowerT);
                TestEqual("i", TestableSpeechComponent->GetVisemeEventArray()[3].Viseme, EViseme::I);
                TestEqual("k", TestableSpeechComponent->GetVisemeEventArray()[4].Viseme, EViseme::K);
                TestEqual("S", TestableSpeechComponent->GetVisemeEventArray()[5].Viseme, EViseme::S);
                TestEqual("T", TestableSpeechComponent->GetVisemeEventArray()[6].Viseme, EViseme::T);
                TestEqual("f", TestableSpeechComponent->GetVisemeEventArray()[7].Viseme, EViseme::F);
                TestEqual("a", TestableSpeechComponent->GetVisemeEventArray()[8].Viseme, EViseme::A);
                TestEqual("e", TestableSpeechComponent->GetVisemeEventArray()[9].Viseme, EViseme::LowerE);
                TestEqual("s", TestableSpeechComponent->GetVisemeEventArray()[10].Viseme, EViseme::LowerS);
                TestEqual("r", TestableSpeechComponent->GetVisemeEventArray()[11].Viseme, EViseme::R);
                TestEqual("u", TestableSpeechComponent->GetVisemeEventArray()[12].Viseme, EViseme::U);
                TestEqual("@", TestableSpeechComponent->GetVisemeEventArray()[13].Viseme, EViseme::At);
                TestEqual("o", TestableSpeechComponent->GetVisemeEventArray()[14].Viseme, EViseme::LowerO);
                TestEqual("O", TestableSpeechComponent->GetVisemeEventArray()[15].Viseme, EViseme::O);
                TestEqual("sil", TestableSpeechComponent->GetVisemeEventArray()[16].Viseme, EViseme::Sil);
                TestTrue("All lambdas invoked during GenerateSpeechSync", MockPollyClient->SynthesizeSpeechBehaviors.IsEmpty());
            });

            // This test is to ensure viseme data doesn't stick around in between runs of StartSpeech, which can cause issues during playback with repeated states/animations
            It("should synthesize visemes for '!@#ABC000', replacing the previous synthesized data", [this]() {
                MockPollyClient* MockPollyClient = TestableSpeechComponent->GetPollyClient();
                // given proper calls to SynthesizeSpeech and thus two PollyVisemeOutcome objects with IsSuccess = true and 
                // containing buffers generated from two different proper streams of visemes
                MockPollyClient->AddSynthesizeSpeechBehavior(CreatePollySuccessfulOutcome("@#ABCDE12345"));
                MockPollyClient->AddSynthesizeSpeechBehavior(CreatePollySuccessfulOutcome("{\"time\":125,\"type\":\"viseme\",\"value\":\"k\"}\n{\"time\":237,\"type\":\"viseme\",\"value\":\"a\"}\n{\"time\":562,\"type\":\"viseme\",\"value\":\"sil\"}\n{\"time\":1330,\"type\":\"viseme\",\"value\":\"p\"}\n{\"time\":1442,\"type\":\"viseme\",\"value\":\"a\"}\n{\"time\":1505,\"type\":\"viseme\",\"value\":\"t\"}\n{\"time\":1642,\"type\":\"viseme\",\"value\":\"e\"}\n{\"time\":1692,\"type\":\"viseme\",\"value\":\"p\"}\n{\"time\":1755,\"type\":\"viseme\",\"value\":\"i\"}\n{\"time\":1817,\"type\":\"viseme\",\"value\":\"s\"}\n{\"time\":1905,\"type\":\"viseme\",\"value\":\"S\"}\n{\"time\":2030,\"type\":\"viseme\",\"value\":\"o\"}\n{\"time\":2192,\"type\":\"viseme\",\"value\":\"a\"}\n{\"time\":2230,\"type\":\"viseme\",\"value\":\"t\"}\n{\"time\":2330,\"type\":\"viseme\",\"value\":\"@\"}\n{\"time\":2542,\"type\":\"viseme\",\"value\":\"sil\"}"));
                MockPollyClient->AddSynthesizeSpeechBehavior(CreatePollySuccessfulOutcome("@#ABCDE12345"));
                MockPollyClient->AddSynthesizeSpeechBehavior(CreatePollySuccessfulOutcome("{\"time\":62,\"type\":\"viseme\",\"value\":\"p\"}\n{\"time\":187,\"type\":\"viseme\",\"value\":\"E\"}\n{\"time\":237,\"type\":\"viseme\",\"value\":\"s\"}\n{\"time\":300,\"type\":\"viseme\",\"value\":\"E\"}\n{\"time\":375,\"type\":\"viseme\",\"value\":\"t\"}\n{\"time\":450,\"type\":\"viseme\",\"value\":\"t\"}\n{\"time\":562,\"type\":\"viseme\",\"value\":\"k\"}\n{\"time\":625,\"type\":\"viseme\",\"value\":\"a\"}\n{\"time\":712,\"type\":\"viseme\",\"value\":\"r\"}\n{\"time\":750,\"type\":\"viseme\",\"value\":\"@\"}\n{\"time\":937,\"type\":\"viseme\",\"value\":\"t\"}\n{\"time\":1037,\"type\":\"viseme\",\"value\":\"@\"}\n{\"time\":1062,\"type\":\"viseme\",\"value\":\"t\"}\n{\"time\":1125,\"type\":\"viseme\",\"value\":\"t\"}\n{\"time\":1200,\"type\":\"viseme\",\"value\":\"u\"}\n{\"time\":1250,\"type\":\"viseme\",\"value\":\"E\"}\n{\"time\":1312,\"type\":\"viseme\",\"value\":\"t\"}\n{\"time\":1487,\"type\":\"viseme\",\"value\":\"t\"}\n{\"time\":1562,\"type\":\"viseme\",\"value\":\"u\"}\n{\"time\":1587,\"type\":\"viseme\",\"value\":\"E\"}\n{\"time\":1625,\"type\":\"viseme\",\"value\":\"t\"}\n{\"time\":1700,\"type\":\"viseme\",\"value\":\"t\"}\n{\"time\":1750,\"type\":\"viseme\",\"value\":\"i\"}\n{\"time\":1875,\"type\":\"viseme\",\"value\":\"T\"}\n{\"time\":1937,\"type\":\"viseme\",\"value\":\"r\"}\n{\"time\":2087,\"type\":\"viseme\",\"value\":\"i\"}\n{\"time\":2275,\"type\":\"viseme\",\"value\":\"sil\"}"));
                // when GenerateSpeechSync is invoked for the first time
                TestableSpeechComponent->GenerateSpeechSync("sampletext", EVoiceId::Joanna);
                // then VisemeEventArray should be filled with 16 visemes from the first PollyVisemeOutcome object 
                TestTrue("VisemeEventArray is filled after call", TestableSpeechComponent->GetVisemeEventArray().Num() == 16);
                // when GenerateSpeechSync is invoked for the second time
                TestableSpeechComponent->GenerateSpeechSync("sampletext", EVoiceId::Joanna);
                // then VisemeEventArray should only contain the 27 visemes from the second PollyVisemeOutcome object
                TestTrue("VisemeEventArray empties previous data and fills with new data after second call", TestableSpeechComponent->GetVisemeEventArray().Num() == 27);
                TestTrue("All lambdas invoked during GenerateSpeechSync", MockPollyClient->SynthesizeSpeechBehaviors.IsEmpty());
            });

            It("should log an error for non-Json file", [this]() {
                AddExpectedError(TEXT("Failed to parse json formatted viseme sequence returned by Amazon Polly"), EAutomationExpectedErrorFlags::Contains);
                MockPollyClient* MockPollyClient = TestableSpeechComponent->GetPollyClient();
                MockPollyClient->AddSynthesizeSpeechBehavior(CreatePollySuccessfulOutcome("@#ABCDE12345"));
                // given a proper call to SynthesizeSpeech 
                // and thus a PollyOutcome object with IsSuccess = true and a Buffer generated from 
                // an improper stream that does not contain a JSON 
                MockPollyClient->AddSynthesizeSpeechBehavior(CreatePollySuccessfulOutcome("NOT A JSON"));
                // when GenerateSpeechSync is invoked
                TestableSpeechComponent->GenerateSpeechSync("sampletext", EVoiceId::Joanna);
                // then an error should be logged and VisemeEventArray and Audiobuffer should be empty 
                TestTrue("VisemeEventArray is empty after call", TestableSpeechComponent->GetVisemeEventArray().Num() == 0);
                TestTrue("Audiobuffer is empty after call", TestableSpeechComponent->GetAudiobuffer().Num() == 0);
                TestTrue("All lambdas invoked during GenerateSpeechSync", MockPollyClient->SynthesizeSpeechBehaviors.IsEmpty());
                HasMetExpectedErrors();
            });

            It("should log an error for an invalid Json file", [this]() {
                AddExpectedError(TEXT("Failed to parse json formatted viseme sequence returned by Amazon Polly"), EAutomationExpectedErrorFlags::Contains);
                MockPollyClient* MockPollyClient = TestableSpeechComponent->GetPollyClient();
                MockPollyClient->AddSynthesizeSpeechBehavior(CreatePollySuccessfulOutcome("@#ABCDE12345"));
                // given a proper call to SynthesizeSpeech 
                // and thus a PollyOutcome object with IsSuccess = true and a Buffer generated from 
                // an improper stream that contains an improperly formatted JSON
                MockPollyClient->AddSynthesizeSpeechBehavior(CreatePollySuccessfulOutcome("{\"random\":125,\"field\":\"hi\",\"mom\":\"k\"}"));
                // when GenerateSpeechSync is invoked
                TestableSpeechComponent->GenerateSpeechSync("sampletext", EVoiceId::Joanna);
                // then an error should be logged and VisemeEventArray and Audiobuffer should be empty 
                TestTrue("VisemeEventArray is empty after call", TestableSpeechComponent->GetVisemeEventArray().Num() == 0);
                TestTrue("Audiobuffer is empty after call", TestableSpeechComponent->GetAudiobuffer().Num() == 0);
                TestTrue("All lambdas invoked during GenerateSpeechSync", MockPollyClient->SynthesizeSpeechBehaviors.IsEmpty());
                HasMetExpectedErrors();
            });

            It("should log an error for an invalid viseme value", [this]() {
                AddExpectedError(TEXT("Tried to read an invalid viseme value. Returning Sil as default."), EAutomationExpectedErrorFlags::Contains);
                MockPollyClient* MockPollyClient = TestableSpeechComponent->GetPollyClient();
                MockPollyClient->AddSynthesizeSpeechBehavior(CreatePollySuccessfulOutcome("@#ABCDE12345"));
                // given a proper call to SynthesizeSpeech 
                // and thus a PollyOutcome object with IsSuccess = true and a Buffer generated from 
                // an improper stream that contains a properly formatted JSON with 
                // an invalid viseme value 
                MockPollyClient->AddSynthesizeSpeechBehavior(CreatePollySuccessfulOutcome("{\"time\":125,\"type\":\"viseme\",\"value\":\"J\"}"));
                // when GenerateSpeechSync is invoked 
                TestableSpeechComponent->GenerateSpeechSync("sampletext", EVoiceId::Joanna);
                // then an error should be logged and the returned viseme should be the default viseme Sil 
                TestEqual("Sil returned when an invalid viseme is read", TestableSpeechComponent->GetVisemeEventArray()[0].Viseme, EViseme::Sil);
                TestTrue("All lambdas invoked during GenerateSpeechSync", MockPollyClient->SynthesizeSpeechBehaviors.IsEmpty());
                HasMetExpectedErrors();
            });

            It("should synthesize audio for 'Hi! My name is Chandler!'", [this]() {
                MockPollyClient* MockPollyClient = TestableSpeechComponent->GetPollyClient();
                // given a proper call to SynthesizeSpech
                // and thus a PollyOutcome object with IsSuccess = true and a Buffer generated from
                // a proper audio stream 
                MockPollyClient->AddSynthesizeSpeechBehavior(CreatePollySuccessfulOutcome("Hi! My name is Chandler!"));
                MockPollyClient->AddSynthesizeSpeechBehavior(CreatePollySuccessfulOutcome("{\"time\":125,\"type\":\"viseme\",\"value\":\"p\"}"));
                // when GenerateSpeechSync is invoked
                TestableSpeechComponent->GenerateSpeechSync("Hi! My name is Chandler!", EVoiceId::Joey);
                // then the Audiobuffer should be filled with the Buffer data
                TestTrue("StringStream is a multiple of 4", (TestableSpeechComponent->GetAudiobuffer().Num() % 4) == 0);
                TestTrue("AudioBuffer is filled after call", TestableSpeechComponent->GetAudiobuffer().Num() > 0);
                // when a USoundWave object is constructed from the Audiobuffer
                USoundWaveProcedural* PollyAudio = NewObject<USoundWaveProcedural>();
                PollyAudio->SetSampleRate(16000);
                PollyAudio->NumChannels = 1;
                PollyAudio->DecompressionType = DTYPE_Procedural;
                PollyAudio->QueueAudio(TestableSpeechComponent->GetAudiobuffer().GetData(), TestableSpeechComponent->GetAudiobuffer().Num());
                // then the USoundWave object should be valid and playable 
                TestTrue("PollyAudio->IsPlayable() should return true", PollyAudio->IsPlayable());
                TestTrue("All lambdas invoked during GenerateSpeechSync", MockPollyClient->SynthesizeSpeechBehaviors.IsEmpty());
            });

            It("should synthesize audio for '@#ABCDE12345'", [this]() {
                MockPollyClient* MockPollyClient = TestableSpeechComponent->GetPollyClient();
                // given a proper call to SynthesizeSpech
                // and thus a PollyOutcome object with IsSuccess = true and a Buffer generated from
                // a proper audio stream 
                MockPollyClient->AddSynthesizeSpeechBehavior(CreatePollySuccessfulOutcome("@#ABCDE12345"));
                MockPollyClient->AddSynthesizeSpeechBehavior(CreatePollySuccessfulOutcome("{\"time\":125,\"type\":\"viseme\",\"value\":\"p\"}"));
                // when GenerateSpeechSync is invoked
                TestableSpeechComponent->GenerateSpeechSync("@#ABCDE12345", EVoiceId::Kimberly);
                // then the Audiobuffer should be filled with the Buffer data
                TestTrue("StringStream is a multiple of 4", (TestableSpeechComponent->GetAudiobuffer().Num() % 4) == 0);
                TestTrue("AudioBuffer is filled after call", TestableSpeechComponent->GetAudiobuffer().Num() > 0);
                // when a USoundWave object is constructed from the Audiobuffer
                USoundWaveProcedural* PollyAudio = NewObject<USoundWaveProcedural>();
                PollyAudio->SetSampleRate(16000);
                PollyAudio->NumChannels = 1;
                PollyAudio->DecompressionType = DTYPE_Procedural;
                PollyAudio->QueueAudio(TestableSpeechComponent->GetAudiobuffer().GetData(), TestableSpeechComponent->GetAudiobuffer().Num());
                // then the USoundWave object should be valid and playable 
                TestTrue("PollyAudio->IsPlayable() should return true", PollyAudio->IsPlayable());
                TestTrue("All lambdas invoked during GenerateSpeechSync", MockPollyClient->SynthesizeSpeechBehaviors.IsEmpty());
            });

            // This test is to ensure audio data doesn't stick around in between runs of StartSpeech, which can cause issues during playback with repeated audio
            It("should replace the previous synthesized data", [this]() {
                MockPollyClient* MockPollyClient = TestableSpeechComponent->GetPollyClient();
                // given proper calls to SynthesizeSpech
                // and thus two PollyOutcome objects with IsSuccess = true and buffers generated from
                // two proper audio streams
                MockPollyClient->AddSynthesizeSpeechBehavior(CreatePollySuccessfulOutcome("Hi! My name is Chandler!"));
                MockPollyClient->AddSynthesizeSpeechBehavior(CreatePollySuccessfulOutcome("{\"time\":125,\"type\":\"viseme\",\"value\":\"p\"}"));
                MockPollyClient->AddSynthesizeSpeechBehavior(CreatePollySuccessfulOutcome("Hi! My name is Chandler!"));
                MockPollyClient->AddSynthesizeSpeechBehavior(CreatePollySuccessfulOutcome("{\"time\":125,\"type\":\"viseme\",\"value\":\"p\"}"));
                // when GenerateSpeechSync is invoked for the first time
                TestableSpeechComponent->GenerateSpeechSync("Hi! My name is Chandler!", EVoiceId::Joey);
                // then the Audiobuffer should be filled with the buffer data from
                // the first PollyOutcome object 
                TestTrue("AudioBuffer size is 24, the size of the first audio stream", TestableSpeechComponent->GetAudiobuffer().Num() == 24);
                // when GenerateSpeechSync is invoked for the second time
                TestableSpeechComponent->GenerateSpeechSync("Hi! My name is Chandler!", EVoiceId::Joey);
                // then the Audiobuffer data from the first call to GenerateSpeechSync should be replaced 
                // by the Audiobuffer data from the second call to GenerateSpech 
                TestTrue("AudioBuffer size is 24, the size of the second audio stream, and does not contain a duplicate of the audio stream data from the first call", TestableSpeechComponent->GetAudiobuffer().Num() == 24);
                TestTrue("All lambdas invoked during GenerateSpeechSync", MockPollyClient->SynthesizeSpeechBehaviors.IsEmpty());
            });
        });

        Describe("StartSpeech()", [this]() {

            BeforeEach([this]() {
                TestableSpeechComponent = NewObject<UTestableSpeechComponent>();
                TestableSpeechComponent->InitializePollyClient();
            });

            It("should iterate through entire VisemeEventArray, set CurrentViseme state to last Viseme, and generate a valid SoundWave object", [this]() {
                MockPollyClient* MockPollyClient = TestableSpeechComponent->GetPollyClient();
                MockPollyClient->AddSynthesizeSpeechBehavior(CreatePollySuccessfulOutcome("Hi! My name is Chandler!"));
                // given a proper call to SynthesizeSpeech
                // and thus a proper PollyOutcome object with IsSuccess = true and contains
                // a Buffer containing data from a valid viseme stream 
                MockPollyClient->AddSynthesizeSpeechBehavior(CreatePollySuccessfulOutcome("{\"time\":125,\"type\":\"viseme\",\"value\":\"p\"}\n{\"time\":200,\"type\":\"viseme\",\"value\":\"E\"}\n{\"time\":237,\"type\":\"viseme\",\"value\":\"t\"}\n{\"time\":450,\"type\":\"viseme\",\"value\":\"i\"}\n{\"time\":500,\"type\":\"viseme\",\"value\":\"k\"}"));
                // given a call to GenerateSpeechSync 
                TestableSpeechComponent->GenerateSpeechSync("sampletext", EVoiceId::Joanna);
                TestTrue("All lambdas invoked during GenerateSpeechSync", MockPollyClient->SynthesizeSpeechBehaviors.IsEmpty());
                // when StartSpeech is invoked
                USoundWaveProcedural* PollyAudio = TestableSpeechComponent->StartSpeech();
                // then the USoundWave object constructed from the Audiobuffer data should be playable 
                // and have a duration of 24*8 / (16000 * 16) = 0.00075
                // and the CurrentViseme state should be set to the first Viseme, P 
                // and the CurrentVisemeIndex should be 0 
                TestTrue("PollyAudio->IsPlayable() should return true", PollyAudio->IsPlayable());
                TestTrue("PollyAudio->Duration should return 0.00075", PollyAudio->Duration == 0.00075f);
                TestTrue("CurrentVisemeIndex should be 0", TestableSpeechComponent->GetCurrentVisemeIndex() == 0);
                TestEqual("CurrentViseme should be the first Viseme, P", TestableSpeechComponent->GetCurrentViseme(), EViseme::P);
                // when PlayNextViseme() is invoked by the timer 
                TestableSpeechComponent->PlayNextViseme();
                // then the CurrentViseme State should be set to the second Viseme, E
                // and the CurrentVisemeIndex should be 1 
                TestTrue("CurrentVisemeIndex should be 1", TestableSpeechComponent->GetCurrentVisemeIndex() == 1);
                TestEqual("CurrentViseme should be the second Viseme, E", TestableSpeechComponent->GetCurrentViseme(), EViseme::E);
                // when PlayNextViseme() is invoked by the timer 
                TestableSpeechComponent->PlayNextViseme();
                // then the CurrentViseme State should be set to the third Viseme, LowerT
                // and the CurrentVisemeIndex should be 2
                TestTrue("CurrentVisemeIndex should be 2", TestableSpeechComponent->GetCurrentVisemeIndex() == 2);
                TestEqual("CurrentViseme should be the second Viseme, LowerT", TestableSpeechComponent->GetCurrentViseme(), EViseme::LowerT);
                // when PlayNextViseme() is invoked by the timer 
                TestableSpeechComponent->PlayNextViseme();
                // then the CurrentViseme State should be set to the fourth Viseme, I
                // and the CurrentVisemeIndex should be 3 
                TestTrue("CurrentVisemeIndex should be 3", TestableSpeechComponent->GetCurrentVisemeIndex() == 3);
                TestEqual("CurrentViseme should be the third Viseme, I", TestableSpeechComponent->GetCurrentViseme(), EViseme::I);
                // when PlayNextViseme() is invoked by the timer 
                TestableSpeechComponent->PlayNextViseme();
                // then the CurrentViseme State should be set to the last Viseme, K
                // and the CurrentVisemeIndex should be 4
                TestTrue("CurrentVisemeIndex should be 4", TestableSpeechComponent->GetCurrentVisemeIndex() == 4);
                TestEqual("CurrentViseme should be the final Viseme, K", TestableSpeechComponent->GetCurrentViseme(), EViseme::K);
                // when PlayNextViseme() is invoked by the timer for the final time 
                TestableSpeechComponent->PlayNextViseme();
                // then the CurrentVisemeState should still be the last Viseme, K 
                // and the CurrentVisemeIndex should be the size of the VisemeEventArray, 5
                TestTrue("CurrentVisemeIndex should be the size of the VisemeEventArray, 5", TestableSpeechComponent->GetCurrentVisemeIndex() == TestableSpeechComponent->GetVisemeEventArray().Num());
                TestEqual("CurrentViseme should still be the final Viseme, K", TestableSpeechComponent->GetCurrentViseme(), EViseme::K);
            });

            It("should iterate through entire VisemeEventArray, set CurrentViseme state to last Viseme for a single viseme, and generate a valid SoundWave object", [this]() {
                MockPollyClient* MockPollyClient = TestableSpeechComponent->GetPollyClient();
                MockPollyClient->AddSynthesizeSpeechBehavior(CreatePollySuccessfulOutcome("Hi! My name is Chandler!"));
                // given a proper call to SynthesizeSpeech
                // and thus a proper PollyOutcome object with IsSuccess = true and contains
                // a Buffer containing data from a valid viseme stream with a single viseme
                MockPollyClient->AddSynthesizeSpeechBehavior(CreatePollySuccessfulOutcome("{\"time\":125,\"type\":\"viseme\",\"value\":\"p\"}"));
                // given a call to GenerateSpeechSync 
                TestableSpeechComponent->GenerateSpeechSync("sampletext", EVoiceId::Joey);
                TestTrue("All lambdas invoked during GenerateSpeechSync", MockPollyClient->SynthesizeSpeechBehaviors.IsEmpty());
                // when StartSpeech is invoked
                USoundWaveProcedural* PollyAudio = TestableSpeechComponent->StartSpeech();
                // then the USoundWave object constructed from the Audiobuffer data should be playable 
                // and the CurrentViseme state should be set to the first and final Viseme, P 
                // and the CurrentVisemeIndex should be 0 
                TestTrue("PollyAudio->IsPlayable() should return true", PollyAudio->IsPlayable());
                TestTrue("CurrentVisemeIndex should be 0", TestableSpeechComponent->GetCurrentVisemeIndex() == 0);
                TestEqual("CurrentViseme should be the final Viseme, P", TestableSpeechComponent->GetCurrentViseme(), EViseme::P);
                // when PlayNextViseme() is invoked by the timer
                TestableSpeechComponent->PlayNextViseme();
                // then the CurrentViseme state should still be the viseme P
                // and the CurrentVisemeIndex should be the size of the VisemeEventArray 
                TestEqual("CurrentViseme should still be the final Viseme, P", TestableSpeechComponent->GetCurrentViseme(), EViseme::P);
                TestTrue("CurrentVisemeIndex should be the size of the VisemeEventArray, 1", TestableSpeechComponent->GetCurrentVisemeIndex() == TestableSpeechComponent->GetVisemeEventArray().Num());
            });

            It("should not StartSpeech before GenerateSpeechSync invoked (empty VisemeEventArray)", [this]() {
                AddExpectedError(TEXT("Failed to start speech"), EAutomationExpectedErrorFlags::Contains);
                auto result = TestableSpeechComponent->StartSpeech();
                TestTrue("StartSpeech returns a nullptr on error", nullptr == result);
                HasMetExpectedErrors();
            });
        });
    });
}
