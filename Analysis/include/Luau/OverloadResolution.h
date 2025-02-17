// This file is part of the Luau programming language and is licensed under MIT License; see LICENSE.txt for details
#pragma once

#include "Luau/Ast.h"
#include "Luau/InsertionOrderedMap.h"
#include "Luau/NotNull.h"
#include "Luau/TypeFwd.h"
#include "Luau/Location.h"
#include "Luau/Error.h"
#include "Luau/Subtyping.h"

namespace Luau
{

struct BuiltinTypes;
struct TypeArena;
struct Scope;
struct InternalErrorReporter;
struct TypeCheckLimits;
struct Subtyping;

class Normalizer;

struct OverloadResolver
{
    enum Analysis
    {
        Ok,
        TypeIsNotAFunction,
        ArityMismatch,
        OverloadIsNonviable, // Arguments were incompatible with the overloads parameters but were otherwise compatible by arity
    };

    OverloadResolver(NotNull<BuiltinTypes> builtinTypes, NotNull<TypeArena> arena, NotNull<Normalizer> normalizer, NotNull<Scope> scope,
        NotNull<InternalErrorReporter> reporter, NotNull<TypeCheckLimits> limits, Location callLocation);

    NotNull<BuiltinTypes> builtinTypes;
    NotNull<TypeArena> arena;
    NotNull<Normalizer> normalizer;
    NotNull<Scope> scope;
    NotNull<InternalErrorReporter> ice;
    NotNull<TypeCheckLimits> limits;
    Subtyping subtyping;
    Location callLoc;

    // Resolver results
    std::vector<TypeId> ok;
    std::vector<TypeId> nonFunctions;
    std::vector<std::pair<TypeId, ErrorVec>> arityMismatches;
    std::vector<std::pair<TypeId, ErrorVec>> nonviableOverloads;
    InsertionOrderedMap<TypeId, std::pair<OverloadResolver::Analysis, size_t>> resolution;


    std::pair<OverloadResolver::Analysis, TypeId> selectOverload(TypeId ty, TypePackId args);
    void resolve(TypeId fnTy, const TypePack* args, AstExpr* selfExpr, const std::vector<AstExpr*>* argExprs);

private:
    std::optional<ErrorVec> testIsSubtype(const Location& location, TypeId subTy, TypeId superTy);
    std::optional<ErrorVec> testIsSubtype(const Location& location, TypePackId subTy, TypePackId superTy);
    std::pair<Analysis, ErrorVec> checkOverload(
        TypeId fnTy, const TypePack* args, AstExpr* fnLoc, const std::vector<AstExpr*>* argExprs, bool callMetamethodOk = true);
    static bool isLiteral(AstExpr* expr);
    LUAU_NOINLINE
    std::pair<Analysis, ErrorVec> checkOverload_(
        TypeId fnTy, const FunctionType* fn, const TypePack* args, AstExpr* fnExpr, const std::vector<AstExpr*>* argExprs);
    size_t indexof(Analysis analysis);
    void add(Analysis analysis, TypeId ty, ErrorVec&& errors);
};

} // namespace Luau
