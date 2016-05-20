// -*- C++ -*-
// Copyright (c) 2015, Just Software Solutions Ltd
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or
// without modification, are permitted provided that the
// following conditions are met:
//
// 1. Redistributions of source code must retain the above
// copyright notice, this list of conditions and the following
// disclaimer.
//
// 2. Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following
// disclaimer in the documentation and/or other materials
// provided with the distribution.
//
// 3. Neither the name of the copyright holder nor the names of
// its contributors may be used to endorse or promote products
// derived from this software without specific prior written
// permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
// CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
// INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
// NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
// HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
// OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
// EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#ifndef _JSS_EXPERIMENTAL_VARIANT_HEADER
#define _JSS_EXPERIMENTAL_VARIANT_HEADER
#include <stddef.h>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <new>
#include <utility>
#include <limits.h>

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4521)
#pragma warning(disable:4522)
#endif

namespace sdl {

template<typename _Type>
struct emplaced_type_t{};

template<ptrdiff_t _Index>
struct emplaced_index_t{};

class bad_variant_access: public std::logic_error{
public:
    explicit bad_variant_access(const std::string& what_arg):
        std::logic_error(what_arg)
    {}
    explicit bad_variant_access(const char* what_arg):
        std::logic_error(what_arg)
    {}
};

template<ptrdiff_t _Offset,typename _Type,typename ... _Types>
struct __type_index_helper;

template<ptrdiff_t _Offset,typename _Type,typename _Head,typename ... _Rest>
struct __type_index_helper<_Offset,_Type,_Head,_Rest...>{
    static constexpr ptrdiff_t __value=
        __type_index_helper<_Offset+1,_Type,_Rest...>::__value;
};

template<ptrdiff_t _Offset,typename _Type,typename ... _Rest>
struct __type_index_helper<_Offset,_Type,_Type,_Rest...>{
    static constexpr ptrdiff_t __value=_Offset;
};

template<typename _Type,typename ... _Types>
struct __type_index{
    static constexpr ptrdiff_t __value=
        __type_index_helper<0,_Type,_Types...>::__value;
};

template<ptrdiff_t _Index,typename ... _Types>
struct __indexed_type;

template<typename _Head,typename ... _Rest>
struct __indexed_type<0,_Head,_Rest...>{
    typedef _Head __type;
};

template<typename _Head,typename ... _Rest>
struct __indexed_type<-1,_Head,_Rest...>{
    typedef void __type;
};

template<ptrdiff_t _Index,typename _Head,typename ... _Rest>
struct __indexed_type<_Index,_Head,_Rest...>{
    typedef typename __indexed_type<_Index-1,_Rest...>::__type __type;
};

template<ptrdiff_t _Index,typename ..._Types>
struct __next_index{
    static constexpr ptrdiff_t __value=
        (_Index>=ptrdiff_t(sizeof...(_Types)-1))?-1:_Index+1;
};

template<typename ... _Types>
class variant;

template<typename _Type,typename ... _Types>
constexpr _Type& get(variant<_Types...>&);

template<typename _Type,typename ... _Types>
constexpr _Type const& get(variant<_Types...> const&);

template<ptrdiff_t _Index,typename ... _Types>
constexpr typename __indexed_type<_Index,_Types...>::__type& get(variant<_Types...>&);

template<ptrdiff_t _Index,typename ... _Types>
constexpr typename __indexed_type<_Index,_Types...>::__type const& get(
    variant<_Types...> const&);

template<ptrdiff_t _Index,typename ... _Types>
struct __variant_accessor;

template<size_t __count,
         bool __larger_than_char=(__count>SCHAR_MAX),
    bool __larger_than_short=(__count>SHRT_MAX),
    bool __larger_than_int=(__count>INT_MAX)>
struct __discriminator_type{
    typedef signed char __type;
};

template<size_t __count>
struct __discriminator_type<__count,true,false,false>{
    typedef signed short __type;
};

template<size_t __count>
struct __discriminator_type<__count,true,true,false>{
    typedef int __type;
};
template<size_t __count>
struct __discriminator_type<__count,true,true,true>{
    typedef signed long __type;
};

template<typename _Type>
struct __stored_type{
    typedef _Type __type;
};

template<typename _Type>
struct __stored_type<_Type&>{
    typedef _Type* __type;
};

template<typename ... _Types>
struct __all_trivially_destructible;

template<>
struct __all_trivially_destructible<> {
    static constexpr bool __value=true;
};

template<typename _Type>
struct __all_trivially_destructible<_Type> {
    static constexpr bool __value=
        std::is_trivially_destructible<typename __stored_type<_Type>::__type>::value;
};

template<typename _Head,typename ... _Rest>
struct __all_trivially_destructible<_Head,_Rest...> {
    static constexpr bool __value=
        __all_trivially_destructible<_Head>::__value &&
        __all_trivially_destructible<_Rest...>::__value;
};

template<typename _Target,typename ... _Args>
struct __storage_nothrow_constructible{
    static const bool __value=
        noexcept(_Target(std::declval<_Args>()...));
};

template<typename ... _Types>
struct __storage_nothrow_move_constructible;

template<>
struct __storage_nothrow_move_constructible<> {
    static constexpr bool __value=true;
};

template<typename _Type>
struct __storage_nothrow_move_constructible<_Type> {
    static constexpr bool __value=
        std::is_nothrow_move_constructible<
        typename __stored_type<_Type>::__type>::value;
};

template<typename _Head,typename ... _Rest>
struct __storage_nothrow_move_constructible<_Head,_Rest...> {
    static constexpr bool __value=
        __storage_nothrow_move_constructible<_Head>::__value &&
        __storage_nothrow_move_constructible<_Rest...>::__value;
};

template<ptrdiff_t _Index,typename ... _Types>
struct __other_storage_nothrow_move_constructible;

template<typename _Head,typename ... _Rest>
struct __other_storage_nothrow_move_constructible<0,_Head,_Rest...>{
    static const bool __value=__storage_nothrow_move_constructible<_Rest...>::__value;
};

template<typename _Head,typename ... _Rest>
struct __other_storage_nothrow_move_constructible<-1,_Head,_Rest...>{
    static const bool __value=
        __storage_nothrow_move_constructible<_Head,_Rest...>::__value;
};

template<ptrdiff_t _Index,typename _Head,typename ... _Rest>
struct __other_storage_nothrow_move_constructible<_Index,_Head,_Rest...>{
    static const bool __value=
        __storage_nothrow_move_constructible<_Head>::__value &&
        __other_storage_nothrow_move_constructible<_Index-1,_Rest...>::__value;
};

template<ptrdiff_t _Index,typename ... _Types>
struct __backup_storage_required{
    static const bool __value=
        !__storage_nothrow_move_constructible<
        typename __indexed_type<_Index,_Types...>::__type>::__value &&
        !__other_storage_nothrow_move_constructible<_Index,_Types...>::__value;
};

template<ptrdiff_t _Index,ptrdiff_t _Count,typename ... _Types>
struct __any_backup_storage_required_impl{
    static const bool __value=
        __backup_storage_required<_Index,_Types...>::__value ||
        __any_backup_storage_required_impl<_Index+1,_Count-1,_Types...>::__value;
};

template<ptrdiff_t _Index,typename ... _Types>
struct __any_backup_storage_required_impl<_Index,0,_Types...>{
    static const bool __value=false;
};

template<typename _Variant>
struct __any_backup_storage_required;

template<typename ... _Types>
struct __any_backup_storage_required<variant<_Types...> >{
    static const bool __value=
        __any_backup_storage_required_impl<0,sizeof...(_Types),_Types...>::__value;
};

template<typename ... _Types>
union __variant_data;

template<typename _Type,bool=std::is_literal_type<_Type>::value>
struct __variant_storage{
    typedef _Type __type;

    static constexpr _Type& __get(__type& __val){
        return __val;
    }
    static constexpr const _Type& __get(__type const& __val){
        return __val;
    }
    static void __destroy(__type&){}
};

template<typename _Type>
struct __storage_wrapper{
    typename std::aligned_storage<sizeof(_Type),alignof(_Type)>::type __storage;

    template<typename ... _Args>
    static constexpr void __construct(void* __p,_Args&& ... __args){
        new (__p) _Type(std::forward<_Args>(__args)...);
    }

    template <typename _Dummy = _Type>
    __storage_wrapper(
        typename std::enable_if<std::is_default_constructible<_Dummy>::value,
        void (__storage_wrapper::*)()>::type = nullptr) {
      __construct(&__storage);
    }

    template <typename _Dummy = _Type>
    __storage_wrapper(
        typename std::enable_if<!std::is_default_constructible<_Dummy>::value,
        void (__storage_wrapper::*)()>::type = nullptr) {
    }
    
    template<typename _First,typename ... _Args>
    __storage_wrapper(_First&& __first,_Args&& ... __args){
        __construct(&__storage,std::forward<_First>(__first),std::forward<_Args>(__args)...);
    }

    _Type& __get(){
        return *static_cast<_Type*>(static_cast<void*>(&__storage));
    }
    constexpr _Type const& __get() const{
        return *static_cast<_Type const*>(static_cast<void const*>(&__storage));
    }
    void __destroy(){
        __get().~_Type();
    }
};

template<typename _Type>
struct __storage_wrapper<_Type&>{
    _Type* __storage;

    template<typename _Arg>
    constexpr __storage_wrapper(_Arg& __arg):
        __storage(&__arg){}

    _Type& __get(){
        return *__storage;
    }
    constexpr _Type const& __get() const{
        return *__storage;
    }
};

template<typename _Type>
struct __variant_storage<_Type,false>{
    typedef __storage_wrapper<_Type> __type;

    static constexpr _Type& __get(__type& __val){
        return __val.__get();
    }
    static constexpr const _Type& __get(__type const& __val){
        return __val.__get();
    }
    static void __destroy(__type& __val){
        __val.__destroy();
    }
};

template<typename _Type,bool __b>
struct __variant_storage<_Type&,__b>{
    typedef __storage_wrapper<_Type&> __type;

    static constexpr _Type& __get(__type& __val){
        return __val.__get();
    }
    static constexpr const _Type& __get(__type const& __val){
        return __val.__get();
    }
    static void __destroy(__type&){}
};

template<>
union __variant_data<>{
    constexpr __variant_data(){}
};

template<typename _Type>
union __variant_data<_Type>{
    typename __variant_storage<_Type>::__type __val;
    struct __dummy_type{} __dummy;

    constexpr __variant_data():__dummy(){}

    template<typename ... _Args>
    constexpr __variant_data(emplaced_index_t<0>,_Args&& ... __args):
        __val(std::forward<_Args>(__args)...){}

    _Type& __get(emplaced_index_t<0>){
        return __variant_storage<_Type>::__get(__val);
    }
    constexpr const _Type& __get(emplaced_index_t<0>) const{
        return __variant_storage<_Type>::__get(__val);
    }
    void __destroy(emplaced_index_t<0>){
        __variant_storage<_Type>::__destroy(__val);
    }
};

template<typename _Head,typename ... _Rest>
union __variant_data<_Head,_Rest...>{
    __variant_data<_Head> __head;
    __variant_data<_Rest...> __rest;

    constexpr __variant_data():
        __head(){}

    template<typename ... _Args>
    constexpr __variant_data(emplaced_index_t<-1>,_Args&& ... __args):
        __head(emplaced_index_t<-1>(),std::forward<_Args>(__args)...){}
    
    template<typename ... _Args>
    constexpr __variant_data(emplaced_index_t<0>,_Args&& ... __args):
        __head(emplaced_index_t<0>(),std::forward<_Args>(__args)...){}
    template<ptrdiff_t _Index,typename ... _Args>
    constexpr __variant_data(emplaced_index_t<_Index>,_Args&& ... __args):
        __rest(emplaced_index_t<_Index-1>(),std::forward<_Args>(__args)...){}

    _Head& __get(emplaced_index_t<0>){
        return __head.__get(emplaced_index_t<0>());
    }

    constexpr const _Head& __get(emplaced_index_t<0>) const{
        return __head.__get(emplaced_index_t<0>());
    }

    template<ptrdiff_t _Index>
    typename __indexed_type<_Index-1,_Rest...>::__type& __get(
        emplaced_index_t<_Index>){
        return __rest.__get(emplaced_index_t<_Index-1>());
    }
    template<ptrdiff_t _Index>
    constexpr const typename __indexed_type<_Index-1,_Rest...>::__type& __get(
        emplaced_index_t<_Index>) const{
        return __rest.__get(emplaced_index_t<_Index-1>());
    }

    void __destroy(emplaced_index_t<0>){
        __head.__destroy(emplaced_index_t<0>());
    }
    template<ptrdiff_t _Index>
    void __destroy(emplaced_index_t<_Index>){
        __rest.__destroy(emplaced_index_t<_Index-1>());
    }
};


template<ptrdiff_t... _Indices>
struct __index_sequence{
    typedef __index_sequence<_Indices...,sizeof...(_Indices)> __next;
    static constexpr size_t __length=sizeof...(_Indices);
};

template<typename ... _Types>
struct __type_indices;

template<>
struct __type_indices<>{
    typedef __index_sequence<> __type;
};

template<typename _Type>
struct __type_indices<_Type>{
    typedef __index_sequence<0> __type;
};

template<typename _Type,typename ... _Rest>
struct __type_indices<_Type,_Rest...>{
    typedef typename __type_indices<_Rest...>::__type::__next __type;
};

template<typename _Variant>
struct __variant_indices;

template<typename ... _Types>
struct __variant_indices<variant<_Types...>>{
    typedef typename __type_indices<_Types...>::__type __type;
};

template<typename _Variant,
         typename _Indices=typename __variant_indices<_Variant>::__type>
struct __move_construct_op_table;

template<typename _Variant,ptrdiff_t ... _Indices>
struct __move_construct_op_table<_Variant,__index_sequence<_Indices...>>{
    typedef void(* const __func_type)(_Variant*,_Variant&);
    
    template<ptrdiff_t _Index>
    static void __move_construct_func(
        _Variant * __lhs,_Variant& __rhs){
        __lhs->template __emplace_construct<_Index>(
            std::move(get<_Index>(__rhs)));
    }

    static const __func_type __apply[sizeof...(_Indices)];
};

template<typename _Variant,ptrdiff_t ... _Indices>
const typename __move_construct_op_table<_Variant,__index_sequence<_Indices...>>::
__func_type
__move_construct_op_table<_Variant,__index_sequence<_Indices...>>::__apply[
    sizeof...(_Indices)]={
        &__move_construct_func<_Indices>...
    };

template<typename _Variant,
         typename _Indices=typename __variant_indices<_Variant>::__type>
struct __move_assign_op_table;

template<typename _Variant,ptrdiff_t ... _Indices>
struct __move_assign_op_table<_Variant,__index_sequence<_Indices...>>{
    typedef void(* const __func_type)(_Variant*,_Variant&);
    
    template<ptrdiff_t _Index>
    static void __move_assign_func(
        _Variant * __lhs,_Variant& __rhs){
        get<_Index>(*__lhs)=std::move(get<_Index>(__rhs));
    }

    static const __func_type __apply[sizeof...(_Indices)];
};

template<typename _Variant,ptrdiff_t ... _Indices>
const typename __move_assign_op_table<_Variant,__index_sequence<_Indices...>>::
__func_type
__move_assign_op_table<_Variant,__index_sequence<_Indices...>>::__apply[
    sizeof...(_Indices)]={
        &__move_assign_func<_Indices>...
    };

template<typename _Variant,
         typename _Indices=typename __variant_indices<_Variant>::__type>
struct __copy_construct_op_table;

template<typename _Variant,ptrdiff_t ... _Indices>
struct __copy_construct_op_table<_Variant,__index_sequence<_Indices...>>{
    typedef void(* const __func_type)(_Variant*,_Variant const&);
    
    template<ptrdiff_t _Index>
    static void __copy_construct_func(
        _Variant * __lhs,_Variant const& __rhs){
        __lhs->template __emplace_construct<_Index>(
            get<_Index>(__rhs));
    }

    static const __func_type __apply[sizeof...(_Indices)];
};

template<typename _Variant,ptrdiff_t ... _Indices>
const typename __copy_construct_op_table<_Variant,__index_sequence<_Indices...>>::
__func_type
__copy_construct_op_table<_Variant,__index_sequence<_Indices...>>::__apply[
    sizeof...(_Indices)]={
        &__copy_construct_func<_Indices>...
    };

template<typename _Variant,
         typename _Indices=typename __variant_indices<_Variant>::__type>
struct __copy_assign_op_table;

template<typename _Variant,ptrdiff_t ... _Indices>
struct __copy_assign_op_table<_Variant,__index_sequence<_Indices...>>{
    typedef void(* const __func_type)(_Variant*,_Variant const&);
    
    template<ptrdiff_t _Index>
    static void __copy_assign_func(
        _Variant * __lhs,_Variant const& __rhs){
        get<_Index>(*__lhs)=get<_Index>(__rhs);
    }

    static const __func_type __apply[sizeof...(_Indices)];
};

template<typename _Variant,ptrdiff_t ... _Indices>
const typename __copy_assign_op_table<_Variant,__index_sequence<_Indices...>>::
__func_type
__copy_assign_op_table<_Variant,__index_sequence<_Indices...>>::__apply[
    sizeof...(_Indices)]={
        &__copy_assign_func<_Indices>...
    };

template<typename _Variant,
         typename _Indices=typename __variant_indices<_Variant>::__type>
struct __destroy_op_table;

template<typename _Variant,ptrdiff_t ... _Indices>
struct __destroy_op_table<_Variant,__index_sequence<_Indices...>>{
    typedef void(* const __func_type)(_Variant*);
    
    template<ptrdiff_t _Index>
    static void __destroy_func(
        _Variant * __self){
        if(__self->__index>=0){
            __self->__storage.__destroy(emplaced_index_t<_Index>());
        }
    }

    static const __func_type __apply[sizeof...(_Indices)];
};

template<typename _Variant,ptrdiff_t ... _Indices>
const typename __destroy_op_table<_Variant,__index_sequence<_Indices...>>::
__func_type
__destroy_op_table<_Variant,__index_sequence<_Indices...>>::__apply[
    sizeof...(_Indices)]={
        &__destroy_func<_Indices>...
    };

template<typename _Variant,
         typename _Indices=typename __variant_indices<_Variant>::__type>
struct __swap_op_table;

template<typename _Variant,ptrdiff_t ... _Indices>
struct __swap_op_table<_Variant,__index_sequence<_Indices...>>{
    typedef void(* const __func_type)(_Variant&,_Variant&);
    
    template<ptrdiff_t _Index>
    static void __swap_func(
        _Variant & __lhs,_Variant & __rhs){
        swap(get<_Index>(__lhs),get<_Index>(__rhs));
    }

    static const __func_type __apply[sizeof...(_Indices)];
};

template<typename _Variant,ptrdiff_t ... _Indices>
const typename __swap_op_table<_Variant,__index_sequence<_Indices...>>::
__func_type
__swap_op_table<_Variant,__index_sequence<_Indices...>>::__apply[
    sizeof...(_Indices)]={
        &__swap_func<_Indices>...
    };

template<typename _Variant,
         typename _Indices=typename __variant_indices<_Variant>::__type>
struct __equality_op_table;

template<typename _Variant,ptrdiff_t ... _Indices>
struct __equality_op_table<_Variant,__index_sequence<_Indices...>>{
    typedef bool(* const __compare_func_type)(_Variant const&,_Variant const&);
    
    template<ptrdiff_t _Index>
    static bool __equality_compare_func(
        _Variant const& __lhs,_Variant const& __rhs){
        return get<_Index>(__lhs)==get<_Index>(__rhs);
    }

    static const __compare_func_type __equality_compare[sizeof...(_Indices)];
};

template<typename _Variant,ptrdiff_t ... _Indices>
const typename __equality_op_table<_Variant,__index_sequence<_Indices...>>::
__compare_func_type
__equality_op_table<_Variant,__index_sequence<_Indices...>>::__equality_compare[
    sizeof...(_Indices)]={
        &__equality_compare_func<_Indices>...
    };

template<typename _Variant,
         typename _Indices=typename __variant_indices<_Variant>::__type>
struct __less_than_op_table;

template<typename _Variant,ptrdiff_t ... _Indices>
struct __less_than_op_table<_Variant,__index_sequence<_Indices...>>{
    typedef bool(* const __compare_func_type)(_Variant const&,_Variant const&);
    
    template<ptrdiff_t _Index>
    static bool __less_than_compare_func(
        _Variant const& __lhs,_Variant const& __rhs){
        return get<_Index>(__lhs)<get<_Index>(__rhs);
    }

    static const __compare_func_type __less_than_compare[sizeof...(_Indices)];
};

template<typename _Variant,ptrdiff_t ... _Indices>
const typename __less_than_op_table<_Variant,__index_sequence<_Indices...>>::
__compare_func_type
__less_than_op_table<_Variant,__index_sequence<_Indices...>>::__less_than_compare[
    sizeof...(_Indices)]={
        &__less_than_compare_func<_Indices>...
    };

template<typename _Variant>
struct __variant_storage_type;

template<typename _Derived,bool __trivial_destructor>
struct __variant_base
{
    ~__variant_base(){
        static_cast<_Derived*>(this)->__destroy_self();
    }
};

template<typename _Derived>
struct __variant_base<_Derived,true>{
};


template<ptrdiff_t _Offset,typename _CurrentSequence,
         typename _Type,typename ... _Types>
struct __all_indices_helper;

template<ptrdiff_t _Offset,ptrdiff_t ... _Indices,
         typename _Type,typename ... _Rest>
struct __all_indices_helper<
    _Offset,__index_sequence<_Indices...>,
    _Type,_Type,_Rest...>{
    typedef typename __all_indices_helper<
        _Offset+1,__index_sequence<_Indices...,_Offset>,_Type,_Rest...>::__type
        __type;
};

template<ptrdiff_t _Offset,typename _CurrentSequence,
         typename _Type,typename _Head,typename ... _Rest>
struct __all_indices_helper<_Offset,_CurrentSequence,_Type,_Head,_Rest...>{
    typedef typename __all_indices_helper<
        _Offset+1,_CurrentSequence,_Type,_Rest...>::__type __type;
};

template<ptrdiff_t _Offset,typename _CurrentSequence,typename _Type>
struct __all_indices_helper<_Offset,_CurrentSequence,_Type>{
    typedef _CurrentSequence __type;
};

template<typename _Type,typename ... _Types>
struct __all_indices{
    typedef typename __all_indices_helper<
        0,__index_sequence<>,_Type,_Types...>::__type __type;
};

template<typename ... _Sequences>
struct __combine_sequences;

template<ptrdiff_t ... _Indices1,ptrdiff_t ... _Indices2>
struct __combine_sequences<
    __index_sequence<_Indices1...>,__index_sequence<_Indices2...>>{
    typedef __index_sequence<_Indices1...,_Indices2...> __type;
};

template<typename _Sequence,typename ... _Rest>
struct __combine_sequences<_Sequence,_Rest...>{
    typedef typename __combine_sequences<
        _Sequence,
        typename __combine_sequences<_Rest...>::__type>::__type __type;
};

template<typename _Indices>
struct __first_index;

template<ptrdiff_t _FirstIndex,ptrdiff_t ... _Rest>
struct __first_index<__index_sequence<_FirstIndex,_Rest...>>{
    static constexpr ptrdiff_t __value=_FirstIndex;
};

template<ptrdiff_t _Offset,typename _CurrentSequence,
         typename _Type,typename ... _Types>
struct __constructible_matches_helper;

template<ptrdiff_t _Offset,typename _Sequence,typename _Type>
struct __constructible_matches_helper<
    _Offset,_Sequence,_Type>{
    typedef _Sequence __type;
};

template<bool _Accept,ptrdiff_t _Entry>
struct __sequence_or_empty{
    typedef __index_sequence<> __type;
};

template<ptrdiff_t _Entry>
struct __sequence_or_empty<true,_Entry>{
    typedef __index_sequence<_Entry> __type;
};

template<ptrdiff_t _Offset,typename _CurrentSequence,
         typename _Type,typename _Head,typename ... _Rest>
struct __constructible_matches_helper<
    _Offset,_CurrentSequence,_Type,_Head,_Rest...>{
    typedef
    typename __constructible_matches_helper<
        _Offset+1,
        typename __combine_sequences<
            _CurrentSequence,
            typename __sequence_or_empty<
                std::is_constructible<_Head,_Type>::value,
                _Offset>::__type>::__type,
        _Type,_Rest...>::__type __type;
};

template<typename _Type,typename ... _Types>
struct __constructible_matches{
    typedef typename __constructible_matches_helper<
        0,__index_sequence<>,_Type,_Types...>::__type __type;
};

template<typename _Type,typename ... _Types>
struct __type_index_to_construct{
    typedef typename __all_indices<_Type,_Types...>::__type __direct_matches;
    typedef typename __all_indices<
        typename std::remove_const<
            typename std::remove_reference<_Type>::type
            >::type,_Types...>::__type __value_matches;

    typedef typename __constructible_matches<_Type,_Types...>::__type
    __constructibles;

    static_assert(
        (__direct_matches::__length>0) ||
        (__value_matches::__length>0) ||
        (__constructibles::__length==1),
        "For conversion construction of variants, exactly one type must be constructible");
    
    typedef typename __combine_sequences<
        __direct_matches,__value_matches,
        __constructibles>::__type __all_matches;
    
    static constexpr ptrdiff_t __value=__first_index<__all_matches>::__value;
};

struct __replace_construct_helper{
    template<
        ptrdiff_t _Index,
        bool __construct_directly,
        bool __indexed_type_has_nothrow_move,
        bool __other_types_have_nothrow_move>
    struct __helper;

    template<typename _Variant,
             typename _Indices=typename __variant_indices<_Variant>::__type>
    struct __op_table;
};

template<
    ptrdiff_t _Index,
    bool __other_types_have_nothrow_move>
struct __replace_construct_helper::__helper<
    _Index,false,true,__other_types_have_nothrow_move>{

    template<typename _Variant,typename ... _Args>
    static void __trampoline(_Variant& __v,_Args&& ... __args){
        __v.template __two_stage_replace<_Index>(__args...);
    }
};

template<
    ptrdiff_t _Index,
    bool __indexed_type_has_nothrow_move,
    bool __other_types_have_nothrow_move>
struct __replace_construct_helper::__helper<
    _Index,true,__indexed_type_has_nothrow_move,
    __other_types_have_nothrow_move>{

    template<typename _Variant,typename ... _Args>
    static void __trampoline(_Variant& __v,_Args&& ... __args){
        __v.template __direct_replace<_Index>(std::forward<_Args>(__args)...);
    }
};


template<
    ptrdiff_t _Index>
struct __replace_construct_helper::__helper<
    _Index,false,false,true>{

    template<typename _Variant,typename ... _Args>
    static void __trampoline(_Variant& __v,_Args&& ... __args){
        __v.template __local_backup_replace<_Index>(std::forward<_Args>(__args)...);
    }
};

template<
    ptrdiff_t _Index>
struct __replace_construct_helper::__helper<
    _Index,false,false,false>{

    template<typename _Variant,typename ... _Args>
    static void __trampoline(_Variant& __v,_Args&& ... __args){
        __v.template __direct_replace<_Index>(std::forward<_Args>(__args)...);
    }
};

template<typename _Variant,ptrdiff_t ... _Indices>
struct __replace_construct_helper::__op_table<_Variant,__index_sequence<_Indices...>>{
    typedef void(* const __move_func_type)(_Variant*,_Variant&);
    typedef void(* const __copy_func_type)(_Variant*,_Variant const&);
    
    template<ptrdiff_t _Index>
    static void __move_assign_func(
        _Variant * __lhs,_Variant& __rhs){
        __lhs->template __replace_construct<_Index>(std::move(get<_Index>(__rhs)));
        __rhs.__destroy_self();
    }

    template<ptrdiff_t _Index>
    static void __copy_assign_func(
        _Variant * __lhs,_Variant const& __rhs){
        __lhs->template __replace_construct<_Index>(get<_Index>(__rhs));
    }
    
    static const __move_func_type __move_assign[sizeof...(_Indices)];
    static const __copy_func_type __copy_assign[sizeof...(_Indices)];
};

template<typename _Variant,ptrdiff_t ... _Indices>
const typename __replace_construct_helper::__op_table<
    _Variant,__index_sequence<_Indices...>>::__move_func_type
__replace_construct_helper::__op_table<
    _Variant,__index_sequence<_Indices...>>::__move_assign[
    sizeof...(_Indices)]={
        &__move_assign_func<_Indices>...
    };

template<typename _Variant,ptrdiff_t ... _Indices>
const typename __replace_construct_helper::__op_table<
    _Variant,__index_sequence<_Indices...>>::__copy_func_type
__replace_construct_helper::__op_table<
    _Variant,__index_sequence<_Indices...>>::__copy_assign[
    sizeof...(_Indices)]={
        &__copy_assign_func<_Indices>...
    };

template<ptrdiff_t _Index,ptrdiff_t _MaskIndex,typename _Storage>
struct __backup_storage_ops{
    static void __move_construct_func(
        _Storage * __dest,_Storage& __source){
        new(__dest) _Storage(
            emplaced_index_t<_Index>(),
            std::move(__source.__get(emplaced_index_t<_Index>())));
    }
    static void __destroy_func(_Storage * __obj){
        __obj->__destroy(emplaced_index_t<_Index>());
    };
};

template<ptrdiff_t _Index,typename _Storage>
struct __backup_storage_ops<_Index,_Index,_Storage>{
    static void __move_construct_func(
        _Storage * /*__dest*/,_Storage& /*__source*/){
        throw std::bad_alloc();
    };
    static void __destroy_func(_Storage * /*__obj*/){
        throw std::bad_alloc();
    };
};

template<ptrdiff_t _MaskIndex,typename _Storage,typename _Indices>
struct __backup_storage_op_table;

template<ptrdiff_t _MaskIndex,typename _Storage,ptrdiff_t ... _Indices>
struct __backup_storage_op_table<
    _MaskIndex,_Storage,__index_sequence<_Indices...> >
{
    typedef void (*__move_func_type)(_Storage * __dest,_Storage& __source);
    typedef void (*__destroy_func_type)(_Storage * __obj);
    
    template<size_t _Index>
    struct __helper{
        typedef __backup_storage_ops<_Index,_MaskIndex,_Storage> __ops;
    };

    static const __move_func_type __move_ops[sizeof...(_Indices)];
    static const __destroy_func_type __destroy_ops[sizeof...(_Indices)];
};

template<ptrdiff_t _MaskIndex,typename _Storage,ptrdiff_t ... _Indices>
const typename __backup_storage_op_table<
    _MaskIndex,_Storage,__index_sequence<_Indices...> >::__move_func_type
__backup_storage_op_table<
    _MaskIndex,_Storage,__index_sequence<_Indices...> >::__move_ops[
        sizeof...(_Indices)]={
        &__helper<_Indices>::__ops::__move_construct_func...
    };

template<ptrdiff_t _MaskIndex,typename _Storage,ptrdiff_t ... _Indices>
const typename __backup_storage_op_table<
    _MaskIndex,_Storage,__index_sequence<_Indices...> >::__destroy_func_type
__backup_storage_op_table<
    _MaskIndex,_Storage,__index_sequence<_Indices...> >::__destroy_ops[
    sizeof...(_Indices)]={
        &__helper<_Indices>::__ops::__destroy_func...
    };

template<ptrdiff_t _Index,typename ... _Types>
struct __backup_storage{
    typedef __variant_data<_Types...> __storage_type;

    typedef __backup_storage_op_table<
        _Index,__storage_type,typename __type_indices<_Types...>::__type>
    __op_table_type;

    ptrdiff_t __backup_index;
    __storage_type& __live_storage;
    __storage_type __backup;

    __backup_storage(ptrdiff_t __live_index_,__storage_type& __live_storage_):
        __backup_index(__live_index_),__live_storage(__live_storage_){
        if(__backup_index>=0){
            __op_table_type::__move_ops[__backup_index](
                &__backup,__live_storage);
            __op_table_type::__destroy_ops[__backup_index](
                &__live_storage);
        }
    }
    void __destroy(){
        if(__backup_index>=0)
            __op_table_type::__destroy_ops[__backup_index](
                &__backup);
        __backup_index=-1;
    }
    
    ~__backup_storage(){
        if(__backup_index>=0){
            __op_table_type::__move_ops[__backup_index](
                &__live_storage,__backup);
            __destroy();
        }
    }
};

template<typename ... _Types>
class variant:
        private __variant_base<
    variant<_Types...>,__all_trivially_destructible<_Types...>::__value>
{
    typedef __variant_base<variant<_Types...>,__all_trivially_destructible<_Types...>::__value> __base_type;
    friend __base_type;
    friend struct __copy_construct_op_table<variant>;
    friend struct __copy_assign_op_table<variant>;
    friend struct __move_construct_op_table<variant>;
    friend struct __move_assign_op_table<variant>;
    friend struct __destroy_op_table<variant>;
    
    template<ptrdiff_t _Index,typename ... _Types2>
    friend struct __variant_accessor;

    friend struct __replace_construct_helper;

    typedef __variant_data<_Types...> __storage_type;
    __storage_type __storage;
    typename __discriminator_type<sizeof ... (_Types)>::__type __index;

    template<ptrdiff_t _Index,typename ... _Args>
    ptrdiff_t __emplace_construct(_Args&& ... __args){
        new(&__storage) __storage_type(
            emplaced_index_t<_Index>(),std::forward<_Args>(__args)...);
        return  _Index;
    }
    
    void __destroy_self(){
        if(valueless_by_exception())
            return;
        __destroy_op_table<variant>::__apply[index()](this);
        __index=-1;
    }
    
    ptrdiff_t __move_construct(variant& __other){
        ptrdiff_t const __other_index=__other.index();
        if(__other_index==-1)
            return -1;
        __move_construct_op_table<variant>::__apply[__other_index](this,__other);
        __other.__destroy_self();
        return __other_index;
    }

    ptrdiff_t __copy_construct(variant const& __other){
        ptrdiff_t const __other_index=__other.index();
        if(__other_index==-1)
            return -1;
        __copy_construct_op_table<variant>::__apply[__other_index](this,__other);
        return __other_index;
    }

    template<ptrdiff_t _Index,typename ... _Args>
    void __replace_construct(_Args&& ... __args){
        typedef typename __indexed_type<_Index,_Types...>::__type __this_type;
        __replace_construct_helper::__helper<
            _Index,
            __storage_nothrow_constructible<__this_type,_Args...>::__value ||
            (sizeof...(_Types)==1),
            __storage_nothrow_move_constructible<__this_type>::__value,
            __other_storage_nothrow_move_constructible<
                _Index,_Types...>::__value
            >::__trampoline(*this,std::forward<_Args>(__args)...);
    }

    template<ptrdiff_t _Index,typename ... _Args>
    void __two_stage_replace(_Args&& ... __args){
        typedef typename __indexed_type<_Index,_Types...>::__type __type;
        __variant_data<__type> __local(
            emplaced_index_t<0>(),std::forward<_Args>(__args)...);
        __destroy_self();
        __emplace_construct<_Index>(
            std::move(__local.__get(emplaced_index_t<0>())));
        __index=_Index;
        __local.__destroy(emplaced_index_t<0>());
    }

    template<ptrdiff_t _Index,typename ... _Args>
    void __local_backup_replace(_Args&& ... __args){
        __backup_storage<_Index,_Types...> __backup(__index,__storage);
        __emplace_construct<_Index>(std::forward<_Args>(__args)...);
        __index=_Index;
        __backup.__destroy();
    }
    
    template<ptrdiff_t _Index,typename ... _Args>
    void __direct_replace(_Args&& ... __args) {
        __destroy_self();
        __emplace_construct<_Index>(std::forward<_Args>(__args)...);
        __index=_Index;
    }

public:
    constexpr variant()
        noexcept(noexcept(typename __indexed_type<0,_Types...>::__type())):
        __storage(emplaced_index_t<0>()),
        __index(0)
    {}

    constexpr variant(variant&& __other):
        __index(__move_construct(__other))
    {}

    constexpr variant(variant& __other):
        __index(__copy_construct(__other))
    {}

    constexpr variant(variant const& __other):
        __index(__copy_construct(__other))
    {}

    template<typename _Type,typename ... _Args>
    explicit constexpr variant(emplaced_type_t<_Type>,_Args&& ... __args):
        __storage(
            emplaced_index_t<__type_index<_Type,_Types...>::__value>(),
            std::forward<_Args>(__args)...),
        __index(__type_index<_Type,_Types...>::__value)
    {
        static_assert(std::is_constructible<_Type,_Args...>::value,"Type must be constructible from args");
    }

    template<ptrdiff_t _Index,typename ... _Args>
    explicit constexpr variant(emplaced_index_t<_Index>,_Args&& ... __args):
        __storage(emplaced_index_t<_Index>(),std::forward<_Args>(__args)...),
        __index(_Index)
    {
        static_assert(std::is_constructible<typename __indexed_type<_Index,_Types...>::__type,_Args...>::value,"Type must be constructible from args");
    }
    
    template<typename _Type>
    constexpr variant(_Type&& __x):
        __storage(
            emplaced_index_t<
            __type_index_to_construct<_Type,_Types...>::__value>(),
            std::forward<_Type>(__x)),
        __index(__type_index_to_construct<_Type,_Types...>::__value)
    {}

    template<typename _Type,
             typename _Enable=
             typename std::enable_if<
                 (__constructible_matches<std::initializer_list<_Type>,_Types...>::__type::__length>0)
             >::type>
    constexpr variant(std::initializer_list<_Type> __x):
        __storage(
            emplaced_index_t<
            __type_index_to_construct<std::initializer_list<_Type>,_Types...>::__value>(),
            __x),
        __index(__type_index_to_construct<std::initializer_list<_Type>,_Types...>::__value)
    {}
    
    template<typename _Type>
    variant& operator=(_Type&& __x){
        constexpr ptrdiff_t _Index=
            __type_index_to_construct<_Type,_Types...>::__value;
        if(_Index==__index){
            get<_Index>(*this)=std::forward<_Type>(__x);
        }
        else{
            __replace_construct<_Index>(std::forward<_Type>(__x));
        }
        return *this;
    }

    variant& operator=(variant const& __other){
        if(__other.valueless_by_exception()){
            __destroy_self();
        }
        else if(__other.index()==index()){
            __copy_assign_op_table<variant>::__apply[index()](this,__other);
        }
        else{
            __replace_construct_helper::__op_table<variant>::__copy_assign[
                __other.index()](this,__other);
        }
        return *this;
    }
    variant& operator=(variant& __other){
        if(__other.valueless_by_exception()){
            __destroy_self();
        }
        else if(__other.index()==index()){
            __copy_assign_op_table<variant>::__apply[index()](this,__other);
        }
        else{
            __replace_construct_helper::__op_table<variant>::__copy_assign[
                __other.index()](this,__other);
        }
        return *this;
    }
    variant& operator=(variant&& __other){
        if(__other.valueless_by_exception()){
            __destroy_self();
        }
        else if(__other.index()==index()){
            __move_assign_op_table<variant>::__apply[index()](this,__other);
            __other.__destroy_self();
        }
        else{
            __replace_construct_helper::__op_table<variant>::__move_assign[
                __other.index()](this,__other);
        }
        return *this;
    }

    template<typename _Type,typename ... _Args>
    void emplace(_Args&& ... __args){
        __direct_replace<__type_index<_Type,_Types...>::__value>(
            std::forward<_Args>(__args)...);
    }
    
    template<ptrdiff_t _Index,typename ... _Args>
    void emplace(_Args&& ... __args){
        __direct_replace<_Index>(std::forward<_Args>(__args)...);
    }
    
    constexpr bool valueless_by_exception() const noexcept{
        return __index==-1;
    }
    constexpr ptrdiff_t index() const noexcept{
        return __index;
    }

    void swap(variant& __other){
        if(__other.index()==index()){
            if(!valueless_by_exception())
                __swap_op_table<variant>::__apply[index()](*this,__other);
        }
        else{
            std::swap(*this,__other);
        }
    }
};

template<ptrdiff_t _Index,typename ... _Types>
struct __variant_accessor{
    typedef typename __indexed_type<_Index,_Types...>::__type __type;
    static constexpr __type& get(variant<_Types...>& __v){
        return __v.__storage.__get(emplaced_index_t<_Index>());
    }
    static constexpr __type const& get(variant<_Types...> const& __v){
        return __v.__storage.__get(emplaced_index_t<_Index>());
    }
};

template<typename _Indices,typename _Type,typename _Variant>
struct __get_by_type_helper;

template<typename _Type,typename _Variant>
struct __get_by_type_helper<__index_sequence<>,_Type,_Variant>{
    static _Type& __get(_Variant& /*__v*/){
        throw bad_variant_access("Bad variant type in get");
    }
    static const _Type& __get(_Variant const& /*__v*/){
        throw bad_variant_access("Bad variant type in get");
    }
};

template<ptrdiff_t _FirstIndex,ptrdiff_t ... _OtherIndices,
         typename _Type,typename _Variant>
struct __get_by_type_helper<
    __index_sequence<_FirstIndex,_OtherIndices...>,_Type,_Variant>{
    static constexpr _Type& __get(_Variant& __v){
        return (__v.index()==_FirstIndex)?
            get<_FirstIndex>(__v):
            __get_by_type_helper<
            __index_sequence<_OtherIndices...>,_Type,_Variant>::__get(__v);
    }
    static constexpr const _Type& __get(const _Variant& __v){
        return (__v.index()==_FirstIndex)?
            get<_FirstIndex>(__v):
            __get_by_type_helper<
            __index_sequence<_OtherIndices...>,_Type,_Variant>::__get(__v);
    }
};

template<typename _Type,typename ... _Types>
constexpr _Type& get(variant<_Types...>& __v){
    typedef typename __all_indices<_Type,_Types...>::__type __all_matches;
    return __get_by_type_helper<__all_matches,_Type,variant<_Types...>>::__get(__v);
}

template<typename _Type,typename ... _Types>
constexpr _Type const& get(variant<_Types...> const& __v){
    typedef typename __all_indices<_Type,_Types...>::__type __all_matches;
    return __get_by_type_helper<__all_matches,_Type,variant<_Types...>>::__get(__v);
}

template<ptrdiff_t _Index,typename ... _Types>
constexpr typename __indexed_type<_Index,_Types...>::__type const& get(
    variant<_Types...> const& __v){
    return *((_Index!=__v.index())?throw bad_variant_access("Bad variant index in get"):
        &__variant_accessor<_Index,_Types...>::get(__v));
}

template<ptrdiff_t _Index,typename ... _Types>
constexpr typename __indexed_type<_Index,_Types...>::__type& get(variant<_Types...>& __v){
    return *((_Index!=__v.index())?throw bad_variant_access("Bad variant index in get"):
        &__variant_accessor<_Index,_Types...>::get(__v));
}

template<typename _Type,typename ... _Types>
constexpr bool holds_alternative(variant<_Types...> const& __v) noexcept{
    return __v.index()==__type_index<_Type,_Types...>::__value;
}

template<typename _Visitor,typename ... _Types>
struct __visitor_return_type;

template<typename _Visitor>
struct __visitor_return_type<_Visitor>{
    typedef decltype(std::declval<_Visitor&>()()) __type;
};

template<typename _Visitor,typename _Head,typename ... _Rest>
struct __visitor_return_type<_Visitor,_Head,_Rest...>{
    typedef decltype(std::declval<_Visitor&>()(std::declval<_Head&>())) __type;
};

template<typename _Visitor,typename ... _Types>
struct __visitor_table{
    typedef variant<_Types...> __variant_type;
    typedef typename __visitor_return_type<_Visitor,_Types...>::__type __return_type;
    typedef __return_type (*__func_type)(_Visitor&,__variant_type&);

    template<typename _Type>
    static __return_type __trampoline_func(_Visitor& __visitor,__variant_type& __v){
        return __visitor(get<_Type>(__v));
    }

    static const __func_type __trampoline[sizeof...(_Types)];
};

template<typename _Visitor,typename ... _Types>
const typename __visitor_table<_Visitor,_Types...>::__func_type __visitor_table<_Visitor,_Types...>::__trampoline[sizeof...(_Types)]={
        &__trampoline_func<_Types>...
    };

template<typename _Visitor,typename ... _Types>
typename __visitor_return_type<_Visitor,_Types...>::__type
visit(_Visitor&& __visitor,variant<_Types...>& __v){
    if(__v.valueless_by_exception())
        throw bad_variant_access("Visiting of empty variant");
    return __visitor_table<_Visitor,_Types...>::__trampoline[__v.index()](__visitor,__v);
}

template<typename _Visitor,typename ... _Variants>
struct __multi_visitor_return_type{
    typedef decltype(std::declval<_Visitor&>()(get<0>(std::declval<_Variants>())...))
    __type;
};

template<size_t _VariantIndex,typename _Indices>
struct __visit_helper;

template<ptrdiff_t ... _Indices>
struct __visit_helper<0,__index_sequence<_Indices...>>{
    template<typename _Visitor,typename ... _Variants>
    static typename __multi_visitor_return_type<_Visitor,_Variants...>::__type
    __visit(_Visitor& __visitor,_Variants& ... __v){
        return __visitor(get<_Indices>(__v)...);
    }
};

template<size_t _Index,typename ... _Args>
struct __arg_selector_t;

template<typename _Head,typename ... _Rest>
struct __arg_selector_t<0,_Head,_Rest...>{
    typedef _Head __type;

    static __type& __select(_Head& __head,_Rest& ...){
        return __head;
    }
};

template<size_t _Index,typename _Head,typename ... _Rest>
struct __arg_selector_t<_Index,_Head,_Rest...>{
    typedef typename __arg_selector_t<_Index-1,_Rest...>::__type __type;
    static __type& __select(_Head&,_Rest& ... __rest){
        return __arg_selector_t<_Index-1,_Rest...>::__select(__rest...);
    }
};

template<size_t _Index,typename ... _Args>
typename __arg_selector_t<_Index,_Args...>::__type&& __arg_selector(_Args&& ... __args){
    return std::forward<typename __arg_selector_t<_Index,_Args...>::__type>(
        __arg_selector_t<_Index,_Args...>::__select(__args...));
}

template<ptrdiff_t _Index,size_t _VariantIndex,ptrdiff_t ... _Indices>
struct __visit_helper2{
    template<typename _Visitor,typename ... _Variants>
    static typename __multi_visitor_return_type<_Visitor,_Variants...>::__type
    __visit(_Visitor& __visitor,_Variants&& ... __v){
        if(__arg_selector<_VariantIndex-1>(__v...).index()==_Index){
            return __visit_helper<
                _VariantIndex-1,__index_sequence<_Index,_Indices...>>::
                __visit(__visitor,std::forward<_Variants>(__v)...);
        }
        else{
            return __visit_helper2<_Index-1,_VariantIndex,_Indices...>::__visit(
                __visitor,std::forward<_Variants>(__v)...);
        }
        
    }
};

template<size_t _VariantIndex,ptrdiff_t ... _Indices>
struct __visit_helper2<-1,_VariantIndex,_Indices...>{
    template<typename _Visitor,typename ... _Variants>
    static typename __multi_visitor_return_type<_Visitor,_Variants...>::__type
    __visit(_Visitor& /*__visitor*/,_Variants&& ... /*__v*/){
        throw bad_variant_access("Visiting of empty variant");
    }
};

template<typename _Variant>
struct __variant_type_count;

template<typename ... _Types>
struct __variant_type_count<variant<_Types...>>{
    static constexpr size_t __value=sizeof...(_Types);
};

template<typename _Variant>
struct __variant_type_count<_Variant&>{
    static constexpr size_t __value=__variant_type_count<_Variant>::__value;
};

template<typename _Variant>
struct __variant_type_count<_Variant const&>{
    static constexpr size_t __value=__variant_type_count<_Variant>::__value;
};

template<size_t _VariantIndex,ptrdiff_t ... _Indices>
struct __visit_helper<_VariantIndex,__index_sequence<_Indices...>>{

    template<typename _Visitor,typename ... _Variants>
    static typename __multi_visitor_return_type<_Visitor,_Variants...>::__type
    __visit(_Visitor& __visitor,_Variants&& ... __v){
        return __visit_helper2<
            __variant_type_count<
                typename __arg_selector_t<
                    _VariantIndex-1,_Variants...>::__type>::__value-1,
            _VariantIndex,_Indices...>::__visit(
                __visitor,std::forward<_Variants&&>(__v)...);
    }
};

template<typename _Visitor,typename ... _Variants>
typename __multi_visitor_return_type<_Visitor,_Variants...>::__type
visit(_Visitor&& __visitor,_Variants&& ... __v){
    return __visit_helper<sizeof...(_Variants),__index_sequence<>>::__visit(
        __visitor,std::forward<_Variants>(__v)...);
}

template<typename ... _Types>
bool operator==(variant<_Types...> const& __lhs,variant<_Types...> const& __rhs){
    return (__lhs.index()==__rhs.index()) &&
        ((__lhs.index()==-1) ||
         __equality_op_table<variant<_Types...>>::__equality_compare[__lhs.index()](
             __lhs,__rhs));
}

template<typename ... _Types>
bool operator!=(variant<_Types...> const& __lhs,variant<_Types...> const& __rhs){
    return !(__lhs==__rhs);
}

template<typename ... _Types>
bool operator<(variant<_Types...> const& __lhs,variant<_Types...> const& __rhs){
    return (__lhs.index()<__rhs.index()) ||
        ((__lhs.index()==__rhs.index()) &&
         ((__lhs.index()!=-1) &&
          __less_than_op_table<variant<_Types...>>::
          __less_than_compare[__lhs.index()](__lhs,__rhs)));
}

template<typename ... _Types>
bool operator>(variant<_Types...> const& __lhs,variant<_Types...> const& __rhs){
    return __rhs<__lhs;
}

template<typename ... _Types>
bool operator<=(variant<_Types...> const& __lhs,variant<_Types...> const& __rhs){
    return !(__rhs<__lhs);
}

template<typename ... _Types>
bool operator>=(variant<_Types...> const& __lhs,variant<_Types...> const& __rhs){
    return !(__lhs>__rhs);
}

} // end namespace sdl

#ifdef _MSC_VER
#pragma warning(pop)
#endif

#endif
