//===--- SerializedSILLoader.h - Handle SIL section in modules --*- c++ -*-===//
//
// This source file is part of the Swift.org open source project
//
// Copyright (c) 2014 - 2015 Apple Inc. and the Swift project authors
// Licensed under Apache License v2.0 with Runtime Library Exception
//
// See http://swift.org/LICENSE.txt for license information
// See http://swift.org/CONTRIBUTORS.txt for the list of Swift project authors
//
//===----------------------------------------------------------------------===//

#ifndef SWIFT_SERIALIZATION_SILLOADER_H
#define SWIFT_SERIALIZATION_SILLOADER_H

namespace swift {
class ASTContext;
class Module;
class SILDeserializer;
class SILFunction;
class SILGlobalVariable;
class SILModule;
class SILVTable;
class SILWitnessTable;

/// Maintains a list of SILDeserializer, one for each serialized modules
/// in ASTContext. It provides lookupSILFunction that will perform lookup
/// on each SILDeserializer.
class SerializedSILLoader {
public:
  class Callback {
  public:
    /// Observe that we deserialized a function declaration.
    virtual void didDeserialize(Module *M, SILFunction *fn) {}

    /// Observe that we successfully deserialized a function body.
    virtual void didDeserializeFunctionBody(Module *M, SILFunction *fn) {}

    /// Oberve that we successfully deserialized a witness table's entries.
    virtual void didDeserializeWitnessTableEntries(Module *M,
                                                   SILWitnessTable *wt) {}

    /// Observe that we deserialized a global variable declaration.
    virtual void didDeserialize(Module *M, SILGlobalVariable *var) {}

    /// Observe that we deserialized a v-table declaration.
    virtual void didDeserialize(Module *M, SILVTable *vtable) {}

    /// Observe that we deserialized a witness-table declaration.
    virtual void didDeserialize(Module *M, SILWitnessTable *wtable) {}

    virtual ~Callback() = default;
  private:
    virtual void _anchor();
  };

private:
  std::vector<std::unique_ptr<SILDeserializer> > LoadedSILSections;

  explicit SerializedSILLoader(ASTContext &ctx, SILModule *SILMod,
                               Callback *callback);

public:
  /// Create a new loader.
  ///
  /// \param callback - not owned by the loader
  static std::unique_ptr<SerializedSILLoader> create(ASTContext &ctx,
                                                     SILModule *SILMod,
                                                     Callback *callback) {
    return std::unique_ptr<SerializedSILLoader>(
      new SerializedSILLoader(ctx, SILMod, callback));
  }
  ~SerializedSILLoader();

  SILFunction *lookupSILFunction(SILFunction *Callee);
  SILVTable *lookupVTable(Identifier Name);
  SILVTable *lookupVTable(const ClassDecl *C) {
    return lookupVTable(C->getName());
  }
  SILWitnessTable *lookupWitnessTable(SILWitnessTable *C);

  /// Invalidate the cached entry for SILFunction.
  void invalidateEntry(SILFunction *fn);

  /// Deserialize all SILFunctions, VTables, and WitnessTables in all
  /// SILModules.
  void getAll();

  /// Deserialize all SILFunctions in all SILModules.
  void getAllSILFunctions();

  /// Deserialize all VTables in all SILModules.
  void getAllVTables();

  /// Deserialize all WitnessTables in all SILModules.
  void getAllWitnessTables();

  SerializedSILLoader(const SerializedSILLoader &) = delete;
  SerializedSILLoader(SerializedSILLoader &&) = delete;
  SerializedSILLoader &operator=(const SerializedSILLoader &) = delete;
  SerializedSILLoader &operator=(SerializedSILLoader &&) = delete;
};

} // end namespace swift

#endif
