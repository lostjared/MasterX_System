/**
 * @file icode_visit.cpp
 * @brief Pascal code generator AST visitor method implementations
 * @author Jared Bruni
 */
#include "icode.hpp"

namespace pascal {

    CodeGenVisitor::CodeGenVisitor()
        : regInUse(registers.size(), false),
          ptrRegInUse(ptrRegisters.size(), false) {
        initializeFloatRegisters();
        initializeBuiltins();
    }

    void CodeGenVisitor::visit(ProgramNode &node) {
        name = node.name;
        // Separate native modules from unit dependencies
        static const std::unordered_set<std::string> nativeModules = {"io", "std", "string", "sdl", "strlib"};
        for (const auto &mod : node.uses) {
            if (mod == "strlib")
                usedModules.insert("string");
            else if (nativeModules.count(mod))
                usedModules.insert(mod);
            else
                objectDeps.push_back(mod);
        }
        if (node.block) {
            node.block->accept(*this);
        }
    }

    void CodeGenVisitor::visit(UnitNode &node) {
        name = node.name;
        isUnit = true;
        // Process uses clause
        static const std::unordered_set<std::string> nativeModules = {"io", "std", "string", "sdl", "strlib"};
        for (const auto &mod : node.uses) {
            if (mod == "strlib")
                usedModules.insert("string");
            else if (nativeModules.count(mod))
                usedModules.insert(mod);
            else
                objectDeps.push_back(mod);
        }

        // Emit UNIT_INIT function label — allocs from interface vars go here
        emitLabel("function PROC_UNIT_INIT");

        // Register interface declarations' signatures WITHOUT deferring code
        // (interface declarations have null blocks — they are forward declarations)
        for (auto &decl : node.interfaceDecls) {
            if (!decl)
                continue;
            if (auto *funcDecl = dynamic_cast<FuncDeclNode *>(decl.get())) {
                // Just register the signature info; don't call accept()
                FuncInfo funcInfo;
                funcInfo.returnType = getTypeFromString(funcDecl->returnType);
                for (auto &p : funcDecl->parameters) {
                    if (auto pn = dynamic_cast<ParameterNode *>(p.get())) {
                        for (size_t i = 0; i < pn->identifiers.size(); ++i) {
                            funcInfo.paramTypes.push_back(getTypeFromString(pn->type));
                        }
                    }
                }
                funcSignatures[funcDecl->name] = funcInfo;
                setVarType(funcDecl->name, funcInfo.returnType);
            } else if (dynamic_cast<ConstDeclNode *>(decl.get()) ||
                       dynamic_cast<VarDeclNode *>(decl.get()) ||
                       dynamic_cast<TypeDeclNode *>(decl.get())) {
                // Process interface const/var/type so they are available
                // to the unit's own implementation code
                decl->accept(*this);
            }
            // ProcDeclNode forward declarations don't need signature registration
            // (they have no return type to track)
        }

        // Close UNIT_INIT function
        emit("ret");

        // Process implementation declarations (actual code)
        for (auto &decl : node.implDecls) {
            if (decl)
                decl->accept(*this);
        }
    }

    void CodeGenVisitor::visit(BlockNode &node) {
        for (auto &decl : node.declarations) {
            if (!decl)
                continue;
            if (dynamic_cast<TypeDeclNode *>(decl.get())) {
                decl->accept(*this);
            }
        }

        for (auto &decl : node.declarations) {
            if (!decl)
                continue;
            if (dynamic_cast<TypeDeclNode *>(decl.get()))
                continue;

            if (generatingDeferredCode) {
                if (dynamic_cast<ProcDeclNode *>(decl.get()) ||
                    dynamic_cast<FuncDeclNode *>(decl.get())) {
                    continue;
                }
            }
            decl->accept(*this);
        }

        if (node.compoundStatement) {
            node.compoundStatement->accept(*this);
        }
    }

    void CodeGenVisitor::visit(VarDeclNode &node) {
        for (const auto &varName : node.identifiers) {
            std::string mangledName = mangleVariableName(varName);
            std::string typeName;

            if (std::holds_alternative<std::string>(node.type)) {
                typeName = std::get<std::string>(node.type);
            } else if (std::holds_alternative<std::unique_ptr<ASTNode>>(node.type)) {
                auto &typeNode = std::get<std::unique_ptr<ASTNode>>(node.type);

                if (auto *arrayTypeNode = dynamic_cast<ArrayTypeNode *>(typeNode.get())) {
                    ArrayInfo info = buildArrayInfoFromNode(arrayTypeNode);
                    arrayInfo[mangledName] = std::move(info);

                    setVarType(varName, VarType::PTR);
                    int slot = newSlotFor(mangledName);
                    setSlotType(slot, VarType::PTR);
                    varSlot[varName] = slot;
                    varSlot[mangledName] = slot;

                    updateDataSectionInitialValue(slotVar(slot), "ptr", "null");

                    if (arrayInfo[mangledName].isDynamic) {
                        // Dynamic array: no alloc at declaration, starts as null
                        // Create companion length variable
                        std::string lenName = mangledName + "_dynlen";
                        int lenSlot = newSlotFor(lenName);
                        setSlotType(lenSlot, VarType::INT);
                        varSlot[lenName] = lenSlot;
                        updateDataSectionInitialValue(slotVar(lenSlot), "int", "0");
                        dynArrayLenSlot[mangledName] = lenSlot;

                        std::string currentScope = getCurrentScopeName();
                        if (currentScope.empty())
                            globalArrays.push_back(slotVar(slot));
                        else
                            functionScopedArrays[currentScope].push_back(slotVar(slot));
                    } else {
                        emit3("alloc",
                              slotVar(slot),
                              std::to_string(arrayInfo[mangledName].elementSize),
                              std::to_string(arrayInfo[mangledName].size));

                        std::string currentScope = getCurrentScopeName();
                        if (currentScope.empty())
                            globalArrays.push_back(slotVar(slot));
                        else
                            functionScopedArrays[currentScope].push_back(slotVar(slot));
                    }
                    continue;
                }

                if (auto *ptrTypeNode = dynamic_cast<PointerTypeNode *>(typeNode.get())) {
                    setVarType(varName, VarType::PTR);
                    int slot = newSlotFor(mangledName);
                    setSlotType(slot, VarType::PTR);
                    varSlot[varName] = slot;
                    varSlot[mangledName] = slot;
                    pointerBaseType[mangledName] = ptrTypeNode->baseTypeName;
                    pointerBaseType[varName] = ptrTypeNode->baseTypeName;
                    updateDataSectionInitialValue(slotVar(slot), "ptr", "null");
                    continue;
                }

                if (dynamic_cast<SetTypeNode *>(typeNode.get())) {
                    setVarType(varName, VarType::PTR);
                    int slot = newSlotFor(mangledName);
                    setSlotType(slot, VarType::PTR);
                    varSlot[varName] = slot;
                    varSlot[mangledName] = slot;
                    setVars.insert(mangledName);
                    setVars.insert(varName);
                    updateDataSectionInitialValue(slotVar(slot), "ptr", "null");
                    // Allocate 256 bytes (one byte per ordinal 0..255)
                    emit3("alloc", slotVar(slot), "8", "256");
                    // Zero-fill the set
                    std::string loopIdx = allocReg();
                    std::string loopStart = newLabel("SET_INIT");
                    std::string loopEnd = newLabel("SET_INIT_END");
                    emit2("mov", loopIdx, "0");
                    emitLabel(loopStart);
                    emit2("cmp", loopIdx, "256");
                    emit1("jge", loopEnd);
                    emit4("store", "0", slotVar(slot), loopIdx, "8");
                    emit2("add", loopIdx, "1");
                    emit1("jmp", loopStart);
                    emitLabel(loopEnd);
                    freeReg(loopIdx);

                    std::string currentScope = getCurrentScopeName();
                    if (currentScope.empty())
                        globalArrays.push_back(slotVar(slot));
                    else
                        functionScopedArrays[currentScope].push_back(slotVar(slot));
                    continue;
                }

                if (auto *simpleTypeNode = dynamic_cast<SimpleTypeNode *>(typeNode.get())) {
                    typeName = simpleTypeNode->typeName;
                } else if (dynamic_cast<RecordTypeNode *>(typeNode.get())) {
                    typeName = "record";
                }
            } else {
                typeName = "unknown";
            }

            int slot = newSlotFor(mangledName);
            varSlot[varName] = slot;
            varSlot[mangledName] = slot;

            std::string normalizedTypeName = resolveTypeName(lc(typeName));

            // Handle set type aliases (e.g., type MySet = set of integer; var s: MySet;)
            if (normalizedTypeName.rfind("set of ", 0) == 0) {
                setVarType(varName, VarType::PTR);
                setSlotType(slot, VarType::PTR);
                setVars.insert(mangledName);
                setVars.insert(varName);
                updateDataSectionInitialValue(slotVar(slot), "ptr", "null");
                emit3("alloc", slotVar(slot), "8", "256");
                // Zero-fill the set
                std::string loopIdx = allocReg();
                std::string loopStart = newLabel("SET_INIT");
                std::string loopEnd = newLabel("SET_INIT_END");
                emit2("mov", loopIdx, "0");
                emitLabel(loopStart);
                emit2("cmp", loopIdx, "256");
                emit1("jge", loopEnd);
                emit4("store", "0", slotVar(slot), loopIdx, "8");
                emit2("add", loopIdx, "1");
                emit1("jmp", loopStart);
                emitLabel(loopEnd);
                freeReg(loopIdx);
                std::string currentScope = getCurrentScopeName();
                if (currentScope.empty())
                    globalArrays.push_back(slotVar(slot));
                else
                    functionScopedArrays[currentScope].push_back(slotVar(slot));
                continue;
            }

            // Handle file/text type variables
            if (normalizedTypeName == "file" || normalizedTypeName == "text") {
                setVarType(varName, VarType::PTR);
                setSlotType(slot, VarType::PTR);
                fileVars.insert(mangledName);
                fileVars.insert(varName);
                updateDataSectionInitialValue(slotVar(slot), "ptr", "null");
                // Create companion filename variable
                std::string fileNameVar = mangledName + "_filename";
                int fnSlot = newSlotFor(fileNameVar);
                setSlotType(fnSlot, VarType::PTR);
                setVarType(fileNameVar, VarType::PTR);
                varSlot[fileNameVar] = fnSlot;
                fileVarNames[mangledName] = fileNameVar;
                fileVarNames[varName] = fileNameVar;
                continue;
            }

            auto recordTypeIt = recordTypes.find(normalizedTypeName);
            if (recordTypeIt != recordTypes.end()) {
                varRecordType[mangledName] = normalizedTypeName;
                varRecordType[varName] = normalizedTypeName;

                int recordSize = recordTypeIt->second.size;

                setVarType(varName, VarType::RECORD);
                setSlotType(slot, VarType::PTR);

                updateDataSectionInitialValue(slotVar(slot), "ptr", "null");
                emit3("alloc", slotVar(slot), std::to_string(recordSize), "1");

                allocateRecordFieldArrays(mangledName, normalizedTypeName);

                std::string currentScope = getCurrentScopeName();
                recordsToFreeInScope[currentScope].push_back(mangledName);
                continue;
            }

            auto aliasIt = arrayInfo.find(normalizedTypeName);
            if (aliasIt != arrayInfo.end()) {
                arrayInfo[mangledName] = aliasIt->second;

                setVarType(varName, VarType::PTR);
                setSlotType(slot, VarType::PTR);

                updateDataSectionInitialValue(slotVar(slot), "ptr", "null");
                emit3("alloc",
                      slotVar(slot),
                      std::to_string(arrayInfo[mangledName].elementSize),
                      std::to_string(arrayInfo[mangledName].size));

                std::string currentScope = getCurrentScopeName();
                if (currentScope.empty())
                    globalArrays.push_back(slotVar(slot));
                else
                    functionScopedArrays[currentScope].push_back(slotVar(slot));
                continue;
            }

            if (!normalizedTypeName.empty() && normalizedTypeName[0] == '^') {
                std::string baseType = normalizedTypeName.substr(1);
                setVarType(varName, VarType::PTR);
                setSlotType(slot, VarType::PTR);
                pointerBaseType[mangledName] = baseType;
                pointerBaseType[varName] = baseType;
                updateDataSectionInitialValue(slotVar(slot), "ptr", "null");
                continue;
            }

            VarType vType = getTypeFromString(typeName);
            setVarType(varName, vType);
            setSlotType(slot, vType);

            if (!currentFunctionName.empty())
                currentFuncLocalSlots.push_back(slotVar(slot));

            if (vType == VarType::PTR || vType == VarType::RECORD) {
                updateDataSectionInitialValue(slotVar(slot), "ptr", "null");
            } else if (vType == VarType::DOUBLE) {
                updateDataSectionInitialValue(slotVar(slot), "float", "0.0");
            } else {
                updateDataSectionInitialValue(slotVar(slot), "int", "0");
            }
        }
    }

    void CodeGenVisitor::visit(ProcCallNode &node) {
        std::string procLower = node.name;
        std::transform(procLower.begin(), procLower.end(), procLower.begin(),
                       [](unsigned char c) { return std::tolower(c); });

        if (procLower == "new") {
            if (node.arguments.size() != 1)
                throw std::runtime_error("new() requires exactly one pointer argument");

            if (auto *fieldNode = dynamic_cast<FieldAccessNode *>(node.arguments[0].get())) {
                std::string baseName;
                std::string recType;
                bool baseIsDirectPointer = false;

                if (auto *v = dynamic_cast<VariableNode *>(fieldNode->recordExpr.get())) {
                    baseName = findMangledName(v->name);
                    recType = getVarRecordTypeName(v->name);
                } else {
                    fieldNode->recordExpr->accept(*this);
                    baseName = popValue();
                    baseIsDirectPointer = true;
                    recType = getVarRecordTypeNameFromExpr(fieldNode->recordExpr.get());
                }

                recType = resolveTypeName(lc(recType));
                auto recTypeIt = recordTypes.find(recType);
                if (recTypeIt == recordTypes.end())
                    throw std::runtime_error("new(): unknown record type: " + recType);

                const std::string fieldName = lc(fieldNode->fieldName);
                auto it = recTypeIt->second.nameToIndex.find(fieldName);
                if (it == recTypeIt->second.nameToIndex.end())
                    throw std::runtime_error("new(): field not found: " + fieldName);

                const auto &fieldInfo = recTypeIt->second.fields[it->second];
                const int byteOffset = fieldInfo.offset;

                std::string fieldTypeName = fieldInfo.typeName;
                int elemSize = 8;
                if (!fieldTypeName.empty() && fieldTypeName[0] == '^') {
                    std::string baseType = resolveTypeName(lc(fieldTypeName.substr(1)));
                    elemSize = getTypeSizeByName(baseType);
                    if (isRecordTypeName(baseType)) {
                        auto rit = recordTypes.find(baseType);
                        if (rit != recordTypes.end())
                            elemSize = rit->second.size;
                    }
                }

                std::string tmp = allocTempPtr();
                emit3("alloc", tmp, std::to_string(elemSize), "1");

                std::string basePtr = baseIsDirectPointer ? baseName : ensurePtrBase(baseName);
                emit4("store", tmp, basePtr, std::to_string(byteOffset), "1");

                if (isReg(tmp) && !isParmReg(tmp))
                    freeReg(tmp);
                if (baseIsDirectPointer && isReg(basePtr) && !isParmReg(basePtr))
                    freeReg(basePtr);
                return;
            }

            if (auto *arrNode = dynamic_cast<ArrayAccessNode *>(node.arguments[0].get())) {
                ArrayInfo *info = getArrayInfoForArrayAccess(arrNode);
                if (!info)
                    throw std::runtime_error("new(): unknown array: " + getArrayNameFromBase(arrNode->base.get()));

                int elemSize = 8;
                std::string et = info->elementType;
                if (!et.empty() && et[0] == '^') {
                    std::string baseType = resolveTypeName(lc(et.substr(1)));
                    elemSize = getTypeSizeByName(baseType);
                    if (isRecordTypeName(baseType)) {
                        auto rit = recordTypes.find(baseType);
                        if (rit != recordTypes.end())
                            elemSize = rit->second.size;
                    }
                }

                std::string idx = eval(arrNode->index.get());
                if (getExpressionType(arrNode->index.get()) == VarType::DOUBLE) {
                    std::string intIdx = allocReg();
                    emit2("mov", intIdx, idx);
                    if (isReg(idx) && !isParmReg(idx))
                        freeReg(idx);
                    idx = intIdx;
                }

                std::string elemIndex;
                if (isReg(idx) && !isParmReg(idx)) {
                    elemIndex = idx;
                } else {
                    elemIndex = allocReg();
                    emit2("mov", elemIndex, idx);
                }
                if (info->lowerBound != 0)
                    emit2("sub", elemIndex, std::to_string(info->lowerBound));

                std::string base;
                if (auto var = dynamic_cast<VariableNode *>(arrNode->base.get())) {
                    std::string mangled = findMangledArrayName(var->name);
                    base = ensurePtrBase(storageSymbolFor(mangled));
                } else {
                    arrNode->base->accept(*this);
                    base = popValue();
                }

                std::string tmp = allocTempPtr();
                emit3("alloc", tmp, std::to_string(elemSize), "1");
                emit4("store", tmp, base, elemIndex, std::to_string(info->elementSize));

                if (isReg(tmp) && !isParmReg(tmp))
                    freeReg(tmp);
                if (elemIndex != idx && isReg(idx) && !isParmReg(idx))
                    freeReg(idx);
                freeReg(elemIndex);
                return;
            }

            auto *varNode = dynamic_cast<VariableNode *>(node.arguments[0].get());
            if (!varNode)
                throw std::runtime_error("new() argument must be a variable, record field, or array element");

            std::string mangled = findMangledName(varNode->name);
            std::string sym = storageSymbolFor(mangled);
            int elemSize = getPointerElementSize(varNode->name);

            std::string baseType = getPointerBaseTypeName(varNode->name);
            baseType = resolveTypeName(lc(baseType));
            int allocCount = 1;
            if (isRecordTypeName(baseType)) {
                auto it = recordTypes.find(baseType);
                if (it != recordTypes.end()) {
                    elemSize = it->second.size;
                }
            }

            emit3("alloc", sym, std::to_string(elemSize), std::to_string(allocCount));
            return;
        }

        if (procLower == "dispose") {
            if (node.arguments.size() != 1)
                throw std::runtime_error("dispose() requires exactly one pointer argument");

            if (auto *fieldNode = dynamic_cast<FieldAccessNode *>(node.arguments[0].get())) {
                std::string baseName;
                std::string recType;
                bool baseIsDirectPointer = false;

                if (auto *v = dynamic_cast<VariableNode *>(fieldNode->recordExpr.get())) {
                    baseName = findMangledName(v->name);
                    recType = getVarRecordTypeName(v->name);
                } else {
                    fieldNode->recordExpr->accept(*this);
                    baseName = popValue();
                    baseIsDirectPointer = true;
                    recType = getVarRecordTypeNameFromExpr(fieldNode->recordExpr.get());
                }

                recType = resolveTypeName(lc(recType));
                auto recTypeIt = recordTypes.find(recType);
                if (recTypeIt == recordTypes.end())
                    throw std::runtime_error("dispose(): unknown record type: " + recType);

                const std::string fieldName = lc(fieldNode->fieldName);
                auto it = recTypeIt->second.nameToIndex.find(fieldName);
                if (it == recTypeIt->second.nameToIndex.end())
                    throw std::runtime_error("dispose(): field not found: " + fieldName);

                const auto &fieldInfo = recTypeIt->second.fields[it->second];
                const int byteOffset = fieldInfo.offset;

                std::string basePtr = baseIsDirectPointer ? baseName : ensurePtrBase(baseName);
                std::string tmp = allocTempPtr();
                emit4("load", tmp, basePtr, std::to_string(byteOffset), "1");
                emit1("free", tmp);

                if (isReg(tmp) && !isParmReg(tmp))
                    freeReg(tmp);
                if (baseIsDirectPointer && isReg(basePtr) && !isParmReg(basePtr))
                    freeReg(basePtr);
                return;
            }

            if (auto *arrNode = dynamic_cast<ArrayAccessNode *>(node.arguments[0].get())) {
                ArrayInfo *info = getArrayInfoForArrayAccess(arrNode);
                if (!info)
                    throw std::runtime_error("dispose(): unknown array: " + getArrayNameFromBase(arrNode->base.get()));

                std::string idx = eval(arrNode->index.get());
                if (getExpressionType(arrNode->index.get()) == VarType::DOUBLE) {
                    std::string intIdx = allocReg();
                    emit2("mov", intIdx, idx);
                    if (isReg(idx) && !isParmReg(idx))
                        freeReg(idx);
                    idx = intIdx;
                }

                std::string elemIndex;
                if (isReg(idx) && !isParmReg(idx)) {
                    elemIndex = idx;
                } else {
                    elemIndex = allocReg();
                    emit2("mov", elemIndex, idx);
                }
                if (info->lowerBound != 0)
                    emit2("sub", elemIndex, std::to_string(info->lowerBound));

                std::string base;
                if (auto var = dynamic_cast<VariableNode *>(arrNode->base.get())) {
                    std::string mangled = findMangledArrayName(var->name);
                    base = ensurePtrBase(storageSymbolFor(mangled));
                } else {
                    arrNode->base->accept(*this);
                    base = popValue();
                }

                std::string tmp = allocTempPtr();
                emit4("load", tmp, base, elemIndex, std::to_string(info->elementSize));
                emit1("free", tmp);

                if (isReg(tmp) && !isParmReg(tmp))
                    freeReg(tmp);
                if (elemIndex != idx && isReg(idx) && !isParmReg(idx))
                    freeReg(idx);
                freeReg(elemIndex);
                return;
            }

            auto *varNode = dynamic_cast<VariableNode *>(node.arguments[0].get());
            if (!varNode)
                throw std::runtime_error("dispose() argument must be a variable, record field, or array element");

            std::string mangled = findMangledName(varNode->name);
            std::string sym = storageSymbolFor(mangled);
            emit1("free", sym);
            return;
        }

        // SetLength(arr, newSize) — resize a dynamic array
        if (procLower == "setlength" && node.arguments.size() == 2) {
                auto *varNode = dynamic_cast<VariableNode *>(node.arguments[0].get());
                if (!varNode)
                    throw std::runtime_error("SetLength: first argument must be a dynamic array variable");

                std::string mangled = findMangledArrayName(varNode->name);
                auto it = arrayInfo.find(mangled);
                if (it == arrayInfo.end())
                    it = arrayInfo.find(varNode->name);
                if (it == arrayInfo.end() || !it->second.isDynamic)
                    throw std::runtime_error("SetLength: " + varNode->name + " is not a dynamic array");

                std::string arrMangled = it->first;
                std::string sym = storageSymbolFor(arrMangled);

                // Evaluate the new size
                std::string newSize = eval(node.arguments[1].get());

                // Emit realloc: realloc arrPtr, elementSize, newCount
                emit3("realloc", sym, std::to_string(it->second.elementSize), newSize);

                // Update companion length variable
                auto lenIt = dynArrayLenSlot.find(arrMangled);
                if (lenIt != dynArrayLenSlot.end()) {
                    emit2("mov", slotVar(lenIt->second), newSize);
                }

                if (isReg(newSize) && !isParmReg(newSize))
                    freeReg(newSize);
                return;
            }

        auto handler = builtinRegistry.findHandler(procLower);
        if (handler) {
            handler->generate(*this, procLower, node.arguments);
            return;
        }

        std::vector<std::string> evaluated_args;
        std::vector<VarType> argTypes;

        for (auto &arg : node.arguments) {
            std::string argVal = eval(arg.get());
            evaluated_args.push_back(argVal);
            argTypes.push_back(getExpressionType(arg.get()));
        }

        // Build target register list
        std::vector<std::string> targetRegs(evaluated_args.size());
        size_t intRegIdx = 1;
        size_t ptrRegIdx = 0;
        size_t floatRegIdx = 0;

        for (size_t i = 0; i < evaluated_args.size(); ++i) {
            if (argTypes[i] == VarType::STRING || argTypes[i] == VarType::PTR || argTypes[i] == VarType::RECORD) {
                if (ptrRegIdx < ptrRegisters.size())
                    targetRegs[i] = ptrRegisters[ptrRegIdx++];
            } else if (argTypes[i] == VarType::DOUBLE) {
                if (floatRegIdx < floatRegisters.size())
                    targetRegs[i] = floatRegisters[floatRegIdx++];
            } else {
                if (intRegIdx < registers.size())
                    targetRegs[i] = registers[intRegIdx++];
            }
        }

        // Spill any evaluated arg that sits in another arg's target register
        for (size_t i = 0; i < evaluated_args.size(); ++i) {
            if (evaluated_args[i].empty() || targetRegs[i].empty())
                continue;
            if (evaluated_args[i] == targetRegs[i])
                continue;
            for (size_t j = 0; j < evaluated_args.size(); ++j) {
                if (j == i)
                    continue;
                if (evaluated_args[j] == targetRegs[i]) {
                    // arg j's value is in a register that arg i needs as target
                    std::string spill = allocReg();
                    emit2("mov", spill, evaluated_args[j]);
                    evaluated_args[j] = spill;
                }
            }
        }

        // Save caller's in-use registers that could be clobbered by this call
        std::vector<std::string> savedRegs;
        for (size_t i = 1; i < regInUse.size() && i < registers.size(); ++i) {
            if (!regInUse[i]) continue;
            bool isEvalArg = false;
            for (const auto &ea : evaluated_args) {
                if (ea == registers[i]) { isEvalArg = true; break; }
            }
            if (isEvalArg) continue;
            savedRegs.push_back(registers[i]);
        }
        for (const auto &sr : savedRegs)
            emit1("push", sr);

        // Now move into target registers
        {
            auto eit = externalFuncs.find(node.name);
            std::string objPrefix = (eit != externalFuncs.end()) ? eit->second + "." : "";
            for (size_t i = 0; i < evaluated_args.size(); ++i) {
                if (!targetRegs[i].empty())
                    emit2("mov", objPrefix + targetRegs[i], evaluated_args[i]);
            }
        }

        std::string mangledName = findMangledFuncName(node.name, true);
        {
            std::string prefix = "PROC_";
            if (funcSignatures.count(node.name))
                prefix = "FUNC_";
            std::string label = prefix + mangledName;
            auto eit = externalFuncs.find(node.name);
            if (eit != externalFuncs.end())
                label = eit->second + "." + label;

            if (!currentFuncLocalSlots.empty()) {
                for (const auto &slot : currentFuncLocalSlots)
                    emit1("push", slot);
            }

            emit1("call", label);

            if (!currentFuncLocalSlots.empty()) {
                for (auto it2 = currentFuncLocalSlots.rbegin(); it2 != currentFuncLocalSlots.rend(); ++it2)
                    emit1("pop", *it2);
            }
        }

        // Restore caller's saved registers
        for (auto rit = savedRegs.rbegin(); rit != savedRegs.rend(); ++rit)
            emit1("pop", *rit);

        for (const auto &arg : evaluated_args)
            if (isReg(arg) && !isParmReg(arg))
                freeReg(arg);
    }

    void CodeGenVisitor::visit(FuncCallNode &node) {
        // Handle array-specific Length, High, Low before string Length builtin
        std::string fnLower = node.name;
        std::transform(fnLower.begin(), fnLower.end(), fnLower.begin(),
                       [](unsigned char c) { return std::tolower(c); });
        if ((fnLower == "length" || fnLower == "high" || fnLower == "low") && node.arguments.size() == 1) {
            if (auto *varNode = dynamic_cast<VariableNode *>(node.arguments[0].get())) {
                std::string mangled = findMangledArrayName(varNode->name);
                auto it = arrayInfo.find(mangled);
                if (it == arrayInfo.end())
                    it = arrayInfo.find(varNode->name);
                if (it != arrayInfo.end()) {
                    if (fnLower == "low") {
                        // Dynamic arrays are always 0-based; static arrays use lowerBound
                        std::string r = allocReg();
                        emit2("mov", r, std::to_string(it->second.isDynamic ? 0 : it->second.lowerBound));
                        pushValue(r);
                        return;
                    }
                    if (it->second.isDynamic) {
                        // Runtime: read from companion length variable
                        std::string arrMangled = (it == arrayInfo.find(mangled)) ? mangled : varNode->name;
                        auto lenIt = dynArrayLenSlot.find(arrMangled);
                        if (lenIt == dynArrayLenSlot.end())
                            throw std::runtime_error("Length/High: dynamic array has no companion length variable: " + varNode->name);
                        std::string lenSym = slotVar(lenIt->second);
                        std::string r = allocReg();
                        emit2("mov", r, lenSym);
                        if (fnLower == "high") {
                            emit2("sub", r, "1");
                        }
                        pushValue(r);
                        return;
                    } else {
                        // Static array: compile-time constants
                        std::string r = allocReg();
                        if (fnLower == "length") {
                            emit2("mov", r, std::to_string(it->second.size));
                        } else { // high
                            emit2("mov", r, std::to_string(it->second.upperBound));
                        }
                        pushValue(r);
                        return;
                    }
                }
            }
        }

        auto handler = builtinRegistry.findHandler(fnLower);
        if (handler) {
            if (handler->generateWithResult(*this, fnLower, node.arguments))
                return;
        }

        std::vector<std::string> evaluated_args;
        std::vector<VarType> argTypes;

        for (auto &arg : node.arguments) {
            std::string argVal = eval(arg.get());
            evaluated_args.push_back(argVal);
            argTypes.push_back(getExpressionType(arg.get()));
        }

        // Build target register list
        std::vector<std::string> targetRegs(evaluated_args.size());
        size_t intRegIdx = 1;
        size_t ptrRegIdx = 0;
        size_t floatRegIdx = 0;

        for (size_t i = 0; i < evaluated_args.size(); ++i) {
            if (argTypes[i] == VarType::STRING || argTypes[i] == VarType::PTR || argTypes[i] == VarType::RECORD) {
                if (ptrRegIdx < ptrRegisters.size())
                    targetRegs[i] = ptrRegisters[ptrRegIdx++];
            } else if (argTypes[i] == VarType::DOUBLE) {
                if (floatRegIdx < floatRegisters.size())
                    targetRegs[i] = floatRegisters[floatRegIdx++];
            } else {
                if (intRegIdx < registers.size())
                    targetRegs[i] = registers[intRegIdx++];
            }
        }

        // Spill any evaluated arg that sits in another arg's target register
        for (size_t i = 0; i < evaluated_args.size(); ++i) {
            if (evaluated_args[i].empty() || targetRegs[i].empty())
                continue;
            if (evaluated_args[i] == targetRegs[i])
                continue;
            for (size_t j = 0; j < evaluated_args.size(); ++j) {
                if (j == i)
                    continue;
                if (evaluated_args[j] == targetRegs[i]) {
                    std::string spill = allocReg();
                    emit2("mov", spill, evaluated_args[j]);
                    evaluated_args[j] = spill;
                }
            }
        }

        // Save caller's in-use registers that could be clobbered by this call
        std::vector<std::string> savedRegs;
        for (size_t i = 1; i < regInUse.size() && i < registers.size(); ++i) {
            if (!regInUse[i]) continue;
            bool isEvalArg = false;
            for (const auto &ea : evaluated_args) {
                if (ea == registers[i]) { isEvalArg = true; break; }
            }
            if (isEvalArg) continue;
            savedRegs.push_back(registers[i]);
        }
        for (const auto &sr : savedRegs)
            emit1("push", sr);

        // Now move into target registers
        {
            auto eit = externalFuncs.find(node.name);
            std::string objPrefix = (eit != externalFuncs.end()) ? eit->second + "." : "";
            for (size_t i = 0; i < evaluated_args.size(); ++i) {
                if (!targetRegs[i].empty())
                    emit2("mov", objPrefix + targetRegs[i], evaluated_args[i]);
            }
        }

        std::string mangledName = findMangledFuncName(node.name, false);
        {
            std::string label = "FUNC_" + mangledName;
            auto eit = externalFuncs.find(node.name);
            if (eit != externalFuncs.end())
                label = eit->second + "." + label;

            if (!currentFuncLocalSlots.empty()) {
                for (const auto &slot : currentFuncLocalSlots)
                    emit1("push", slot);
            }

            emit1("call", label);

            if (!currentFuncLocalSlots.empty()) {
                for (auto it2 = currentFuncLocalSlots.rbegin(); it2 != currentFuncLocalSlots.rend(); ++it2)
                    emit1("pop", *it2);
            }
        }

        // Restore caller's saved registers
        for (auto rit = savedRegs.rbegin(); rit != savedRegs.rend(); ++rit)
            emit1("pop", *rit);

        auto it = funcSignatures.find(node.name);
        VarType returnType = (it != funcSignatures.end()) ? it->second.returnType : VarType::INT;

        std::string resultLocation;
        {
            auto eit = externalFuncs.find(node.name);
            std::string objPrefix = (eit != externalFuncs.end()) ? eit->second + "." : "";
            if (returnType == VarType::DOUBLE) {
                resultLocation = allocFloatReg();
                emit2("mov", resultLocation, objPrefix + "xmm0");
            } else if (returnType == VarType::STRING || returnType == VarType::PTR || returnType == VarType::RECORD) {
                resultLocation = allocTempPtr();
                emit2("mov", resultLocation, objPrefix + "arg0");
            } else {
                resultLocation = allocReg();
                emit2("mov", resultLocation, objPrefix + "rax");
            }
        }
        pushValue(resultLocation);

        for (const auto &arg : evaluated_args)
            if (isReg(arg) && !isParmReg(arg))
                freeReg(arg);
    }

    void CodeGenVisitor::visit(FuncDeclNode &node) {
        std::string mangledName = mangleVariableName(node.name);
        if (declaredFuncs.count(mangledName)) {
            return;
        }
        declaredFuncs[mangledName] = true;

        FuncInfo funcInfo;
        funcInfo.returnType = getTypeFromString(node.returnType);
        for (auto &p : node.parameters) {
            if (auto pn = dynamic_cast<ParameterNode *>(p.get())) {
                for (size_t i = 0; i < pn->identifiers.size(); ++i) {
                    funcInfo.paramTypes.push_back(getTypeFromString(pn->type));
                }
            }
        }
        funcSignatures[node.name] = funcInfo;
        setVarType(node.name, funcInfo.returnType);

        // Only defer code generation if there's an actual body (not a forward declaration)
        if (node.block) {
            auto path = scopeHierarchy;
            path.push_back(node.name);
            deferredFuncs.push_back({&node, path});

            scopeHierarchy.push_back(node.name);
            if (auto blockNode = dynamic_cast<BlockNode *>(node.block.get())) {
                for (auto &decl : blockNode->declarations) {
                    if (dynamic_cast<ProcDeclNode *>(decl.get()) || dynamic_cast<FuncDeclNode *>(decl.get())) {
                        decl->accept(*this);
                    }
                }
            }
            scopeHierarchy.pop_back();
        }
    }

    void CodeGenVisitor::visit(ProcDeclNode &node) {
        std::string mangledName = mangleVariableName(node.name);
        if (declaredProcs.count(mangledName)) {
            return;
        }
        declaredProcs[mangledName] = true;

        // Only defer code generation if there's an actual body (not a forward declaration)
        if (node.block) {
            auto path = scopeHierarchy;
            path.push_back(node.name);
            deferredProcs.push_back({&node, path});

            scopeHierarchy.push_back(node.name);
            if (auto blockNode = dynamic_cast<BlockNode *>(node.block.get())) {
                for (auto &decl : blockNode->declarations) {
                    if (dynamic_cast<ProcDeclNode *>(decl.get()) || dynamic_cast<FuncDeclNode *>(decl.get())) {
                        decl->accept(*this);
                    }
                }
            }
            scopeHierarchy.pop_back();
        }
    }

    void CodeGenVisitor::visit(TypeAliasNode &node) {
        auto lc = [](std::string s) { std::transform(s.begin(), s.end(), s.begin(),
                                    [](unsigned char c){ return std::tolower(c); }); return s; };
        typeAliases[lc(node.typeName)] = lc(node.baseType);
    }

    void CodeGenVisitor::visit(ParameterNode &node) {
        auto lc = [](std::string s) {
            std::transform(s.begin(), s.end(), s.begin(),
                           [](unsigned char c) { return std::tolower(c); });
            return s;
        };
        for (auto &idRaw : node.identifiers) {
            std::string id = idRaw;
            int slot = newSlotFor(id);
            std::string t = lc(node.type);
            if (t == "string") {
                setVarType(id, VarType::STRING);
                setSlotType(slot, VarType::STRING);
            } else if (t == "integer" || t == "boolean") {
                setVarType(id, VarType::INT);
                setSlotType(slot, VarType::INT);
            } else if (t == "real") {
                setVarType(id, VarType::DOUBLE);
                setSlotType(slot, VarType::DOUBLE);
            } else if (!t.empty() && t[0] == '^') {
                setVarType(id, VarType::PTR);
                setSlotType(slot, VarType::PTR);
            } else {
                std::string rt = resolveTypeName(t);
                if (recordTypes.count(rt)) {
                    setVarType(id, VarType::RECORD);
                    setSlotType(slot, VarType::PTR);
                    varRecordType[id] = rt;
                    currentParamTypes[id] = rt;
                }
            }
        }
    }

    /**
     * @brief Visit a compound statement block, freeing temporary pointers after each statement.
     *
     * Snapshots the set of allocated temporary pointers before each statement
     * and emits `free` instructions for any new temporaries that were not
     * "escaped" (assigned to a named variable).  This prevents memory leaks
     * from intermediate string concatenation results and other temporary
     * heap allocations.
     */
    void CodeGenVisitor::visit(CompoundStmtNode &node) {
        for (auto &stmt : node.statements) {
            if (!stmt)
                continue;

            // snapshot allocated temp ptrs before statement
            auto ptrsBefore = allocatedPtrs;
            auto escapedBefore = escapedTempPtrs;

            if (auto v = dynamic_cast<VariableNode *>(stmt.get())) {
                std::string mangled = findMangledFuncName(v->name, true);
                if (declaredProcs.count(mangled)) {
                    std::string prefix = "PROC_";
                    if (funcSignatures.count(v->name))
                        prefix = "FUNC_";
                    std::string label = prefix + mangled;
                    auto eit = externalFuncs.find(v->name);
                    if (eit != externalFuncs.end())
                        label = eit->second + "." + label;
                    emit1("call", label);
                    // free any temp ptrs allocated during this statement (not escaped)
                    for (auto &p : allocatedPtrs) {
                        if (!ptrsBefore.count(p) && !escapedTempPtrs.count(p))
                            emit("free " + p);
                    }
                    allocatedPtrs = ptrsBefore;
                    continue;
                }
            }
            stmt->accept(*this);

            // free any temp ptrs allocated during this statement (not escaped)
            for (auto &p : allocatedPtrs) {
                if (!ptrsBefore.count(p) && !escapedTempPtrs.count(p))
                    emit("free " + p);
            }
            allocatedPtrs = ptrsBefore;
        }
    }

    void CodeGenVisitor::visit(IfStmtNode &node) {
        std::string elseL = newLabel("ELSE");
        std::string endL = newLabel("ENDIF");

        std::string condResult = eval(node.condition.get());

        emit2("cmp", condResult, "0");
        emit1("je", elseL);

        if (isReg(condResult) && !isParmReg(condResult)) {
            freeReg(condResult);
        }
        if (node.thenStatement) {
            node.thenStatement->accept(*this);
        }
        if (node.elseStatement) {
            emit1("jmp", endL);
            emitLabel(elseL);
            node.elseStatement->accept(*this);
            emitLabel(endL);
        } else {
            emitLabel(elseL);
        }
    }

    void CodeGenVisitor::visit(WhileStmtNode &node) {
        std::string start = newLabel("WHILE");
        std::string end = newLabel("ENDWHILE");
        loopContinueLabels.push_back(start);
        loopEndLabels.push_back(end);
        emitLabel(start);
        std::string c = eval(node.condition.get());
        emit2("cmp", c, "0");
        emit1("je", end);
        if (isReg(c) && !isParmReg(c))
            freeReg(c);
        if (node.statement)
            node.statement->accept(*this);
        emit1("jmp", start);
        emitLabel(end);
        loopContinueLabels.pop_back();
        loopEndLabels.pop_back();
    }

    void CodeGenVisitor::visit(ForStmtNode &node) {
        std::string startVal = eval(node.startValue.get());
        std::string endVal = eval(node.endValue.get());

        startVal = coerceToIntImmediate(startVal);
        endVal = coerceToIntImmediate(endVal);

        std::string mangledLoopVar = mangleVariableName(node.variable);
        int slot = newSlotFor(mangledLoopVar);
        emit2("mov", slotVar(slot), startVal);
        if (isReg(startVal) && !isParmReg(startVal))
            freeReg(startVal);

        std::string loopStartLabel = newLabel("FOR");
        std::string loopEndLabel = newLabel("ENDFOR");
        std::string continueLabel = newLabel("FOR_CONTINUE");

        loopContinueLabels.push_back(continueLabel);
        loopEndLabels.push_back(loopEndLabel);

        std::string endCmp = endVal;

        emitLabel(loopStartLabel);

        emit2("cmp", slotVar(slot), endCmp);
        if (node.isDownto)
            emit1("jl", loopEndLabel);
        else
            emit1("jg", loopEndLabel);

        if (node.statement)
            node.statement->accept(*this);
        emitLabel(continueLabel);
        if (node.isDownto)
            emit2("sub", slotVar(slot), "1");
        else
            emit2("add", slotVar(slot), "1");
        emit1("jmp", loopStartLabel);
        emitLabel(loopEndLabel);

        loopContinueLabels.pop_back();
        loopEndLabels.pop_back();

        if (isReg(endCmp) && !isParmReg(endCmp))
            freeReg(endCmp);
    }

    void CodeGenVisitor::visit(BinaryOpNode &node) {
        auto isStrLike = [&](VarType v) { return v == VarType::STRING || v == VarType::PTR; };
        VarType lt = getExpressionType(node.left.get());
        VarType rt = getExpressionType(node.right.get());

        // Handle 'in' operator: expr in [val1, val2, ...] or expr in setVar
        if (node.operator_ == BinaryOpNode::IN) {
            auto *setNode = dynamic_cast<SetLiteralNode *>(node.right.get());
            if (setNode) {
                // Inline set literal: compare against each element
                std::string val = eval(node.left.get());
                std::string foundLabel = newLabel("IN_FOUND");
                std::string endLabel = newLabel("IN_END");
                std::string result = allocReg();

                for (auto &elem : setNode->elements) {
                    std::string elemVal = eval(elem.get());
                    emit2("cmp", val, elemVal);
                    emit1("je", foundLabel);
                    if (isReg(elemVal) && !isParmReg(elemVal))
                        freeReg(elemVal);
                }
                emit2("mov", result, "0");
                emit1("jmp", endLabel);
                emitLabel(foundLabel);
                emit2("mov", result, "1");
                emitLabel(endLabel);

                if (isReg(val) && !isParmReg(val))
                    freeReg(val);
                pushValue(result);
                return;
            }
            // Set variable: load byte at index
            std::string val = eval(node.left.get());
            std::string setVal = eval(node.right.get());
            std::string byte = allocReg();
            std::string result = allocReg();
            emit4("load", byte, setVal, val, "8");
            emit2("cmp", byte, "0");
            std::string notFoundLabel = newLabel("IN_NOT_FOUND");
            std::string endLabel = newLabel("IN_END");
            emit1("je", notFoundLabel);
            emit2("mov", result, "1");
            emit1("jmp", endLabel);
            emitLabel(notFoundLabel);
            emit2("mov", result, "0");
            emitLabel(endLabel);
            freeReg(byte);
            if (isReg(val) && !isParmReg(val))
                freeReg(val);
            if (isReg(setVal) && !isParmReg(setVal))
                freeReg(setVal);
            pushValue(result);
            return;
        }

        // Set operations: + (union), - (difference), * (intersection)
        auto isSetExpr = [&](ASTNode *n) -> bool {
            if (dynamic_cast<SetLiteralNode *>(n))
                return true;
            if (auto *v = dynamic_cast<VariableNode *>(n)) {
                std::string mangled = findMangledName(v->name);
                return setVars.count(mangled) || setVars.count(v->name);
            }
            return false;
        };
        if ((node.operator_ == BinaryOpNode::PLUS ||
             node.operator_ == BinaryOpNode::MINUS ||
             node.operator_ == BinaryOpNode::MULTIPLY) &&
            (isSetExpr(node.left.get()) || isSetExpr(node.right.get()))) {

            std::string leftSet = eval(node.left.get());
            std::string rightSet = eval(node.right.get());

            // Allocate result set
            std::string resultSet = allocTempPtr();
            emit3("alloc", resultSet, "8", "256");

            std::string idx = allocReg();
            std::string lb = allocReg();
            std::string rb = allocReg();
            std::string loopStart = newLabel("SET_OP");
            std::string loopEnd = newLabel("SET_OP_END");
            std::string storeBit = newLabel("SET_OP_STORE");
            std::string storeZero = newLabel("SET_OP_ZERO");

            emit2("mov", idx, "0");
            emitLabel(loopStart);
            emit2("cmp", idx, "256");
            emit1("jge", loopEnd);
            emit4("load", lb, leftSet, idx, "8");
            emit4("load", rb, rightSet, idx, "8");

            if (node.operator_ == BinaryOpNode::PLUS) {
                // Union: result = left OR right
                emit2("cmp", lb, "0");
                emit1("jne", storeBit);
                emit2("cmp", rb, "0");
                emit1("jne", storeBit);
                emit1("jmp", storeZero);
            } else if (node.operator_ == BinaryOpNode::MULTIPLY) {
                // Intersection: result = left AND right
                emit2("cmp", lb, "0");
                emit1("je", storeZero);
                emit2("cmp", rb, "0");
                emit1("je", storeZero);
                emit1("jmp", storeBit);
            } else {
                // Difference: result = left AND NOT right
                emit2("cmp", lb, "0");
                emit1("je", storeZero);
                emit2("cmp", rb, "0");
                emit1("jne", storeZero);
                emit1("jmp", storeBit);
            }
            emitLabel(storeBit);
            emit4("store", "1", resultSet, idx, "8");
            std::string nextLabel = newLabel("SET_OP_NEXT");
            emit1("jmp", nextLabel);
            emitLabel(storeZero);
            emit4("store", "0", resultSet, idx, "8");
            emitLabel(nextLabel);
            emit2("add", idx, "1");
            emit1("jmp", loopStart);
            emitLabel(loopEnd);

            freeReg(idx);
            freeReg(lb);
            freeReg(rb);
            if (isReg(leftSet) && !isParmReg(leftSet))
                freeReg(leftSet);
            if (isReg(rightSet) && !isParmReg(rightSet))
                freeReg(rightSet);
            pushValue(resultSet);
            return;
        }

        if (node.operator_ == BinaryOpNode::PLUS && (isStrLike(lt) || isStrLike(rt))) {
            usedModules.insert("string");
            std::string left = eval(node.left.get());
            std::string right = eval(node.right.get());
            std::string len1 = allocReg(), len2 = allocReg(), totalLen = allocReg();
            emit_invoke("strlen", {left});
            emit("return " + len1);
            emit_invoke("strlen", {right});
            emit("return " + len2);
            emit2("mov", totalLen, len1);
            emit2("add", totalLen, len2);
            emit2("add", totalLen, "1");

            std::string result_str = allocTempPtr();
            emit3("alloc", result_str, "1", totalLen);
            emit_invoke("strncpy", {result_str, left, len1});
            emit_invoke("strncat", {result_str, right, len2});

            pushValue(result_str);
            freeReg(len1);
            freeReg(len2);
            freeReg(totalLen);
            if (isReg(left) && !isParmReg(left))
                freeReg(left);
            if (isReg(right) && !isParmReg(right))
                freeReg(right);
            return;
        }
        try {
            std::string folded = foldNumeric(&node);
            if (!folded.empty()) {
                if (node.operator_ == BinaryOpNode::DIVIDE && isIntegerLiteral(folded))
                    folded += ".0";
                if (isFloatLiteral(folded))
                    pushValue(ensureFloatConstSymbol(folded));
                else
                    pushValue(folded);
                return;
            }
        } catch (...) {
        }

        auto evalOperand = [&](ASTNode *n) -> std::string {
            if (auto var = dynamic_cast<VariableNode *>(n)) {
                std::string v;
                if (tryGetConstNumeric(var->name, v))
                    return v;
            }
            return eval(n);
        };

        std::string left = evalOperand(node.left.get());
        std::string right = evalOperand(node.right.get());

        bool needsFloatOp = (lt == VarType::DOUBLE || rt == VarType::DOUBLE ||
                             isFloatLiteral(left) || isFloatLiteral(right) ||
                             node.operator_ == BinaryOpNode::DIVIDE);

        if (node.operator_ == BinaryOpNode::DIVIDE) {
            needsFloatOp = true;
            if (isIntegerLiteral(left) && !isFloatLiteral(left))
                left += ".0";
            if (isIntegerLiteral(right) && !isFloatLiteral(right))
                right += ".0";
        }

        if (node.operator_ == BinaryOpNode::DIV && (isFloatLiteral(left) || isFloatLiteral(right))) {
            if (isFloatLiteral(left))
                left = std::to_string((long long)std::stod(left));
            if (isFloatLiteral(right))
                right = std::to_string((long long)std::stod(right));
            needsFloatOp = false;
        }

        if (needsFloatOp && (node.operator_ == BinaryOpNode::PLUS ||
                             node.operator_ == BinaryOpNode::MINUS ||
                             node.operator_ == BinaryOpNode::MULTIPLY ||
                             node.operator_ == BinaryOpNode::DIVIDE)) {
            std::string dst;
            if (isFloatReg(left) && !isParmReg(left)) {
                dst = left;
            } else {
                dst = allocFloatReg();
                emit2("mov", dst, left);
            }

            emit2(node.operator_ == BinaryOpNode::DIVIDE ? "div" : node.operator_ == BinaryOpNode::MULTIPLY ? "mul"
                                                               : node.operator_ == BinaryOpNode::MINUS      ? "sub"
                                                                                                            : "add",
                  dst, right);

            if (isReg(right) && !isParmReg(right))
                freeReg(right);
            pushValue(dst);
            return;
        }

        auto emitBinary = [&](const char *op) {
            std::string dst;
            bool leftIsUsableReg = isReg(left) && !isParmReg(left);
            if (leftIsUsableReg)
                dst = left;
            else {
                dst = allocReg();
                emit2("mov", dst, left);
            }
            emit2(op, dst, right);
            if (isReg(right) && !isParmReg(right))
                freeReg(right);
            pushValue(dst);
        };

        switch (node.operator_) {
        case BinaryOpNode::PLUS:
            emitBinary("add");
            break;
        case BinaryOpNode::MINUS:
            emitBinary("sub");
            break;
        case BinaryOpNode::MULTIPLY:
            emitBinary("mul");
            break;
        case BinaryOpNode::DIVIDE:
            emitBinary("div");
            break;
        case BinaryOpNode::DIV:
            emitBinary("div");
            break;
        case BinaryOpNode::MOD:
            emitBinary("mod");
            break;
        case BinaryOpNode::AND:
            pushLogicalAnd(left, right);
            break;
        case BinaryOpNode::OR:
            pushLogicalOr(left, right);
            break;
        case BinaryOpNode::EQUAL:
            pushCmpResult(left, right, "je");
            break;
        case BinaryOpNode::NOT_EQUAL:
            pushCmpResult(left, right, "jne");
            break;
        case BinaryOpNode::LESS:
            pushCmpResult(left, right, "jl");
            break;
        case BinaryOpNode::LESS_EQUAL:
            pushCmpResult(left, right, "jle");
            break;
        case BinaryOpNode::GREATER:
            pushCmpResult(left, right, "jg");
            break;
        case BinaryOpNode::GREATER_EQUAL:
            pushCmpResult(left, right, "jge");
            break;
        default:
            throw std::runtime_error("Unsupported binary operator");
        }
    }

    void CodeGenVisitor::visit(UnaryOpNode &node) {
        std::string v = eval(node.operand.get());
        switch (node.operator_) {
        case UnaryOpNode::MINUS: {
            std::string t = allocReg();
            emit2("mov", t, "0");
            emit2("sub", t, v);
            pushValue(t);
            break;
        }
        case UnaryOpNode::PLUS: {
            pushValue(v);
            break;
        }
        case UnaryOpNode::NOT: {
            emit("not " + v);
            pushValue(v);
            break;
        }
        }
    }

    void CodeGenVisitor::visit(VariableNode &node) {
        // 'result' inside a function refers to the return value
        if (lc(node.name) == "result" && !currentFunctionName.empty()) {
            if (!currentFunctionReturnSlot.empty()) {
                pushValue(currentFunctionReturnSlot);
            } else {
                VarType rt = getVarType(currentFunctionName);
                int slot = newSlotFor("__funcret_" + currentFunctionName);
                if (rt == VarType::DOUBLE)
                    setSlotType(slot, VarType::DOUBLE);
                else if (rt == VarType::STRING || rt == VarType::PTR || rt == VarType::RECORD)
                    setSlotType(slot, VarType::PTR);
                else
                    setSlotType(slot, VarType::INT);
                currentFunctionReturnSlot = slotVar(slot);
                pushValue(currentFunctionReturnSlot);
            }
            return;
        }

        // Check with-scopes: if this name is a field in an active 'with' block,
        // treat it as recordVar.fieldName
        for (auto it = withFieldScopes.rbegin(); it != withFieldScopes.rend(); ++it) {
            auto fit = it->find(lc(node.name));
            if (fit != it->end()) {
                auto recExpr = std::make_unique<VariableNode>(fit->second);
                FieldAccessNode syntheticField(std::move(recExpr), node.name);
                visit(syntheticField);
                return;
            }
        }

        if (auto pit = currentParamLocations.find(node.name); pit != currentParamLocations.end()) {
            pushValue(pit->second);
            return;
        }

        std::string mangled = findMangledName(node.name);

        if (auto ct = compileTimeConstants.find(mangled); ct != compileTimeConstants.end()) {
            pushValue(ct->second);
            return;
        }
        if (auto ct2 = compileTimeConstants.find(node.name); ct2 != compileTimeConstants.end()) {
            pushValue(ct2->second);
            return;
        }
        if (auto ct3 = compileTimeConstants.find(lc(node.name)); ct3 != compileTimeConstants.end()) {
            pushValue(ct3->second);
            return;
        }
        

        if (auto sit = varSlot.find(mangled); sit != varSlot.end()) {
            pushValue(slotVar(sit->second));
            return;
        }

        // Zero-arg function call without parentheses
        if (externalFuncs.count(node.name) || funcSignatures.count(node.name)) {
            FuncCallNode syntheticCall(node.name, {});
            visit(syntheticCall);
            return;
        }

        pushValue(mangled);
    }

    void CodeGenVisitor::visit(NumberNode &node) {
        if (node.isReal || isRealNumber(node.value)) {
            std::string reg = allocFloatReg();
            emit2("mov", reg, ensureFloatConstSymbol(node.value));
            pushValue(reg);
        } else {
            pushValue(node.value);
        }
    }

    void CodeGenVisitor::visit(StringNode &node) {
        std::string sym = internString(node.value);
        pushValue(sym);
    }

    void CodeGenVisitor::visit(BooleanNode &node) {
        pushValue(node.value ? "1" : "0");
    }

    void CodeGenVisitor::visit(EmptyStmtNode &node) {
        // No operation
    }

    void CodeGenVisitor::visit(ConstDeclNode &node) {
        for (const auto &assignment : node.assignments) {
            if (auto strNode = dynamic_cast<StringNode *>(assignment->value.get())) {
                std::string sym = internString(strNode->value);

                std::string mangledName = mangleVariableName(assignment->identifier);
                compileTimeConstants[mangledName] = mangledName;
                compileTimeConstants[assignment->identifier] = mangledName;

                int slot = newSlotFor(mangledName);
                setVarType(assignment->identifier, VarType::PTR);
                setSlotType(slot, VarType::PTR);
                updateDataSectionInitialValue(slotVar(slot), "ptr", "null");
                prolog.push_back("mov " + slotVar(slot) + ", " + sym);
                continue;
            }

            try {
                std::string literalValue = evaluateConstantExpression(assignment->value.get());
                bool isFloat = isRealNumber(literalValue);
                if (isFloat)
                    literalValue = ensureFloatConstSymbol(literalValue);

                std::string varType = isFloat ? "float" : "int";
                std::string mangledName = mangleVariableName(assignment->identifier);

                compileTimeConstants[mangledName] = (isFloat ? realConstants[literalValue] : literalValue);
                compileTimeConstants[assignment->identifier] = (isFloat ? realConstants[literalValue] : literalValue);

                int slot = newSlotFor(mangledName);
                VarType vType = isFloat ? VarType::DOUBLE : VarType::INT;
                setVarType(assignment->identifier, vType);
                setSlotType(slot, vType);

                updateDataSectionInitialValue(slotVar(slot), varType,
                                              isFloat ? realConstants[literalValue] : literalValue);
            } catch (const std::runtime_error &) {
                std::string valueReg = eval(assignment->value.get());
                int slot = newSlotFor(assignment->identifier);
                VarType exprType = getExpressionType(assignment->value.get());
                setVarType(assignment->identifier, exprType);
                setSlotType(slot, exprType);
                std::string varLocation = slotVar(slot);
                emit2("mov", varLocation, valueReg);
                if (isReg(valueReg) && !isParmReg(valueReg))
                    freeReg(valueReg);
            }
        }
    }

    void CodeGenVisitor::visit(RepeatStmtNode &node) {
        std::string startLabel = newLabel("REPEAT");
        std::string endLabel = newLabel("UNTIL");
        std::string continueLabel = newLabel("REPEAT_CONTINUE");

        loopContinueLabels.push_back(continueLabel);
        loopEndLabels.push_back(endLabel);

        emitLabel(startLabel);
        for (auto &stmt : node.statements)
            if (stmt)
                stmt->accept(*this);
        emitLabel(continueLabel);
        std::string condResult = eval(node.condition.get());
        emit2("cmp", condResult, "0");
        emit1("je", startLabel);
        if (isReg(condResult) && !isParmReg(condResult))
            freeReg(condResult);
        emitLabel(endLabel);
        loopContinueLabels.pop_back();
        loopEndLabels.pop_back();
    }

    void CodeGenVisitor::visit(CaseStmtNode &node) {
        VarType exprType = getExpressionType(node.expression.get());
        std::string switchExpr = eval(node.expression.get());
        std::string endLabel = newLabel("CASE_END");
        std::vector<std::string> branchLabels;
        for (size_t i = 0; i < node.branches.size(); i++)
            branchLabels.push_back(newLabel("CASE_" + std::to_string(i)));
        std::string elseLabel = newLabel("CASE_ELSE");
        for (size_t i = 0; i < node.branches.size(); i++) {
            auto &branch = node.branches[i];
            for (auto &value : branch->values) {
                std::string caseValue;
                if ((exprType == VarType::CHAR || exprType == VarType::INT) &&
                    dynamic_cast<StringNode *>(value.get())) {
                    auto *strNode = static_cast<StringNode *>(value.get());
                    if (strNode->value.size() == 1) {
                        caseValue = std::to_string((int)(unsigned char)strNode->value[0]);
                    } else {
                        caseValue = eval(value.get());
                    }
                } else {
                    caseValue = eval(value.get());
                }
                emit2("cmp", switchExpr, caseValue);
                emit1("je", branchLabels[i]);
                if (isReg(caseValue) && !isParmReg(caseValue))
                    freeReg(caseValue);
            }
        }
        if (node.elseStatement)
            emit1("jmp", elseLabel);
        else
            emit1("jmp", endLabel);
        for (size_t i = 0; i < node.branches.size(); i++) {
            emitLabel(branchLabels[i]);
            if (node.branches[i]->statement)
                node.branches[i]->statement->accept(*this);
            emit1("jmp", endLabel);
        }
        if (node.elseStatement) {
            emitLabel(elseLabel);
            node.elseStatement->accept(*this);
        }
        emitLabel(endLabel);
        if (isReg(switchExpr) && !isParmReg(switchExpr))
            freeReg(switchExpr);
    }

    void CodeGenVisitor::visit(ArrayTypeNode &node) {
        // Typically handled within other nodes like VarDeclNode or TypeDeclNode
    }

    void CodeGenVisitor::visit(ArrayDeclarationNode &node) {
        std::string mangledName = findMangledName(node.name);

        std::string elementType;
        if (auto simpleType = dynamic_cast<SimpleTypeNode *>(node.arrayType->elementType.get())) {
            elementType = simpleType->typeName;
        } else if (dynamic_cast<ArrayTypeNode *>(node.arrayType->elementType.get())) {
            elementType = "array";
        } else if (dynamic_cast<RecordTypeNode *>(node.arrayType->elementType.get())) {
            elementType = "record";
        } else if (auto *pt = dynamic_cast<PointerTypeNode *>(node.arrayType->elementType.get())) {
            elementType = "^" + pt->baseTypeName;
        } else {
            elementType = "integer";
        }

        int lowerBound = std::stoi(evaluateConstantExpression(node.arrayType->lowerBound.get()));
        int upperBound = std::stoi(evaluateConstantExpression(node.arrayType->upperBound.get()));
        int size = upperBound - lowerBound + 1;

        int elementSize = 8;

        if (isRecordTypeName(elementType)) {
            auto it = recordTypes.find(elementType);
            if (it != recordTypes.end()) {
                elementSize = it->second.size;
            }
        } else {
            elementSize = getArrayElementSize(elementType);
        }

        ArrayInfo info;
        info.elementType = elementType;
        info.lowerBound = lowerBound;
        info.upperBound = upperBound;
        info.size = size;
        info.elementSize = elementSize;
        mangledName = findMangledName(node.name);
        arrayInfo[mangledName] = std::move(info);
        int slot = newSlotFor(mangledName);
        varSlot[node.name] = slot;
        varSlot[mangledName] = slot;
        setVarType(node.name, VarType::PTR);
        setSlotType(slot, VarType::PTR);
        updateDataSectionInitialValue(slotVar(slot), "ptr", "null");
        emit3("alloc",
              slotVar(slot),
              std::to_string(elementSize),
              std::to_string(size));
        std::string currentScope = getCurrentScopeName();
        if (currentScope.empty())
            globalArrays.push_back(slotVar(slot));
        else
            functionScopedArrays[currentScope].push_back(slotVar(slot));
    }

    void CodeGenVisitor::visit(FieldAccessNode &node) {
        // Cross-unit variable reference: UnitName.varName
        if (auto *v = dynamic_cast<VariableNode *>(node.recordExpr.get())) {
            if (isImportedUnit(v->name)) {
                std::string qualifiedName = v->name + "." + node.fieldName;

                // Check for imported constant first
                auto ct = compileTimeConstants.find(qualifiedName);
                if (ct != compileTimeConstants.end()) {
                    pushValue(ct->second);
                    return;
                }

                // Check for zero-arg function call: UnitName.FuncName
                auto eit = externalFuncs.find(node.fieldName);
                if (eit != externalFuncs.end()) {
                    FuncCallNode syntheticCall(node.fieldName, {});
                    visit(syntheticCall);
                    return;
                }

                VarType vt = getVarType(qualifiedName);
                std::string dst;
                if (vt == VarType::DOUBLE)
                    dst = allocFloatReg();
                else if (vt == VarType::PTR || vt == VarType::STRING)
                    dst = allocTempPtr();
                else
                    dst = allocReg();
                emit2("mov", dst, qualifiedName);
                pushValue(dst);
                return;
            }
        }

        std::string baseName;
        std::string recType;
        bool baseIsDirectPointer = false;

        if (auto *v = dynamic_cast<VariableNode *>(node.recordExpr.get())) {
            baseName = findMangledName(v->name);
            recType = getVarRecordTypeName(v->name);
        } else if (dynamic_cast<ArrayAccessNode *>(node.recordExpr.get())) {
            node.recordExpr->accept(*this);
            baseName = popValue();
            baseIsDirectPointer = true;
            recType = getVarRecordTypeNameFromExpr(node.recordExpr.get());
        } else {
            node.recordExpr->accept(*this);
            baseName = popValue();
            baseIsDirectPointer = true;
            recType = getVarRecordTypeNameFromExpr(node.recordExpr.get());
        }

        recType = resolveTypeName(lc(recType));
        if (recType.empty()) {
            throw std::runtime_error("Unknown record type: (empty) for field " + node.fieldName);
        }

        auto recTypeIt = recordTypes.find(recType);
        if (recTypeIt == recordTypes.end()) {
            throw std::runtime_error("Unknown record type: " + recType + " for field " + node.fieldName);
        }

        const std::string fieldName = lc(node.fieldName);
        auto &recInfo = recTypeIt->second;
        auto it = recInfo.nameToIndex.find(fieldName);
        if (it == recInfo.nameToIndex.end()) {
            throw std::runtime_error("Field not found in record: " + fieldName + " in type " + recType);
        }

        const auto &fieldInfo = recInfo.fields[it->second];
        const int byteOffset = fieldInfo.offset;

        std::string basePtr = baseIsDirectPointer ? baseName : ensurePtrBase(baseName);

        if (fieldInfo.isArray || isRecordTypeName(fieldInfo.typeName)) {
            std::string p = allocTempPtr();
            emit2("mov", p, basePtr);
            emit2("add", p, std::to_string(byteOffset));
            pushValue(p);
            return;
        }

        VarType fieldType = getTypeFromString(fieldInfo.typeName);
        std::string dst;
        if (fieldType == VarType::DOUBLE) {
            dst = allocFloatReg();
        } else if (fieldType == VarType::PTR || fieldType == VarType::STRING) {
            dst = allocTempPtr();
        } else {
            dst = allocReg();
        }

        emit4("load", dst, basePtr, std::to_string(byteOffset), "1");
        pushValue(dst);
    }

    void CodeGenVisitor::visit(AssignmentNode &node) {
        // Intercept 'with' scope: if the LHS is a variable matching a record field, rewrite
        if (auto *varLHS = dynamic_cast<VariableNode *>(node.variable.get())) {
            for (auto it = withFieldScopes.rbegin(); it != withFieldScopes.rend(); ++it) {
                auto fit = it->find(lc(varLHS->name));
                if (fit != it->end()) {
                    auto recExpr = std::make_unique<VariableNode>(fit->second);
                    auto fieldAccess = std::make_unique<FieldAccessNode>(std::move(recExpr), varLHS->name);
                    AssignmentNode syntheticAssign(std::move(fieldAccess), std::move(node.expression));
                    visit(syntheticAssign);
                    // Move expression back so the original node is not left in a moved-from state
                    node.expression = std::move(syntheticAssign.expression);
                    return;
                }
            }
        }

        if (auto deref = dynamic_cast<PointerDerefNode *>(node.variable.get())) {
            std::string rhs = eval(node.expression.get());
            std::string ptrVal = eval(deref->pointer.get());
            std::string base = ensurePtrBase(ptrVal);

            emit4("store", rhs, base, "0", "1");

            if (isReg(rhs) && !isParmReg(rhs))
                freeReg(rhs);
            if (isReg(ptrVal) && !isParmReg(ptrVal) && ptrVal != base)
                freeReg(ptrVal);
            if (isReg(base) && !isParmReg(base))
                freeReg(base);
            return;
        }

        if (auto arr = dynamic_cast<ArrayAccessNode *>(node.variable.get())) {
            ArrayInfo *info = getArrayInfoForArrayAccess(arr);
            if (!info)
                throw std::runtime_error("Unknown array: " + getArrayNameFromBase(arr->base.get()));

            std::string rhs = eval(node.expression.get());
            std::string idx = eval(arr->index.get());

            if (getExpressionType(arr->index.get()) == VarType::DOUBLE) {
                std::string intIdx = allocReg();
                emit2("mov", intIdx, idx);
                if (isReg(idx) && !isParmReg(idx))
                    freeReg(idx);
                idx = intIdx;
            }

#ifdef MXVM_BOUNDS_CHECK
            if (info->isDynamic) {
                std::string arrName = getArrayNameFromBase(arr->base.get());
                std::string mangled = findMangledArrayName(arrName);
                auto lenIt = dynArrayLenSlot.find(mangled);
                if (lenIt == dynArrayLenSlot.end())
                    lenIt = dynArrayLenSlot.find(arrName);
                if (lenIt != dynArrayLenSlot.end())
                    emitDynArrayBoundsCheck(idx, slotVar(lenIt->second));
            } else {
                emitArrayBoundsCheck(idx, info->lowerBound, info->upperBound);
            }
#endif

            std::string elemIndex;
            if (isReg(idx) && !isParmReg(idx)) {
                elemIndex = idx;
            } else {
                elemIndex = allocReg();
                emit2("mov", elemIndex, idx);
            }
            if (info->lowerBound != 0)
                emit2("sub", elemIndex, std::to_string(info->lowerBound));

            VarType elemType = VarType::INT;
            if (info->elementType == "real")
                elemType = VarType::DOUBLE;
            else if (info->elementType == "string" || info->elementType == "ptr" ||
                     info->elementType == "pointer" ||
                     (!info->elementType.empty() && info->elementType[0] == '^'))
                elemType = VarType::PTR;

            VarType rhsType = getExpressionType(node.expression.get());
            if (elemType == VarType::DOUBLE && rhsType != VarType::DOUBLE && !isFloatReg(rhs)) {
                std::string f = allocFloatReg();
                emit2("mov", f, rhs);
                if (isReg(rhs) && !isParmReg(rhs))
                    freeReg(rhs);
                rhs = f;
            } else if (elemType != VarType::DOUBLE && rhsType == VarType::DOUBLE && isFloatReg(rhs)) {
                std::string ir = allocReg();
                emit2("mov", ir, rhs);
                if (isReg(rhs) && !isParmReg(rhs))
                    freeReg(rhs);
                rhs = ir;
            }

            std::string base;
            if (auto var = dynamic_cast<VariableNode *>(arr->base.get())) {
                std::string mangled = findMangledArrayName(var->name);
                base = ensurePtrBase(storageSymbolFor(mangled));
            } else if (auto field = dynamic_cast<FieldAccessNode *>(arr->base.get())) {
                // Cross-unit array assignment: UnitName.arrayName[index] := value
                if (auto *baseVar = dynamic_cast<VariableNode *>(field->recordExpr.get());
                    baseVar && isImportedUnit(baseVar->name)) {
                    std::string qualifiedName = baseVar->name + "." + field->fieldName;
                    base = ensurePtrBase(qualifiedName);
                } else {
                    field->recordExpr->accept(*this);
                    std::string recPtr = popValue();
                    std::string recType = getVarRecordTypeNameFromExpr(field->recordExpr.get());
                    auto ofs_sz = getRecordFieldOffsetAndSize(recType, field->fieldName);
                    int fieldOffset = ofs_sz.first;

                    std::string arrayPtr = allocTempPtr();
                    emit2("mov", arrayPtr, recPtr);
                    emit2("add", arrayPtr, std::to_string(fieldOffset));
                    base = arrayPtr;
                }
            } else if (dynamic_cast<ArrayAccessNode *>(arr->base.get())) {
                arr->base->accept(*this);
                base = popValue();
                base = ensurePtrBase(base);
            } else {
                throw std::runtime_error("Unsupported array base in assignment");
            }

            emit4("store", rhs, base, elemIndex, std::to_string(info->elementSize));

            if (isReg(rhs) && !isParmReg(rhs))
                freeReg(rhs);
            if (elemIndex != idx && isReg(idx) && !isParmReg(idx))
                freeReg(idx);
            freeReg(elemIndex);
            return;
        }

        if (auto field = dynamic_cast<FieldAccessNode *>(node.variable.get())) {
            // Cross-unit simple variable assignment: UnitName.varName := value
            if (auto *v = dynamic_cast<VariableNode *>(field->recordExpr.get());
                v && isImportedUnit(v->name)) {
                std::string qualifiedName = v->name + "." + field->fieldName;
                std::string rhs = eval(node.expression.get());
                emit2("mov", qualifiedName, rhs);
                if (isReg(rhs) && !isParmReg(rhs))
                    freeReg(rhs);
                return;
            }

            std::string rhs = eval(node.expression.get());

            std::string baseName;
            std::string recType;
            bool baseIsDirectPointer = false;

            if (auto *v = dynamic_cast<VariableNode *>(field->recordExpr.get())) {
                baseName = findMangledName(v->name);
                recType = getVarRecordTypeName(v->name);
            } else {
                field->recordExpr->accept(*this);
                baseName = popValue();
                baseIsDirectPointer = true;
                recType = getVarRecordTypeNameFromExpr(field->recordExpr.get());
            }

            recType = resolveTypeName(lc(recType));
            if (recType.empty()) {
                throw std::runtime_error("Unknown record type for field assignment: " + field->fieldName);
            }

            auto recTypeIt = recordTypes.find(recType);
            if (recTypeIt == recordTypes.end()) {
                throw std::runtime_error("Unknown record type: " + recType + " for field " + field->fieldName);
            }

            const std::string fieldName = lc(field->fieldName);
            auto &recInfo = recTypeIt->second;
            auto it = recInfo.nameToIndex.find(fieldName);
            if (it == recInfo.nameToIndex.end()) {
                throw std::runtime_error("Field not found in record: " + fieldName + " in type " + recType);
            }

            const auto &fieldInfo = recInfo.fields[it->second];
            const int byteOffset = fieldInfo.offset;

            std::string basePtr = baseIsDirectPointer ? baseName : ensurePtrBase(baseName);

            // Convert between float and int if field type doesn't match rhs type.
            // When assigning a float expression (e.g. trunc() result in xmm reg) to an
            // integer record field, emit a mov to convert from float register to int
            // register first.  Without this, the raw IEEE 754 double bit pattern would
            // be stored into the integer field, producing garbage values.
            VarType fieldType = getTypeFromString(fieldInfo.typeName);
            if (fieldType == VarType::INT && isFloatReg(rhs)) {
                std::string intReg = allocReg();
                emit2("mov", intReg, rhs);
                if (isReg(rhs) && !isParmReg(rhs))
                    freeReg(rhs);
                rhs = intReg;
            } else if (fieldType == VarType::DOUBLE && !isFloatReg(rhs) && isReg(rhs)) {
                std::string fltReg = allocFloatReg();
                emit2("mov", fltReg, rhs);
                if (!isParmReg(rhs))
                    freeReg(rhs);
                rhs = fltReg;
            }

            emit4("store", rhs, basePtr, std::to_string(byteOffset), "1");

            if (isReg(rhs) && !isParmReg(rhs))
                freeReg(rhs);
            if (baseIsDirectPointer && isReg(basePtr) && !isParmReg(basePtr))
                freeReg(basePtr);

            return;
        }

        auto varPtr = dynamic_cast<VariableNode *>(node.variable.get());
        if (!varPtr)
            return;

        std::string varName = varPtr->name;

        // Set assignment: copy 256 bytes from RHS set to LHS set
        if (setVars.count(lc(varName)) || setVars.count(lc(findMangledName(varName)))) {
            std::string rhs = eval(node.expression.get());
            std::string mangled = findMangledName(varName);
            std::string destBase;
            auto sit = varSlot.find(mangled);
            if (sit != varSlot.end())
                destBase = slotVar(sit->second);
            else
                destBase = mangled;

            std::string srcBase = ensurePtrBase(rhs);
            std::string dstBase = ensurePtrBase(destBase);
            std::string idx = allocReg();
            std::string byteVal = allocReg();
            emit2("mov", idx, "0");
            std::string loopLbl = newLabel("set_copy");
            std::string endLbl = newLabel("set_copy_end");
            emitLabel(loopLbl);
            emit2("cmp", idx, "256");
            emit1("jge", endLbl);
            emit4("load", byteVal, srcBase, idx, "8");
            emit4("store", byteVal, dstBase, idx, "8");
            emit2("add", idx, "1");
            emit1("jmp", loopLbl);
            emitLabel(endLbl);
            freeReg(idx);
            freeReg(byteVal);
            if (isReg(rhs) && !isParmReg(rhs))
                freeReg(rhs);
            return;
        }

        std::string rhs;
        VarType varType = getVarType(varName);
        if ((varType == VarType::CHAR || varType == VarType::INT)) {
            if (auto *strNode = dynamic_cast<StringNode *>(node.expression.get())) {
                if (strNode->value.size() == 1) {
                    rhs = std::to_string((int)(unsigned char)strNode->value[0]);
                } else {
                    rhs = eval(node.expression.get());
                }
            } else {
                rhs = eval(node.expression.get());
            }
        } else {
            rhs = eval(node.expression.get());
        }

        auto it = currentParamLocations.find(varName);
        if (it != currentParamLocations.end()) {
            emit2("mov", it->second, rhs);
            if (isReg(rhs) && !isParmReg(rhs))
                freeReg(rhs);
            return;
        }

        if (!currentFunctionName.empty() && (varName == currentFunctionName || varName == "result")) {
            VarType rt = getVarType(currentFunctionName);
            if (rt == VarType::STRING || rt == VarType::PTR || rt == VarType::RECORD) {
                if (currentFunctionReturnSlot.empty()) {
                    int slot = newSlotFor("__funcret_" + currentFunctionName);
                    currentFunctionReturnSlot = slotVar(slot);
                    setSlotType(slot, VarType::PTR);
                }
                emit2("mov", currentFunctionReturnSlot, rhs);
            } else if (rt == VarType::DOUBLE) {
                if (currentFunctionReturnSlot.empty()) {
                    int slot = newSlotFor("__funcret_" + currentFunctionName);
                    currentFunctionReturnSlot = slotVar(slot);
                    setSlotType(slot, VarType::DOUBLE);
                }
                emit2("mov", currentFunctionReturnSlot, rhs);
            } else {
                if (currentFunctionReturnSlot.empty()) {
                    int slot = newSlotFor("__funcret_" + currentFunctionName);
                    currentFunctionReturnSlot = slotVar(slot);
                    setSlotType(slot, VarType::INT);
                }
                emit2("mov", currentFunctionReturnSlot, rhs);
            }
            functionSetReturn = true;
        } else {
            std::string mangled = findMangledName(varName);
            auto it = varSlot.find(mangled);
            if (it != varSlot.end()) {
                emit2("mov", slotVar(it->second), rhs);
                recordLocation(varName, {ValueLocation::MEMORY, slotVar(it->second)});
            } else {
                emit2("mov", mangled, rhs);
                recordLocation(varName, {ValueLocation::MEMORY, mangled});
            }
        }

        if (isTempPtr(rhs) && allocatedPtrs.count(rhs) && !isTempPtr(varName))
            escapedTempPtrs.insert(rhs);

        if (isReg(rhs) && !isParmReg(rhs))
            freeReg(rhs);
    }

    void CodeGenVisitor::visit(ArrayAssignmentNode &node) {
        auto it = arrayInfo.find(node.arrayName);
        if (it == arrayInfo.end())
            throw std::runtime_error("Unknown array: " + node.arrayName);
        ArrayInfo &info = it->second;

        std::string value = eval(node.value.get());
        std::string index = eval(node.index.get());

        if (getExpressionType(node.index.get()) == VarType::DOUBLE) {
            std::string intIndex = allocReg();
            emit2("mov", intIndex, index);
            if (isReg(index) && !isParmReg(index))
                freeReg(index);
            index = intIndex;
        }

#ifdef MXVM_BOUNDS_CHECK
        if (info.isDynamic) {
            auto lenIt = dynArrayLenSlot.find(node.arrayName);
            if (lenIt != dynArrayLenSlot.end())
                emitDynArrayBoundsCheck(index, slotVar(lenIt->second));
        } else {
            emitArrayBoundsCheck(index, info.lowerBound, info.upperBound);
        }
#endif

        std::string elementIndex;
        if (isReg(index) && !isParmReg(index)) {
            elementIndex = index;
        } else {
            elementIndex = allocReg();
            emit2("mov", elementIndex, index);
        }
        if (info.lowerBound != 0)
            emit2("sub", elementIndex, std::to_string(info.lowerBound));

        std::string base = storageSymbolFor(node.arrayName);
        base = ensurePtrBase(base);

        VarType elemType = VarType::INT;
        if (info.elementType == "real")
            elemType = VarType::DOUBLE;
        else if (info.elementType == "string" || info.elementType == "ptr" ||
                 info.elementType == "pointer" ||
                 (!info.elementType.empty() && info.elementType[0] == '^'))
            elemType = VarType::PTR;
        else if (isRecordTypeName(info.elementType))
            elemType = VarType::RECORD;

        VarType rhsType = getExpressionType(node.value.get());
        if (elemType == VarType::DOUBLE && rhsType != VarType::DOUBLE && !isFloatReg(value)) {
            std::string f = allocFloatReg();
            emit2("mov", f, value);
            if (isReg(value) && !isParmReg(value))
                freeReg(value);
            value = f;
        } else if (elemType != VarType::DOUBLE && rhsType == VarType::DOUBLE && isFloatReg(value)) {
            std::string i = allocReg();
            emit2("mov", i, value);
            if (isReg(value) && !isParmReg(value))
                freeReg(value);
            value = i;
        }

        emit4("store", value, base, elementIndex, std::to_string(info.elementSize));

        if (isReg(value) && !isParmReg(value))
            freeReg(value);
        if (elementIndex != index && isReg(index) && !isParmReg(index))
            freeReg(index);
        freeReg(elementIndex);
    }

    void CodeGenVisitor::visit(ArrayAccessNode &node) {
        ArrayInfo *info = getArrayInfoForArrayAccess(&node);
        if (!info) {
            std::string arrayName = getArrayNameFromBase(node.base.get());
            throw std::runtime_error("Unknown array: " + arrayName);
        }

        std::string idx = eval(node.index.get());

        if (getExpressionType(node.index.get()) == VarType::DOUBLE) {
            std::string intIdx = allocReg();
            emit2("mov", intIdx, idx);
            if (isReg(idx) && !isParmReg(idx))
                freeReg(idx);
            idx = intIdx;
        }

        std::string elemIndex;
        if (isReg(idx) && !isParmReg(idx)) {
            elemIndex = idx;
        } else {
            elemIndex = allocReg();
            emit2("mov", elemIndex, idx);
        }
        if (info->lowerBound != 0)
            emit2("sub", elemIndex, std::to_string(info->lowerBound));

        std::string base;
        if (auto var = dynamic_cast<VariableNode *>(node.base.get())) {
            std::string mangled = findMangledArrayName(var->name);
            base = ensurePtrBase(storageSymbolFor(mangled));
        } else if (auto field = dynamic_cast<FieldAccessNode *>(node.base.get())) {
            // Cross-unit array access: UnitName.arrayName[index]
            if (auto *baseVar = dynamic_cast<VariableNode *>(field->recordExpr.get());
                baseVar && isImportedUnit(baseVar->name)) {
                std::string qualifiedName = baseVar->name + "." + field->fieldName;
                base = ensurePtrBase(qualifiedName);
            } else {
                field->recordExpr->accept(*this);
                std::string recPtr = popValue();
                std::string recType = getVarRecordTypeNameFromExpr(field->recordExpr.get());
                auto ofs_sz = getRecordFieldOffsetAndSize(recType, field->fieldName);
                int fieldOffset = ofs_sz.first;

                std::string arrayPtr = allocTempPtr();
                emit2("mov", arrayPtr, recPtr);
                emit2("add", arrayPtr, std::to_string(fieldOffset));
                base = arrayPtr;
            }
        } else if (dynamic_cast<ArrayAccessNode *>(node.base.get())) {
            node.base->accept(*this);
            base = popValue();
        } else {
            throw std::runtime_error("Unsupported array base in access");
        }

        if (info->elementIsArray || isRecordTypeName(info->elementType)) {
            std::string offsetBytes = allocReg();
            emit2("mov", offsetBytes, elemIndex);
            emit2("mul", offsetBytes, std::to_string(info->elementSize));

            std::string elemPtr = allocTempPtr();
            emit2("mov", elemPtr, base);
            emit2("add", elemPtr, offsetBytes);
            pushValue(elemPtr);

            if (elemIndex != idx && isReg(idx) && !isParmReg(idx))
                freeReg(idx);
            freeReg(elemIndex);
            freeReg(offsetBytes);
            return;
        }

        VarType elemType = getExpressionType(&node);
        std::string dst;
        if (elemType == VarType::DOUBLE)
            dst = allocFloatReg();
        else if (elemType == VarType::PTR ||
                 elemType == VarType::STRING)
            dst = allocTempPtr();
        else
            dst = allocReg();

        emit4("load", dst, base, elemIndex, std::to_string(info->elementSize));
        pushValue(dst);

        if (elemIndex != idx && isReg(idx) && !isParmReg(idx))
            freeReg(idx);
        freeReg(elemIndex);
    }

    void CodeGenVisitor::visit(RecordTypeNode &node) {
        // Typically handled within other nodes
    }

    void CodeGenVisitor::visit(RecordDeclarationNode &node) {
        RecordTypeInfo info;
        int offset = 0;

        auto lc = [](std::string s) {
            std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) { return std::tolower(c); });
            return s;
        };

        auto resolveTypeName = [&](std::string t) {
            t = lc(t);
            std::unordered_set<std::string> seen;
            while (typeAliases.count(t) && !seen.count(t)) {
                seen.insert(t);
                t = lc(typeAliases.at(t));
            }
            return t;
        };

        for (auto &f : node.recordType->fields) {
            auto &fieldDecl = static_cast<VarDeclNode &>(*f);
            bool fieldIsArray = false;
            ArrayInfo arrInfo{};
            std::string fieldTypeName = "unknown";

            if (std::holds_alternative<std::unique_ptr<ASTNode>>(fieldDecl.type)) {
                auto &typeNode = std::get<std::unique_ptr<ASTNode>>(fieldDecl.type);
                if (auto *at = dynamic_cast<ArrayTypeNode *>(typeNode.get())) {
                    fieldIsArray = true;
                    arrInfo = buildArrayInfoFromNode(at);
                    fieldTypeName = "array";
                } else if (dynamic_cast<RecordTypeNode *>(typeNode.get())) {
                    fieldTypeName = "record";
                } else if (auto *pt = dynamic_cast<PointerTypeNode *>(typeNode.get())) {
                    fieldTypeName = "^" + lc(pt->baseTypeName);
                } else if (auto *st = dynamic_cast<SimpleTypeNode *>(typeNode.get())) {
                    fieldTypeName = resolveTypeName(st->typeName);
                }
            } else {
                fieldTypeName = resolveTypeName(std::get<std::string>(fieldDecl.type));
            }

            for (const auto &rawName : fieldDecl.identifiers) {
                RecordField rf;
                rf.name = lc(rawName);
                rf.offset = offset;
                rf.isArray = fieldIsArray;

                if (fieldIsArray) {
                    rf.arrayInfo = arrInfo;
                    rf.typeName = "array";
                    rf.size = rf.arrayInfo.size * rf.arrayInfo.elementSize;
                } else if (isRecordTypeName(fieldTypeName)) {
                    rf.typeName = fieldTypeName;
                    auto it = recordTypes.find(fieldTypeName);
                    rf.size = (it != recordTypes.end()) ? it->second.size : 8;
                } else {
                    rf.typeName = fieldTypeName;
                    rf.size = getTypeSizeByName(rf.typeName);
                }

                info.nameToIndex[rf.name] = (int)info.fields.size();
                info.fields.push_back(rf);
                offset += rf.size;
            }
        }

        // Handle variant part (if present)
        if (!node.recordType->variantTagName.empty()) {
            // Add the tag field as a regular integer field
            RecordField tagField;
            tagField.name = lc(node.recordType->variantTagName);
            tagField.typeName = resolveTypeName(node.recordType->variantTagType);
            tagField.offset = offset;
            tagField.size = getTypeSizeByName(tagField.typeName);
            tagField.isArray = false;
            info.nameToIndex[tagField.name] = (int)info.fields.size();
            info.fields.push_back(tagField);
            offset += tagField.size;

            // All variant arms share the same starting offset (union semantics)
            int variantStart = offset;
            int maxArmSize = 0;

            for (auto &arm : node.recordType->variantArms) {
                int armOffset = variantStart;
                int armSize = 0;

                for (auto &f : arm.fields) {
                    auto &fieldDecl = static_cast<VarDeclNode &>(*f);
                    bool fieldIsArray = false;
                    ArrayInfo arrInfo{};
                    std::string fieldTypeName = "unknown";

                    if (std::holds_alternative<std::unique_ptr<ASTNode>>(fieldDecl.type)) {
                        auto &typeNode = std::get<std::unique_ptr<ASTNode>>(fieldDecl.type);
                        if (auto *at = dynamic_cast<ArrayTypeNode *>(typeNode.get())) {
                            fieldIsArray = true;
                            arrInfo = buildArrayInfoFromNode(at);
                            fieldTypeName = "array";
                        } else if (dynamic_cast<RecordTypeNode *>(typeNode.get())) {
                            fieldTypeName = "record";
                        } else if (auto *pt = dynamic_cast<PointerTypeNode *>(typeNode.get())) {
                            fieldTypeName = "^" + lc(pt->baseTypeName);
                        } else if (auto *st = dynamic_cast<SimpleTypeNode *>(typeNode.get())) {
                            fieldTypeName = resolveTypeName(st->typeName);
                        }
                    } else {
                        fieldTypeName = resolveTypeName(std::get<std::string>(fieldDecl.type));
                    }

                    for (const auto &rawName : fieldDecl.identifiers) {
                        RecordField rf;
                        rf.name = lc(rawName);
                        rf.offset = armOffset;
                        rf.isArray = fieldIsArray;

                        if (fieldIsArray) {
                            rf.arrayInfo = arrInfo;
                            rf.typeName = "array";
                            rf.size = rf.arrayInfo.size * rf.arrayInfo.elementSize;
                        } else if (isRecordTypeName(fieldTypeName)) {
                            rf.typeName = fieldTypeName;
                            auto it = recordTypes.find(fieldTypeName);
                            rf.size = (it != recordTypes.end()) ? it->second.size : 8;
                        } else {
                            rf.typeName = fieldTypeName;
                            rf.size = getTypeSizeByName(rf.typeName);
                        }

                        info.nameToIndex[rf.name] = (int)info.fields.size();
                        info.fields.push_back(rf);
                        armOffset += rf.size;
                        armSize += rf.size;
                    }
                }

                if (armSize > maxArmSize)
                    maxArmSize = armSize;
            }

            offset = variantStart + maxArmSize;
        }

        info.size = offset;
        std::string recordName = lc(node.name);
        recordTypes[recordName] = info;
    }

    void CodeGenVisitor::visit(TypeDeclNode &node) {
        for (auto &typeDecl : node.typeDeclarations) {
            typeDecl->accept(*this);
        }
    }

    void CodeGenVisitor::visit(SimpleTypeNode &node) {
        // Typically handled within other nodes
    }

    void CodeGenVisitor::visit(ArrayTypeDeclarationNode &node) {
        std::string typeKey = resolveTypeName(lc(node.name));
        ArrayInfo info = buildArrayInfoFromNode(node.arrayType.get());
        arrayInfo[typeKey] = std::move(info);
    }

    void CodeGenVisitor::visit(ExitNode &node) {
        if (node.expr) {
            std::string retVal = eval(node.expr.get());
            VarType rt = VarType::UNKNOWN;
            if (!currentFunctionName.empty()) {
                rt = getVarType(currentFunctionName);
            }

            if (rt == VarType::STRING || rt == VarType::PTR || rt == VarType::RECORD) {
                emit2("mov", "arg0", retVal);
            } else if (rt == VarType::DOUBLE) {
                emit2("mov", "xmm0", retVal);
            } else {
                emit2("mov", "rax", retVal);
            }

            functionSetReturn = true;
            if (isReg(retVal) && !isParmReg(retVal))
                freeReg(retVal);
        }
        std::string endLabel = getCurrentEndLabel();
        if (!endLabel.empty()) {
            emit1("jmp", endLabel);
        } else {
            emit("ret");
        }
    }

    void CodeGenVisitor::visit(BreakNode &node) {
        if (!loopEndLabels.empty()) {
            emit1("jmp", loopEndLabels.back());
        }
    }

    void CodeGenVisitor::visit(ContinueNode &node) {
        if (!loopContinueLabels.empty()) {
            emit1("jmp", loopContinueLabels.back());
        }
    }

    void CodeGenVisitor::visit(NilNode &node) {
        pushValue("0");
    }

    void CodeGenVisitor::visit(PointerTypeNode &node) {
        // Handled in VarDeclNode visitor
    }

    void CodeGenVisitor::visit(PointerDerefNode &node) {
        // Evaluate the pointer expression
        std::string ptrVal = eval(node.pointer.get());

        // Determine the pointed-to type
        VarType derefType = VarType::INT;
        if (auto varNode = dynamic_cast<VariableNode *>(node.pointer.get())) {
            derefType = getPointerDerefType(varNode->name);
        }

        std::string base = ensurePtrBase(ptrVal);

        if (derefType == VarType::DOUBLE) {
            std::string result = allocFloatReg();
            emit4("load", result, base, "0", "1");
            pushValue(result);
        } else if (derefType == VarType::PTR || derefType == VarType::STRING) {
            std::string result = allocTempPtr();
            emit4("load", result, base, "0", "1");
            pushValue(result);
        } else {
            std::string result = allocReg();
            emit4("load", result, base, "0", "1");
            pushValue(result);
        }

        if (isReg(ptrVal) && !isParmReg(ptrVal) && ptrVal != base)
            freeReg(ptrVal);
    }

    void CodeGenVisitor::visit(AddressOfNode &node) {
        if (auto varNode = dynamic_cast<VariableNode *>(node.operand.get())) {
            std::string mangled = findMangledName(varNode->name);
            std::string result = allocTempPtr();
            emit2("lea", result, mangled);
            pushValue(result);
        } else {
            throw std::runtime_error("@ operator requires a variable operand");
        }
    }

    void CodeGenVisitor::visit(WithStmtNode &node) {
        std::string recType = getVarRecordTypeName(node.recordVar);
        recType = resolveTypeName(lc(recType));
        if (recType.empty()) {
            throw std::runtime_error("'with' variable '" + node.recordVar + "' is not a record type");
        }
        auto it = recordTypes.find(recType);
        if (it == recordTypes.end()) {
            throw std::runtime_error("Unknown record type '" + recType + "' in with statement");
        }
        std::unordered_map<std::string, std::string> fieldMap;
        for (const auto &[fieldName, idx] : it->second.nameToIndex) {
            fieldMap[fieldName] = node.recordVar;
        }
        withFieldScopes.push_back(std::move(fieldMap));
        if (node.statement)
            node.statement->accept(*this);
        withFieldScopes.pop_back();
    }

    void CodeGenVisitor::visit(GotoStmtNode &node) {
        std::string lbl;
        auto it = gotoLabels.find(node.label);
        if (it != gotoLabels.end()) {
            lbl = it->second;
        } else {
            lbl = newLabel("GOTO_" + node.label);
            gotoLabels[node.label] = lbl;
        }
        emit1("jmp", lbl);
    }

    void CodeGenVisitor::visit(LabelStmtNode &node) {
        std::string lbl;
        auto it = gotoLabels.find(node.label);
        if (it != gotoLabels.end()) {
            lbl = it->second;
        } else {
            lbl = newLabel("GOTO_" + node.label);
            gotoLabels[node.label] = lbl;
        }
        emitLabel(lbl);
        if (node.statement)
            node.statement->accept(*this);
    }

    void CodeGenVisitor::visit(SetLiteralNode &node) {
        // Build a runtime set value: allocate 256 bytes, zero-fill, then set each element
        std::string setPtr = allocTempPtr();
        emit3("alloc", setPtr, "8", "256");

        // Zero-fill
        std::string loopIdx = allocReg();
        std::string loopStart = newLabel("SET_LIT_INIT");
        std::string loopEnd = newLabel("SET_LIT_INIT_END");
        emit2("mov", loopIdx, "0");
        emitLabel(loopStart);
        emit2("cmp", loopIdx, "256");
        emit1("jge", loopEnd);
        emit4("store", "0", setPtr, loopIdx, "8");
        emit2("add", loopIdx, "1");
        emit1("jmp", loopStart);
        emitLabel(loopEnd);
        freeReg(loopIdx);

        // Set each element
        for (auto &elem : node.elements) {
            std::string elemVal = eval(elem.get());
            emit4("store", "1", setPtr, elemVal, "8");
            if (isReg(elemVal) && !isParmReg(elemVal))
                freeReg(elemVal);
        }
        pushValue(setPtr);
    }

    void CodeGenVisitor::visit(SetTypeNode &node) {
        // Set type declarations handled via type aliases; nothing to emit here
    }

    void CodeGenVisitor::visit(EnumTypeDeclNode &node) {
        auto lc = [](std::string s) {
            std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) { return std::tolower(c); });
            return s;
        };
        std::string typeName = lc(node.typeName);
        std::vector<std::string> valueNames;
        for (int i = 0; i < static_cast<int>(node.values.size()); ++i) {
            std::string valName = lc(node.values[i]);
            enumConstants[valName] = i;
            compileTimeConstants[valName] = std::to_string(i);
            valueNames.push_back(valName);
        }
        enumTypes[typeName] = std::move(valueNames);
        // Treat enum type as an alias for integer
        typeAliases[typeName] = "integer";
    }

} // namespace pascal