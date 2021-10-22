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

/**
* Custom KeyFunc for case sensitive FString hashing in TMap
* @see BaseKeyFuncs for details
*/
template <typename ValueType>
struct CaseSensitiveKeyFunc : BaseKeyFuncs<TPair<FString, ValueType>, FString> {                                                                             
    using Super = BaseKeyFuncs<TPair<FString, ValueType>, FString>;
    using ElementInitType = typename Super::ElementInitType;
    using KeyInitType = typename Super::KeyInitType;

    /**
    * Returns the key associated with a ValueType element in the map
    * @param Element - the element in the map 
    * @return - the key associated with the element 
    */
    static KeyInitType GetSetKey(ElementInitType Element) {
        return Element.Key;
    }

    /**
    * Determines if two FString keys are the same through case-sensitive comparison
    * @param A - an FString key
    * @param B - an FString key
    * @return bool - boolean indicating if the keys are the same
    */
    static bool Matches(KeyInitType A, KeyInitType B) {
        return A.Compare(B, ESearchCase::CaseSensitive) == 0;
    }

    /**
    * Returns a hash for an FString key through case-sensitive hashing function
    * @param key - the key to be hasheds
    * @return uint32 - the hash of the key 
    */
    static uint32 GetKeyHash(KeyInitType Key) {
        return FCrc::StrCrc32(*Key);
    }
};
