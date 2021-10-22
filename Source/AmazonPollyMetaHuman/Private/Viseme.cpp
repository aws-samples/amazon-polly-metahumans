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

#include "Viseme.h"
#include "CaseSensitiveKeyFunc.h"

DEFINE_LOG_CATEGORY(LogAmazonPollyViseme);

/*
* Maps the FString representation of a viseme to its corresponding EViseme enum 
* NOTE: A custom case-sensitive key-func is implemented to compare FStrings, as Visemes 
* have both capital and lowercase versions, and the default FString comparison 
* (uses ==) is case-insensitive 
* @param String - an FString of the viseme 
* @return - the EViseme enum corresponding to the FString 
*/
EViseme GetVisemeValueFromString(const FString& String)
{
    static TMap<FString, EViseme, FDefaultSetAllocator, CaseSensitiveKeyFunc<EViseme>> VisemeMap = {
        {TEXT("p"),EViseme::P}, 
        {TEXT("t"),EViseme::LowerT},
        {TEXT("S"),EViseme::S},
        {TEXT("T"),EViseme::T}, 
        {TEXT("f"),EViseme::F},
        {TEXT("k"),EViseme::K},
        {TEXT("i"),EViseme::I},
        {TEXT("r"),EViseme::R},
        {TEXT("s"),EViseme::LowerS},
        {TEXT("u"),EViseme::U},
        {TEXT("@"),EViseme::At},
        {TEXT("a"),EViseme::A}, 
        {TEXT("e"),EViseme::LowerE}, 
        {TEXT("E"),EViseme::E},
        {TEXT("o"),EViseme::LowerO},
        {TEXT("O"),EViseme::O},
        {TEXT("sil"),EViseme::Sil} 
    };
    auto Viseme = VisemeMap.Find(String);
    if (!Viseme)
    {
        UE_LOG(LogAmazonPollyViseme, Error, TEXT("Tried to read an invalid viseme value. Returning Sil as default. Invalid value: %s"), *String);
        return EViseme::Sil;
    }
    return *Viseme;
};