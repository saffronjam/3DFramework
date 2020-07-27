#pragma once

#define BIT(x) (1 << x)
#define TO_STRING(Var) #Var
#define EVENT_FN(fn) std::bind(&fn, this, std::placeholders::_1)