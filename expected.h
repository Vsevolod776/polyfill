/*
 * VsevolodWTF 2023
 */
#ifndef _EXPECTED_H
#define _EXPECTED_H

#include "utility.h"

template< class E >
struct Unexpected
{
	template< class = std::enable_if_t<std::is_default_constructible<E>::value> >
	constexpr explicit Unexpected() : _error() { }
	constexpr Unexpected( const Unexpected& ) = default;
	constexpr Unexpected( Unexpected&& ) = default;
	template< class G = E, class = std::enable_if_t<std::is_constructible<E, G>::value> >
	constexpr explicit Unexpected( G&& e ) : _error( std::forward<G>( e ) ) { }
	template< class... Args, class = std::enable_if_t<std::is_constructible<E, Args...>::value> >
	constexpr explicit Unexpected( in_place_t, Args&&... args ) : _error( std::forward<Args>( args )... ) { }
	constexpr const E& error() const & noexcept { return _error; }
	constexpr E& error() & noexcept { return _error; }
	constexpr const E&& error() const && noexcept { return std::move(_error); }
	constexpr E&& error() && noexcept { return std::move(_error); }
	E _error;
};

template< class E >
class bad_expected_access
	: public bad_expected_access<void>
{
public:
	explicit bad_expected_access(E e)
		: bad_expected_access<void>()
		, _error( std::move(e) )
	{ }
	const E& error() const & noexcept { return _error; }
	E& error() & noexcept { return _error; }
	const E&& error() const && noexcept { return std::move( _error ); }
	E&& error() && noexcept { return std::move(_error); }
private:
	E _error;
};

template<>
class bad_expected_access<void>
	: public std::exception
{
protected:
	virtual const char* what() const noexcept override
	{
		return "bad expected access";
	}
};

template< class T, class E >
struct Expected
{
	using value_type = T;
	using error_type = E;
	using unexpected_type = Unexpected<E>;

	template< class = std::enable_if_t<std::is_default_constructible<T>::value> >
	constexpr Expected() noexcept
		: _value(), _has_value( true )
	{ }
	//template< class = std::enable_if_t<std::is_copy_constructible<T>::value && std::is_copy_constructible<E>::value> >
	constexpr Expected( const Expected &other )
		: _no_init(), _has_value( other._has_value )
	{
		if ( _has_value ) new(std::addressof( _value )) T( other._value );
		else new(std::addressof( _error )) E( other._error );
	}
	//template< class = std::enable_if_t<std::is_move_constructible<T>::value && std::is_move_constructible<E>::value> >
	constexpr Expected( Expected &&other )
		: _no_init(), _has_value( other._has_value )
	{
		if ( _has_value ) new(std::addressof( _value )) T( std::move( other._value ) );
		else new(std::addressof( _error )) E( std::move( other._error ) );
	}
	template< class U = T, class = std::enable_if_t<std::is_constructible<T, U>::value> >
	constexpr Expected( U&& u )	// explicit ???
		: _value( std::forward<U>( u ) ), _has_value( true )
	{ }
	template< class G = E, class = std::enable_if_t<std::is_constructible<E, const G&>::value> >
	constexpr Expected( const Unexpected<G> &g )
		: _error( g.error() ), _has_value( false )
	{ }
	template< class G = E, class = std::enable_if_t<std::is_constructible<E,G>::value> >
	constexpr Expected( Unexpected<G> &&g )
		: _error( std::move(std::forward<Unexpected<G>>( g ).error()) ), _has_value( false )
	{ }
	template< class... Args, class = std::enable_if_t<std::is_constructible<T, Args...>::value> >
	constexpr Expected( in_place_t, Args&& ...u )
		: _value( std::forward<Args>( u )... ), _has_value( true )
	{ }
	template< class... Args, class = std::enable_if_t<std::is_constructible<E, Args...>::value> >
	constexpr Expected( unexpect_t, Args&& ...g )
		: _error( std::forward<Args>( g )... ), _has_value( false )
	{ }

	~Expected() { _has_value ? _value.~T() : _error.~E(); }

	template< class = std::enable_if_t<
		std::is_copy_assignable<T>::value &&
		std::is_copy_constructible<T>::value &&
		std::is_copy_assignable<E>::value &&
		std::is_copy_constructible<E>::value
		> >
	constexpr Expected& operator=( const Expected &other )
	{
		if ( this == &other ) return *this;
		if ( other._has_value ) {
			if ( _has_value ) _value = other._value;
			else {
				_error.~E();
				new(std::addressof( _value )) T( other._value );
				_has_value = true;
				}
			}
		else {
			if ( _has_value ) {
				_value.~T();
				new(std::addressof( _error )) E( other._error );
				_has_value = false;
				}
			else _error = other._error;
			}
		return *this;
	}
	template< class = std::enable_if_t<
		std::is_move_assignable<T>::value &&
		std::is_move_constructible<T>::value &&
		std::is_move_assignable<E>::value &&
		std::is_move_constructible<E>::value
		> >
	constexpr Expected& operator=( Expected &&other ) noexcept
	{
		if ( this == &other ) return *this;
		if ( other._has_value ) {
			if ( _has_value ) _value = std::move(other._value);
			else {
				_error.~E();
				new(std::addressof( _value )) T( std::move(other._value) );
				_has_value = true;
				}
			}
		else {
			if ( _has_value ) {
				_value.~T();
				new(std::addressof( _error )) E( std::move(other._error) );
				_has_value = false;
				}
			else _error = std::move(other._error);
			}
		return *this;
	}
	template< class U, class = std::enable_if_t<std::is_constructible<T, U>::value && std::is_assignable<T&, U>::value> >
	constexpr Expected& operator=( U&& value )
	{
		if ( _has_value ) _value = std::move( value );
		else {
			_error.~E();
			_value = new(std::addressof(_value)) T( std::move( value ) );
			_has_value = true;
			};
		return *this;
	}
	template< class G, class = std::enable_if_t< std::is_constructible<E, const G&>::value && std::is_assignable<E&, const G&>::value > >
	constexpr Expected& operator=( const Unexpected<G>& g )
	{
		if ( _has_value ) {
			_value.~T();
			_error = new(std::addressof( _error )) E( g.error() );
			_has_value = false;
			}
		else _error = g.error();
		return *this;
	}
	template< class G, class = std::enable_if_t< std::is_constructible<E, G>::value && std::is_assignable<E&, G>::value> >
	constexpr Expected& operator=( Unexpected<G>&& g )
	{
		if ( _has_value ) {
			_value.~T();
			_error = new(std::addressof( _error )) E( std::move(std::forward<G>(g).error()) );
			_has_value = false;
			}
		else _error = std::move( std::forward<G>(g).error() );
		return *this;
	}

	constexpr bool has_value() const noexcept { return _has_value; }
	constexpr explicit operator bool() const noexcept { return _has_value; }

	constexpr const T& value() const &
	{
		if ( _has_value ) return _value;
		throw bad_expected_access<E>( error() );
	}
	constexpr T& value() &
	{
		if ( _has_value ) return _value;
		throw bad_expected_access<E>( error() );
	}
	constexpr const T&& value() const &&
	{
		if ( _has_value ) return std::move(_value);
		throw bad_expected_access<E>( std::move(error()) );
	}
	constexpr T&& value() &&
	{
		if ( _has_value ) return std::move( _value );
		throw bad_expected_access<E>( std::move( error() ) );
	}
	template< class U = T, class = std::enable_if_t<std::is_copy_constructible<T>::value && std::is_convertible<U&&, T>::value> >
	constexpr T value_or( U&& u ) const &
	{
		return _has_value ? _value : static_cast<T>(std::forward<U>( u ));
	}
	template< class U = T, class = std::enable_if_t<std::is_move_constructible<T>::value && std::is_convertible<U&&, T>::value> >
	constexpr T value_or( U&& u ) const &&
	{
		return _has_value ? std::move(_value) : static_cast<T>(std::forward<U>( u ));
	}
	constexpr const T* operator->() const { return &_value; }
	constexpr T* operator->() { return &_value; }
	constexpr const T& operator*() const & { return _value; }
	constexpr T& operator*() & { return _value; }
	constexpr const T&& operator*() const && { return std::move(_value); }
	constexpr T&& operator*() && { return std::move(_value); }

	constexpr const E& error() const & { return _error; }
	constexpr E& error() & { return _error; }
	constexpr const E&& error() const && { return std::move(_error); }
	constexpr E&& error() && { return std::move(_error); }

	union { char _no_init; T _value; E _error; };
	bool _has_value;
};

template< class T1, class E1, class T2, class E2 >
constexpr bool operator==( const Expected<T1, E1> &l, const Expected<T2, E2> &r )
{
	if ( l.has_value() ) {
		if ( r.has_value() ) return l.value()==r.value();
		}
	else {
		if ( !r.has_value() ) return l.error()==r.error();
		}
	return false;
}

template< class T1, class E1, class T2 >
constexpr bool operator==( const Expected<T1, E1> &e, const T2 &value )
{
	if ( e.has_value() ) return e.value()==value;
	return false;
}

template< class T1, class E1, class E2 >
constexpr bool operator==( const Expected<T1, E1> &e, const Unexpected<E2> &u )
{
	if ( !e.has_value() ) return e.error()==u.error();
	return false;
}

#endif
