/*
 * Copyright (C) 2009 Google Inc. All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 * 
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "V8HTMLAllCollection.h"

#include "core/dom/NamedNodesCollection.h"
#include "core/html/HTMLAllCollection.h"

#include "V8Node.h"
#include "V8NodeList.h"
#include "bindings/v8/V8Binding.h"

namespace WebCore {

template<class HolderContainer>
static v8::Handle<v8::Value> getNamedItems(HTMLAllCollection* collection, AtomicString name, const HolderContainer& holder)
{
    Vector<RefPtr<Node> > namedItems;
    collection->namedItems(name, namedItems);

    if (!namedItems.size())
        return v8Undefined();

    if (namedItems.size() == 1)
        return toV8Fast(namedItems.at(0).release(), holder, collection);

    // FIXME: HTML5 specification says this should be a HTMLCollection.
    // http://www.whatwg.org/specs/web-apps/current-work/multipage/common-dom-interfaces.html#htmlallcollection
    return toV8Fast(NamedNodesCollection::create(namedItems), holder, collection);
}

template<class HolderContainer>
static v8::Handle<v8::Value> getItem(HTMLAllCollection* collection, v8::Handle<v8::Value> argument, const HolderContainer& holder)
{
    v8::Local<v8::Uint32> index = argument->ToArrayIndex();
    if (index.IsEmpty()) {
        v8::Handle<v8::Value> result = getNamedItems(collection, toWebCoreString(argument->ToString()), holder);

        if (result.IsEmpty())
            return v8::Undefined();

        return result;
    }

    RefPtr<Node> result = collection->item(index->Uint32Value());
    return toV8Fast(result.release(), holder, collection);
}

void V8HTMLAllCollection::itemMethodCustom(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    HTMLAllCollection* imp = V8HTMLAllCollection::toNative(args.Holder());
    v8SetReturnValue(args, getItem(imp, args[0], args));
}

void V8HTMLAllCollection::namedItemMethodCustom(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    HTMLAllCollection* imp = V8HTMLAllCollection::toNative(args.Holder());
    v8::Handle<v8::Value> result = getNamedItems(imp, toWebCoreString(args[0]), args);

    if (result.IsEmpty()) {
        v8SetReturnValueNull(args);
        return;
    }

    v8SetReturnValue(args, result);
}

void V8HTMLAllCollection::legacyCallCustom(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    if (args.Length() < 1)
        return;

    HTMLAllCollection* imp = V8HTMLAllCollection::toNative(args.Holder());

    if (args.Length() == 1) {
        v8SetReturnValue(args, getItem(imp, args[0], args));
        return;
    }

    // If there is a second argument it is the index of the item we want.
    String name = toWebCoreString(args[0]);
    v8::Local<v8::Uint32> index = args[1]->ToArrayIndex();
    if (index.IsEmpty())
        return;

    if (Node* node = imp->namedItemWithIndex(name, index->Uint32Value())) {
        v8SetReturnValue(args, toV8Fast(node, args, imp));
        return;
    }
}

} // namespace WebCore
