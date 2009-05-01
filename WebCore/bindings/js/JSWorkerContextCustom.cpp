/*
 * Copyright (C) 2008, 2009 Apple Inc. All Rights Reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 
 */

#include "config.h"

#if ENABLE(WORKERS)

#include "JSWorkerContext.h"

#include "JSDOMBinding.h"
#include "JSEventListener.h"
#include "JSWorkerLocation.h"
#include "JSWorkerNavigator.h"
#include "JSXMLHttpRequestConstructor.h"
#include "ScheduledAction.h"
#include "WorkerContext.h"
#include "WorkerLocation.h"
#include "WorkerNavigator.h"
#include <interpreter/Interpreter.h>

using namespace JSC;

namespace WebCore {

bool JSWorkerContext::customGetOwnPropertySlot(ExecState* exec, const Identifier& propertyName, PropertySlot& slot)
{
    // Look for overrides before looking at any of our own properties.
    if (JSGlobalObject::getOwnPropertySlot(exec, propertyName, slot))
        return true;
    return false;
}

void JSWorkerContext::mark()
{
    Base::mark();

    JSGlobalData& globalData = *this->globalData();

    markActiveObjectsForContext(globalData, scriptExecutionContext());

    markDOMObjectWrapper(globalData, impl()->optionalLocation());
    markDOMObjectWrapper(globalData, impl()->optionalNavigator());

    markIfNotNull(impl()->onmessage());

    typedef WorkerContext::EventListenersMap EventListenersMap;
    typedef WorkerContext::ListenerVector ListenerVector;
    EventListenersMap& eventListeners = impl()->eventListeners();
    for (EventListenersMap::iterator mapIter = eventListeners.begin(); mapIter != eventListeners.end(); ++mapIter) {
        for (ListenerVector::iterator vecIter = mapIter->second.begin(); vecIter != mapIter->second.end(); ++vecIter)
            (*vecIter)->markJSFunction();
    }
}

JSValue JSWorkerContext::self(ExecState*) const
{
    return JSValue(this);
}

void JSWorkerContext::setSelf(ExecState* exec, JSValue value)
{
    putDirect(Identifier(exec, "self"), value);
}

JSValue JSWorkerContext::xmlHttpRequest(ExecState* exec) const
{
    return getDOMConstructor<JSXMLHttpRequestConstructor>(exec, this);
}

JSValue JSWorkerContext::importScripts(ExecState* exec, const ArgList& args)
{
    if (!args.size())
        return jsUndefined();

    Vector<String> urls;
    for (unsigned i = 0; i < args.size(); i++) {
        urls.append(args.at(i).toString(exec));
        if (exec->hadException())
            return jsUndefined();
    }
    ExceptionCode ec = 0;
    int signedLineNumber;
    intptr_t sourceID;
    UString sourceURL;
    JSValue function;
    exec->interpreter()->retrieveLastCaller(exec, signedLineNumber, sourceID, sourceURL, function);

    impl()->importScripts(urls, sourceURL, signedLineNumber >= 0 ? signedLineNumber : 0, ec);
    setDOMException(exec, ec);
    return jsUndefined();
}

JSValue JSWorkerContext::addEventListener(ExecState* exec, const ArgList& args)
{
    RefPtr<JSEventListener> listener = findOrCreateJSEventListener(args.at(1));
    if (!listener)
        return jsUndefined();
    impl()->addEventListener(args.at(0).toString(exec), listener.release(), args.at(2).toBoolean(exec));
    return jsUndefined();
}

JSValue JSWorkerContext::removeEventListener(ExecState* exec, const ArgList& args)
{
    JSEventListener* listener = findJSEventListener(args.at(1));
    if (!listener)
        return jsUndefined();
    impl()->removeEventListener(args.at(0).toString(exec), listener, args.at(2).toBoolean(exec));
    return jsUndefined();
}

JSValue JSWorkerContext::setTimeout(ExecState* exec, const ArgList& args)
{
    ScheduledAction* action = ScheduledAction::create(exec, args);
    if (exec->hadException())
        return jsUndefined();
    int delay = args.at(1).toInt32(exec);
    return jsNumber(exec, impl()->setTimeout(action, delay));
}

JSValue JSWorkerContext::setInterval(ExecState* exec, const ArgList& args)
{
    ScheduledAction* action = ScheduledAction::create(exec, args);
    if (exec->hadException())
        return jsUndefined();
    int delay = args.at(1).toInt32(exec);
    return jsNumber(exec, impl()->setInterval(action, delay));
}

} // namespace WebCore

#endif // ENABLE(WORKERS)
