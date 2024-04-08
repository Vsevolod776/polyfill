/*
 * VsevolodWTF 2023
 */
#ifndef _OPTIONAL_H
#define _OPTIONAL_H

#include "utility.h"

class bad_optional_access
	: public std::exception
{
protected:
	//bad_expected_access() noexcept : std::exception() { }
	//bad_optional_access( const bad_optional_access& other ) noexcept;
	//bad_optional_access& operator=( const bad_optional_access& other ) noexcept;
	virtual const char* what() const noexcept override
	{
		return "bad optional access";
	}
};

template< class T >
struct optional
{
	using value_type = T;

	constexpr optional() noexcept
		: _no_init(), _has_value( false )
	{ }
	constexpr optional( nullopt_t ) noexcept
		: _no_init(), _has_value( false )
	{ }
	//template< class = std::enable_if_t<std::is_copy_constructible<T>::value> >
	constexpr optional( const optional &o )
		: _no_init(), _has_value( o._has_value )
	{
		if ( _has_value ) new(std::addressof( _value )) T( o._value );
	}
	//template< class = std::enable_if_t<std::is_move_constructible<T>::value> >
	constexpr optional( optional &&o )
		: _no_init(), _has_value( o._has_value )
	{
		if ( _has_value ) new(std::addressof( _value )) T( std::move(o._value) );
	}
	template < class U = T, class = std::enable_if_t<std::is_constructible<T, U>::value> >	// std::is_constructible_v<T, U&&> ???
	constexpr optional( U&& value )
		: _value( std::forward<U>(value) ), _has_value(true)
	{ }
	template< class... Args, class = std::enable_if_t<std::is_constructible<T, Args...>::value> >
	constexpr explicit optional( in_place_t, Args&&... args )
		: _value( std::forward<Args>(args)... ), _has_value( true )
	{ }
	template < class U, class = std::enable_if_t<std::is_constructible<T, const U&>::value> >
	constexpr optional( const optional<U>& other )
		: _no_init(), _has_value( other._has_value )
	{
		if ( _has_value ) new(std::addressof( _value )) T( o._value );
	}
	template < class U, class = std::enable_if_t<std::is_constructible<T, U>::value> >	//std::is_constructible<T, U&&> ???
	constexpr optional( optional<U>&& other )
		: _no_init(), _has_value( other._has_value )
	{
		if ( _has_value ) new(std::addressof( _value )) T( std::move( o._value ) );
	}

	~optional() { reset(); }

	constexpr optional& operator=( nullopt_t ) noexcept
	{
		reset();
		return *this;
	}
	template< class = std::enable_if_t<std::is_copy_assignable<T>::value && std::is_copy_constructible<T>::value> >
	constexpr optional& operator=( const optional& other )
	{
		if ( other._has_value ) {
			if ( _has_value ) _value = other._value;
			else {
				new(std::addressof( _value )) T( other._value );
				_has_value = true;
				}
			}
		else reset();
		return *this;
	}

	template< class = std::enable_if_t<std::is_move_assignable<T>::value && std::is_move_constructible<T>::value> >
	constexpr optional& operator=( optional&& other ) noexcept
	{
		if ( other._has_value ) {
			if ( _has_value ) _value = std::move( other._value );
			else {
				new(std::addressof( _value )) T( std::move( other._value ) );
				_has_value = true;
				}
			}
		else reset();
		return *this;
	}
	template< class U = T, class = std::enable_if_t<std::is_assignable<T&, U>::value && std::is_constructible<T, U>::value> >
	constexpr optional& operator=( U&& value )
	{
		if ( _has_value ) _value = std::move( value );
		else {
			new(std::addressof( _value )) T( std::move( other._value ) );
			_has_value = true;
			};
		return *this;
	}
	template< class U, class = std::enable_if_t<std::is_assignable<T&, const U&>::value && std::is_constructible<T, const U&>::value> >
	constexpr optional& operator=( const optional<U>& other )
	{
		if ( other._has_value ) {
			if ( _has_value ) _value = other._value;
			else {
				new(std::addressof( _value )) T( other._value );
				_has_value = true;
				}
			}
		else reset();
		return *this;
	}
	template< class U, class = std::enable_if_t<std::is_assignable<T&, U>::value && std::is_constructible<T, U>::value> >
	constexpr optional& operator=( optional<U>&& other )
	{
		if ( other._has_value ) {
			if ( _has_value ) _value = std::move( other._value );
			else {
				new(std::addressof( _value )) T( std::move( other._value ) );
				_has_value = true;
				}
			}
		else reset();
		return *this;
	}

	constexpr bool has_value() const noexcept { return _has_value; }
	constexpr explicit operator bool() const noexcept { return _has_value; }

	constexpr const T& value() const &
	{
		if ( _has_value ) return _value;
		throw bad_optional_access();
	}
	constexpr T& value() &
	{
		if ( _has_value ) return _value;
		throw bad_optional_access();
	}
	constexpr const T&& value() const &&
	{
		if ( _has_value ) return std::move( _value );
		throw bad_optional_access();
	}
	constexpr T&& value() &&
	{
		if ( _has_value ) return std::move( _value );
		throw bad_optional_access();
	}

	template< class U = T, class = std::enable_if_t<std::is_copy_constructible<T>::value && std::is_convertible<U&&, T>::value> >
	constexpr T value_or( U&& u ) const &
	{
		return _has_value ? _value : static_cast<T>(std::forward<U>( u ));
	}
	template< class U = T, class = std::enable_if_t<std::is_move_constructible<T>::value && std::is_convertible<U&&, T>::value> >
	constexpr T value_or( U&& u ) const &&
	{
		return _has_value ? std::move( _value ) : static_cast<T>(std::forward<U>( u ));
	}

	constexpr const T* operator->() const { return &_value; }
	constexpr T* operator->() { return &_value; }
	constexpr const T& operator*() const & { return _value; }
	constexpr T& operator*() & { return _value; }
	constexpr const T&& operator*() const && { return std::move( _value ); }
	constexpr T&& operator*() && { return std::move( _value ); }

	constexpr void reset() noexcept
	{
		if ( _has_value ) {
			_value.~T();
			_has_value = false;
			}
	}

	union {	char _no_init; T _value; };
	bool _has_value;
};

template< class T >
constexpr optional<std::decay_t<T>> make_optional( T&& value )
{
	return { std::forward<T>( value ) };
}

template< class T, class... Args >
constexpr optional<T> make_optional( Args&&... args )
{
	return { in_place, std::forward<Args>( args )... };
}

template< class T, class U >
constexpr bool operator==( const optional<T>& l, const optional<U>& r )
{
	if ( l.has_value() ) {
		if ( r.has_value() ) return l.value()==r.value();
		return false;
		};
	return !r.has_value();
}

template< class T, class U >
constexpr bool operator!=( const optional<T>& l, const optional<U>& r )
{
	if ( l.has_value() ) {
		if ( r.has_value() ) return l.value()!=r.value();
		return true;
		};
	return r.has_value();
}

template< class T, class U >
constexpr bool operator==( const optional<T>& opt, const U& value )
{
	if ( opt.has_value() ) return opt.value()==value;
	return false;
}

template< class T, class U >
constexpr bool operator!=( const optional<T>& opt, const U& value )
{
	if ( opt.has_value() ) return opt.value()!=value;
	return false;
}

template< class T >
constexpr bool operator==( const optional<T>& opt, nullopt_t ) noexcept
{
	return !opt.has_value();
}

template< class T >
constexpr bool operator!=( const optional<T>& opt, nullopt_t ) noexcept
{
	return opt.has_value();
}

#endif
