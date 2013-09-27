// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

/*
  CppBoundClass class:
  This base class serves as a parent for C++ classes designed to be bound to
  JavaScript objects.

  Subclasses should define the constructor to build the property and method
  lists needed to bind this class to a JS object.  They should also declare
  and define member variables and methods to be exposed to JS through
  that object.

  See cpp_binding_example.{h|cc} for an example.
*/

#ifndef WEBKIT_RENDERER_CPP_BOUND_CLASS_H_
#define WEBKIT_RENDERER_CPP_BOUND_CLASS_H_

#include <map>
#include <vector>

#include "base/callback.h"
#include "webkit/renderer/cpp_variant.h"
#include "webkit/renderer/webkit_renderer_export.h"

namespace WebKit {
class WebFrame;
}

namespace webkit_glue {

typedef std::vector<CppVariant> CppArgumentList;

// CppBoundClass lets you map Javascript method calls and property accesses
// directly to C++ method calls and CppVariant* variable access.
class WEBKIT_RENDERER_EXPORT CppBoundClass {
 public:
  class IndexOperatorCallback {
   public:
    virtual ~IndexOperatorCallback() { }

    // Sets |value| to the value of this object on following |index|.
    // Return false in case of failure. |value| is always non NULL.
    virtual bool GetValue(int32_t index, CppVariant* value) = 0;

    // Sets the value of this object on the requested |index| to |value|.
    // Returns false in case of failure.
    virtual bool SetValue(int32_t index, const CppVariant& value) = 0;
  };

  class PropertyCallback {
   public:
    virtual ~PropertyCallback() { }

    // Sets |value| to the value of the property. Returns false in case of
    // failure. |value| is always non-NULL.
    virtual bool GetValue(CppVariant* value) = 0;

    // sets the property value to |value|. Returns false in case of failure.
    virtual bool SetValue(const CppVariant& value) = 0;
  };

  // Get the CppBoundClass that is behind the given |object| NPObject.
  // Returns NULL if |object| is not implemented by CppBoundClass.
  static CppBoundClass* GetForNPObject(NPObject* object);

  // The constructor should call BindCallback, BindProperty, and
  // BindFallbackCallback as needed to set up the methods, properties, and
  // fallback method.
  CppBoundClass();
  virtual ~CppBoundClass();

  // Return a CppVariant representing this class, for use with BindProperty().
  // The variant type is guaranteed to be NPVariantType_Object.
  CppVariant* GetAsCppVariant();

  // Return a CppVariant containing an NPObject which owns this object.
  // The returned object has retain count = 1 and when its reference counting
  // reaches zero this CppBoundClass will also be freed.
  // The variant type is guaranteed to be NPVariantType_Object.
  CppVariant WrapInOwningCppVariant();

  // Given a WebFrame, BindToJavascript builds the NPObject that will represent
  // this CppBoundClass object and binds it to the frame's window under the
  // given name.  This should generally be called from the WebView delegate's
  // WindowObjectCleared(). This CppBoundClass object will be accessible to
  // JavaScript as window.<classname>. The owner of this CppBoundClass object is
  // responsible for keeping it around while the frame is alive, and for
  // destroying it afterwards.
  void BindToJavascript(WebKit::WebFrame* frame, const std::string& classname);

  // The type of callbacks.
  typedef base::Callback<void(const CppArgumentList&, CppVariant*)> Callback;
  typedef base::Callback<void(CppVariant*)> GetterCallback;
  typedef base::Callback<void(int32_t, CppVariant*)> IndexGetterCallback;

  // Used by a test.  Returns true if a method with name |name| exists,
  // regardless of whether a fallback is registered.
  bool IsMethodRegistered(const std::string& name) const;

 protected:
  // Bind the Javascript method called |name| to the C++ callback |callback|.
  void BindCallback(const std::string& name, const Callback& callback);

  // Bind Javascript property |name| to the C++ getter callback |callback|.
  // This can be used to create read-only properties.
  void BindGetterCallback(const std::string& name,
                          const GetterCallback& callback);

  // Bind array access operator [] of this Javascript object to the C++ getter
  // callback |callback|. This can be used for read-only access.
  void BindIndexGetterCallback(const IndexGetterCallback& callback);

  // Bind array access opeartor [] of this Javascript object to C++ callback
  // |callback|.
  void BindIndexOperator(IndexOperatorCallback* callback);

  // Bind the Javascript property called |name| to a CppVariant |prop|.
  void BindProperty(const std::string& name, CppVariant* prop);

  // Bind Javascript property called |name| to a PropertyCallback |callback|.
  // CppBoundClass assumes control over the life time of the |callback|.
  void BindProperty(const std::string& name, PropertyCallback* callback);

  // Set the fallback callback, which is called when when a callback is
  // invoked that isn't bound.
  // If it is NULL (its default value), a JavaScript exception is thrown in
  // that case (as normally expected). If non NULL, the fallback method is
  // invoked and the script continues its execution.
  // Passing NULL for |callback| clears out any existing binding.
  // It is used for tests and should probably only be used in such cases
  // as it may cause unexpected behaviors (a JavaScript object with a
  // fallback always returns true when checked for a method's
  // existence).
  void BindFallbackCallback(const Callback& fallback_callback) {
    fallback_callback_ = fallback_callback;
  }

  // Some fields are protected because some tests depend on accessing them,
  // but otherwise they should be considered private.

  typedef std::map<NPIdentifier, PropertyCallback*> PropertyList;
  typedef std::map<NPIdentifier, Callback> MethodList;
  // These maps associate names with property and method pointers to be
  // exposed to JavaScript.
  PropertyList properties_;
  MethodList methods_;
  scoped_ptr<IndexOperatorCallback> index_callback_;

  // The callback gets invoked when a call is made to an nonexistent method.
  Callback fallback_callback_;

 private:
  // NPObject callbacks.
  friend struct CppNPObject;
  bool HasMethod(NPIdentifier ident) const;
  bool Invoke(NPIdentifier ident, const NPVariant* args, size_t arg_count,
              NPVariant* result);
  bool HasProperty(NPIdentifier ident) const;
  bool GetProperty(NPIdentifier ident, NPVariant* result) const;
  bool SetProperty(NPIdentifier ident, const NPVariant* value);

  // A lazily-initialized CppVariant representing this class.  We retain 1
  // reference to this object, and it is released on deletion.
  CppVariant self_variant_;

  // TODO(wez): Remove once crrev.com/14019005 lands.
  bool bound_to_frame_;

  // Dummy NPP to use to register as owner for NPObjects.
  scoped_ptr<NPP_t> npp_;

  DISALLOW_COPY_AND_ASSIGN(CppBoundClass);
};

}  // namespace webkit_glue

#endif  // WEBKIT_RENDERER_CPP_BOUND_CLASS_H_
