/*
 * Nexus.js - The next-gen JavaScript platform
 * Copyright (C) 2016  Abdullah A. Hassan <abdullah@webtomizer.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "nexus.h"
#include "context.h"
#include "object.h"
#include "value.h"
#include "globals/global.h"
#include "scoped_string.h"

NX::Context::Context (NX::Context * parent, NX::Nexus * nx, JSContextGroupRef group, JSClassRef globalClass) :
  myNexus(parent ? parent->nexus() : nx), myGroup (parent ? parent->group() : group),
  myContext (nullptr), myGlobals(), myGlobalObject (nullptr), myModuleObject(nullptr),
  myGenericClass(), myObjectClasses(), myParent(parent)
{
  JSClassRef gClass = globalClass ? globalClass : JSClassCreate(&Global::GlobalClass);
  myContext = myGroup ? JSGlobalContextCreateInGroup(myGroup, gClass) : JSGlobalContextCreate(gClass);
  myContext = JSGlobalContextRetain(myContext);
  myGlobalObject = JSContextGetGlobalObject(myContext);
  JSClassDefinition genericClassDef = kJSClassDefinitionEmpty;
  genericClassDef.className = "Object";
  myGenericClass = JSClassCreate(&genericClassDef);
  if (!globalClass)
    JSClassRelease(gClass);
  JSValueRef exception = nullptr;
  initGlobal(myGlobalObject, &exception);
  if (exception)
    nx->ReportException(myContext, exception);
}

JSValueRef NX::Context::evaluateScript (const std::string & src, JSObjectRef thisObject,
                                       const std::string & filePath, unsigned int lineNo, JSValueRef * exception)
{
  NX::ScopedString srcRef(src);
  NX::ScopedString filePathRef(filePath);
  JSValueRef ret = JSEvaluateScript(myContext, srcRef, thisObject,
                                    filePath.length() ? (JSStringRef)filePathRef : nullptr, lineNo, exception);
  return ret;
}

NX::Context::~Context()
{
  JSValueUnprotect(myContext, myModuleObject);
  for(auto & o : myGlobals)
    JSValueUnprotect(myContext, o.second);
  for(auto & c : myObjectClasses)
    JSClassRelease(c.second);
  JSClassRelease(myGenericClass);
  JSGlobalContextRelease(myContext);
}

JSObjectRef NX::Context::getModuleObject(JSValueRef * exception)
{
  if (myModuleObject)
    return myModuleObject;
  JSObjectRef Module = NX::Object(myContext, getOrInitGlobal("Nexus"))["Module"]->toObject()->value();
  JSObjectRef moduleObject = NX::Object(myContext, Module).construct(std::vector<JSValueRef>(), exception);
  JSValueProtect(myContext, moduleObject);
  return myModuleObject = moduleObject;
}

void NX::Context::initGlobal (JSObjectRef object, JSValueRef * exception)
{
//   try {
    JSObjectSetPrivate(object, this);
    NX::Object global(myContext, object);
    global.set("module", getModuleObject(exception));
    if (exception && *exception) return;
//     JSObjectRef systemObject = global["Loader"]->toObject()->construct(std::vector<JSValueRef>(), exception);
//     if (exception && *exception) return;
//     global.set("System", systemObject, kJSPropertyAttributeNone, exception);
//     if (exception && *exception) return;
//   } catch(const std::runtime_error & e) {
//     NX::Value message(myContext, e.what());
//     JSValueRef args[] { message.value(), nullptr };
//     *exception = JSObjectMakeError(myContext, 1, args, nullptr);
//   }
}


