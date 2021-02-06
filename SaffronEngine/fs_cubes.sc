$input v_color0

/*
 * Copyright 2011-2015 Branimir Karadzic. All rights reserved.
 * License: http://www.opensource.org/licenses/BSD-2-Clause
 */

#include "shaderBase.sc"

void main()
{
	vec4 color = v_color0;
	color.r = 0.0;
	gl_FragColor = color;
}
