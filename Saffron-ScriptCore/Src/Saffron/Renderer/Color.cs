using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Se
{
    public class Color
    {
        public float r, g, b, a;

        public Color(float r, float g, float b, float a)
        {
            this.r = r;
            this.g = g;
            this.b = b;
            this.a = a;
        }

        public static Color Black { get { return new Color(0.0f, 0.0f, 0.0f, 1.0f); } }
        public static Color Blue { get { return new Color(0.0f, 0.0f, 1.0f, 1.0f); } }
        public static Color Green { get { return new Color(0.0f, 1.0f, 0.0f, 1.0f); } }
        public static Color Teal { get { return new Color(0.0f, 1.0f, 1.0f, 1.0f); } }
        public static Color Red { get { return new Color(1.0f, 0.0f, 0.0f, 1.0f); } }
        public static Color Magenta { get { return new Color(1.0f, 0.0f, 1.0f, 1.0f); } }
        public static Color Yellow { get { return new Color(1.0f, 1.0f, 0.0f, 1.0f); } }
        public static Color White { get { return new Color(1.0f, 1.0f, 1.0f, 1.0f); } }

    }
}
