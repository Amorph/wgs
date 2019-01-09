#pragma once

#include <functional>

namespace WGSE
{
	template<class Ret>
	struct FuncPtr 
	{
		const std::function<Ret()>* ptr;
	};

	template<class Ret, typename... Args>
	struct FuncPtr<Ret(Args...)> 
	{
		const std::function<Ret(Args...)>* ptr;
	};
}
