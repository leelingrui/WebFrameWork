#ifndef TRAITS_H
#define TRAITS_H
namespace type_traits
{
	template<template<typename...> typename Target, typename T>
	struct is_base_template_of
	{
		static const bool value = false;
	};
	template<template<typename...> typename _Base, template<typename...> typename _Derived, typename...Args>
	struct is_base_template_of<_Base, _Derived<Args...>>
	{
		static const bool value = std::is_base_of<_Base<Args...>, _Derived<Args...>>::value;
	};
	template<template<class...> class Target, class T>
	struct is_template_of
	{
		static const bool value = false;
	};
	template<template<class...> class Target, class...Args>
	struct is_template_of<Target, Target<Args...>>
	{
		static const bool value = true;
	};
}
#endif