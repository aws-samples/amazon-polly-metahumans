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

DECLARE_LOG_CATEGORY_EXTERN(LogAmazonPollyViseme, Log, All);

/**
* Enum representing a single viseme to be used in 
* setting of CurrentViseme state and for selecting
* the correct pose in the Anim Graph through a Blend Enum function 
*/
UENUM(BlueprintType)
enum class EViseme : uint8
{
    Sil UMETA(DisplayName = "Sil"),
    P UMETA(DisplayName = "P"),
    LowerT UMETA(DisplayName = "LowerT"),
    S UMETA(DisplayName = "S"),
    T UMETA(DisplayName = "T"),
    F UMETA(DisplayName = "F"),
    K UMETA(DisplayName = "K"),
    I UMETA(DisplayName = "I"),
    R UMETA(DisplayName = "R"),
    LowerS UMETA(DisplayName = "LowerS"),
    U UMETA(DisplayName = "U"),
    At UMETA(DisplayName = "@"),
    A UMETA(DisplayName = "A"),
    E UMETA(DisplayName = "E"),
    LowerE UMETA(DisplayName = "LowerE"),
    O UMETA(DisplayName = "O"),
    LowerO UMETA(DisplayName = "LowerO")
};

/**
 * Returns the enum represetnation of the viseme given 
 * the string representation of the viseme. Note that the
 * string representation is that returned by Amazon Polly,
 * not the DisplayName corresponding to the EViseme.
 * 
 * @param String the string representation of the viseme 
 * @return EViseme the enum represetnation of the viseme
 */
EViseme GetVisemeValueFromString(const FString& String);