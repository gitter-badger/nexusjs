#include "globals/scheduler.h"
#include "scheduler.h"
#include "nexus.h"
#include "value.h"
#include "task.h"

#include <boost/thread.hpp>

const JSClassDefinition NX::Globals::Scheduler::Class {
  0, kJSClassAttributeNone, "Scheduler", nullptr, NX::Globals::Scheduler::Properties, NX::Globals::Scheduler::Methods
};


JSValueRef NX::Globals::Scheduler::Get (JSContextRef ctx, JSObjectRef object, JSStringRef propertyName, JSValueRef * exception)
{
  NX::Context * context = Context::FromJsContext(ctx);
  if (JSObjectRef Scheduler = context->getGlobal("Nexus.Scheduler"))
    return Scheduler;
  return context->setGlobal("Nexus.Scheduler", JSObjectMake(context->toJSContext(),
                                                     context->defineOrGetClass(NX::Globals::Scheduler::Class),
                                                     context->nexus()->scheduler().get()));
}

const JSStaticValue NX::Globals::Scheduler::Properties[] {
  { "threadId", [](JSContextRef ctx, JSObjectRef object, JSStringRef propertyName, JSValueRef* exception) -> JSValueRef {
      std::ostringstream ss;
      ss << boost::this_thread::get_id();
      std::string id = ss.str();
      return NX::Value(ctx, id).value();
    }, nullptr, kJSPropertyAttributeReadOnly
  },
  { "concurrency", [](JSContextRef ctx, JSObjectRef object, JSStringRef propertyName, JSValueRef* exception) -> JSValueRef {
      NX::Scheduler * scheduler = reinterpret_cast<NX::Scheduler*>(JSObjectGetPrivate(object));
      return NX::Value(ctx, scheduler->concurrency()).value();
    }, nullptr, kJSPropertyAttributeReadOnly
  },
  { nullptr, nullptr, nullptr, 0 }
};

const JSStaticFunction NX::Globals::Scheduler::Methods[] {
  { "schedule", [](JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
    size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception) -> JSValueRef {
      NX::Context * context = Context::FromJsContext(ctx);
      NX::Scheduler * scheduler = reinterpret_cast<NX::Scheduler*>(JSObjectGetPrivate(thisObject));
      JSObjectRef fun = argumentCount > 0 ? JSValueToObject(ctx, arguments[0], exception) : nullptr;
      if (fun) {
        JSValueProtect(context->toJSContext(), fun);
        NX::AbstractTask * taskPtr = scheduler->scheduleTask([=]() {
          JSValueRef exp = nullptr;
          JSValueRef ret = JSObjectCallAsFunction(context->toJSContext(), fun, nullptr, 0, nullptr, &exp);
          if (exp) {
            NX::Nexus::ReportException(context->toJSContext(), exp);
          }
          JSValueUnprotect(context->toJSContext(), fun);
        });
        if (!taskPtr) {
          NX::Value message(ctx, "unable to schedule task");
          JSValueRef args[] { message.value(), nullptr };
          *exception = JSObjectMakeError(ctx, 1, args, nullptr);
        } /*else {
          NX::ScopedString abortName("abort");
          JSObjectRef taskObject = JSObjectMake(context->toJSContext(), context->defineOrGetClass({ 0, 0, "Task" }), taskPtr);
          JSObjectSetProperty(ctx, taskObject, abortName, JSObjectMakeFunctionWithCallback(
            ctx, abortName, [](JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
                               size_t argumentCount, const JSValueRef arguments[],
                               JSValueRef* exception) -> JSValueRef
          {
            NX::AbstractTask * task = reinterpret_cast<NX::AbstractTask *>(JSObjectGetPrivate(thisObject));
            if (task) {
              if (task->status() == NX::AbstractTask::ABORTED)
                return JSValueMakeBoolean(ctx, false);
              else {
                task->abort();
                return JSValueMakeBoolean(ctx, task->status() == NX::AbstractTask::ABORTED);
              }
            } else {
              NX::Value message(ctx, "task expired");
              JSValueRef args[] { message.value(), nullptr };
              *exception = JSObjectMakeError(ctx, 1, args, nullptr);
            }
            return JSValueMakeBoolean(ctx, false);
          }), kJSPropertyAttributeNone, nullptr);
          JSValueProtect(context->toJSContext(), taskObject);
          taskPtr->setCancellationHandler([=]() {
            if (JSObjectGetPrivate(taskObject)) {
              JSObjectSetPrivate(taskObject, nullptr);
              JSValueUnprotect(context->toJSContext(), taskObject);
            }
          });
          taskPtr->setFinishHandler([=]() {
            if (JSObjectGetPrivate(taskObject)) {
              JSObjectSetPrivate(taskObject, nullptr);
              JSValueUnprotect(context->toJSContext(), taskObject);
            }
          });
          return taskObject;*/
//           return JSValueMakeUndefined(ctx);
//         }
      } else {
        if (!*exception) {
          NX::Value message(ctx, "invalid argument passed to scheduler");
          JSValueRef args[] { message.value(), nullptr };
          *exception = JSObjectMakeError(ctx, 1, args, nullptr);
        }
      }
      return JSValueMakeUndefined(ctx);
    }, 0
  },
  //     { "yield", [](JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
  //       size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception) -> JSValueRef {
  //         NX::Scheduler * scheduler = reinterpret_cast<NX::Scheduler*>(JSObjectGetPrivate(thisObject));
  //         JSGlobalContextRef globalContext = JSContextGetGlobalContext(ctx);
  //         JSValueProtect(globalContext, function);
  //         JSValueProtect(globalContext, thisObject);
  //         try {
  //           scheduler->yield();
  //         } catch(const std::exception & e) {
  //           NX::Value message(ctx, e.what());
  //           JSValueRef args[] { message.value(), nullptr };
  //           *exception = JSObjectMakeError(ctx, 1, args, nullptr);
  //         }
  //         JSValueUnprotect(globalContext, function);
  //         JSValueUnprotect(globalContext, thisObject);
  //         return JSValueMakeUndefined(ctx);
  //       }, 0
  //     },
  { nullptr, nullptr, 0 }
};

