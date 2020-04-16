#pragma once

#define VEWND_EXCEPT(hr) Window::Exception(__LINE__,__FILE__,hr);
#define VEWND_LAST_EXCEPT() Window::Exception(__LINE__,__FILE__,GetLastError());
