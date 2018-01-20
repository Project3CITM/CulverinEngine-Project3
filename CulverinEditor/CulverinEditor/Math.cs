using System;
using System.Runtime.CompilerServices;

namespace CulverinEditor
{
    public class Mathf
    {
        public const float PI = 3.14159274f;
        public const float Epsilon = 1e-06f;
        public const float RadToDeg = 57.295779513082320876f;
        private const float Deg2RadConst = 0.0174532924f;
        private const float Rad2DegConst = 57.29578f;

        public static float Abs(float s)
        {
            return Math.Abs(s);
        }

        public static float Acos(float s)
        {
            return (float)Math.Acos(s);
        }

        public static float Asin(float s)
        {
            return (float)Math.Asin(s);
        }

        public static float Atan(float s)
        {
            return (float)Math.Atan(s);
        }

        public static float Atan2(float x, float y)
        {
            return (float)Math.Atan2(x, y);
        }

        public static float Ceil(float s)
        {
            return (float)Math.Ceiling(s);
        }

        public static float Clamp(float val, float min, float max)
        {
            if (val < min)
            {
                return min;
            }
            else if (val > max)
            {
                return max;
            }

            return val;
        }

        public static float Cos(float s)
        {
            return (float)Math.Cos(s);
        }

        public static int Decimals(float step)
        {
            return Decimals(step);
        }

        public static int Decimals(decimal step)
        {
            return BitConverter.GetBytes(decimal.GetBits(step)[3])[2];
        }

        public static float Deg2rad(float deg)
        {
            return deg * Deg2RadConst;
        }

        public static float Exp(float s)
        {
            return (float)Math.Exp(s);
        }

        public static float Floor(float s)
        {
            return (float)Math.Floor(s);
        }

        public static int Max(int a, int b)
        {
            return (a > b) ? a : b;
        }

        public static float Max(float a, float b)
        {
            return (a > b) ? a : b;
        }

        public static int Min(int a, int b)
        {
            return (a < b) ? a : b;
        }

        public static float Min(float a, float b)
        {
            return (a < b) ? a : b;
        }

        public static float Pow(float x, float y)
        {
            return (float)Math.Pow(x, y);
        }

        public static float Rad2deg(float rad)
        {
            return rad * Rad2DegConst;
        }

        public static float Round(float s)
        {
            return (float)Math.Round(s);
        }

        public static float Sin(float s)
        {
            return (float)Math.Sin(s);
        }

        public static float Sinh(float s)
        {
            return (float)Math.Sinh(s);
        }

        public static float Sqrt(float s)
        {
            return (float)Math.Sqrt(s);
        }

        public static float Tan(float s)
        {
            return (float)Math.Tan(s);
        }

        public static float Tanh(float s)
        {
            return (float)Math.Tanh(s);
        }
    }
}
