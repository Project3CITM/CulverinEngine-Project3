
namespace CulverinEditor
{
    class Quaternion
    {
        public Quaternion()
        {
            x = 0.0f;
            y = 0.0f;
            z = 0.0f;
            w = 0.0f;
        }

        public Quaternion(float x, float y, float z, float w)
        {
            this.x = x;
            this.y = y;
            this.z = z;
            this.w = w;
        }

        public float x;
        public float y;
        public float z;
        public float w;

        public static Quaternion Identity
        {
            get
            {
                return new Quaternion(0.0f, 0.0f, 0.0f, 1.0f);
            }
        }

        public void Set(float x, float y, float z, float w)
        {
            this.x = x;
            this.y = y;
            this.z = z;
            this.w = w;
        }

        public Vector3 ToEulerAngles()
        {
            Vector3 ret = new Vector3
            {
                x = Mathf.Atan2(2 * (x * y + w * z), w * w + x * x - y * y - z * z),
                y = Mathf.Asin(-2 * (x * z - w * y)),
                z = Mathf.Atan2(2 * (y * z + w * x), w * w - x * x - y * y + z * z)
            };
            return ret;
        }

        public static Quaternion FromEulerAngles(Vector3 angles)
        {
            float cos_z_2 = Mathf.Cos(0.5f * angles.z);
            float cos_y_2 = Mathf.Cos(0.5f * angles.y);
            float cos_x_2 = Mathf.Cos(0.5f * angles.x);

            float sin_z_2 = Mathf.Sin(0.5f * angles.z);
            float sin_y_2 = Mathf.Sin(0.5f * angles.y);
            float sin_x_2 = Mathf.Sin(0.5f * angles.x);

            Quaternion ret = new Quaternion
            {
                x = cos_z_2 * cos_y_2 * sin_x_2 - sin_z_2 * sin_y_2 * cos_x_2,
                y = cos_z_2 * sin_y_2 * cos_x_2 + sin_z_2 * cos_y_2 * sin_x_2,
                z = sin_z_2 * cos_y_2 * cos_x_2 - cos_z_2 * sin_y_2 * sin_x_2,
                w = cos_z_2 * cos_y_2 * cos_x_2 + sin_z_2 * sin_y_2 * sin_x_2
            };

            return ret;
        }

        public static Quaternion Lerp(Quaternion a, Quaternion b, float value)
        {

            return (a * (1 - value) + b * value).Normalized;
        }

        public static float DotProduct(Quaternion a, Quaternion b)
        {
            return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
        }

        public static Quaternion Slerp(Quaternion a, Quaternion b, float value)
        {
            Quaternion result;
            float dot = DotProduct(a, b);

            if (dot < 0)
            {
                dot = -dot;
                result = -b;
            }
            else
            {
                result = b;
            }

            if (dot < 0.95f)
            {
                float angle = Mathf.Acos(dot);
                return (a * Mathf.Sin(angle * (1 - value)) + result * Mathf.Sin(angle * value)) / Mathf.Sin(angle);
            }
            else
            {
                return Lerp(a, result, value);
            }
        }

        public static float AngleBetween(Quaternion a, Quaternion b)
        {
            float dot = DotProduct(a, b);
            return Mathf.Acos(Mathf.Min(Mathf.Abs(dot), 1f)) * 2f * Mathf.RadToDeg;
        }

        public static Quaternion RotateTowards(Quaternion from, Quaternion to, float step)
        {
            float angle = AngleBetween(from, to);
            Quaternion result;
            if (angle == 0f)
            {
                result = to;
            }
            else
            {
                float value = Mathf.Min(1f, step / angle);
                result = Slerp(from, to, value);
            }
            return result;
        }

        public static Quaternion FromAngleAxis(float angle, Vector3 axis)
        {
            Vector3 tmp = Mathf.Sin(angle / 2) * axis; //e
            Quaternion ret = new Quaternion
            {
                x = tmp.x,
                y = tmp.y,
                z = tmp.z,
                w = Mathf.Cos(angle / 2)
            };

            return ret;
        }

        public void ToAngleAxis(out float angle, out Vector3 axis)
        {
            Vector3 tmp = new Vector3(x, y, z);
            ToAngleAxis(this, out angle, out axis);
        }

        public static void ToAngleAxis(Quaternion a, out float angle, out Vector3 axis)
        {
            angle = Mathf.Acos(a.w);

            float sin_theta_inv = 1.0f / Mathf.Sin(angle);

            axis = new Vector3
            {
                x = a.x * sin_theta_inv,
                y = a.y * sin_theta_inv,
                z = a.z * sin_theta_inv
            };

            angle *= 2;
        }

        public float Length
        {
            get
            {
                return Magnitude(this);
            }
        }

        public float LengthSquared
        {
            get
            {
                return x * x + y * y + z * z + w * w;
            }
        }

        public static float Magnitude(Quaternion quaternion)
        {
            return Mathf.Sqrt(quaternion.x * quaternion.x + quaternion.y * quaternion.y + quaternion.z * quaternion.z + quaternion.w * quaternion.w);
        }

        public Quaternion Normalized
        {
            get
            {
                return Normalize(this);
            }
        }

        public static Quaternion Normalize(Quaternion quaternion)
        {
            Quaternion ret;
            float magnitude = Magnitude(quaternion);
            if (magnitude > 1E-05f)
            {
                ret = quaternion / magnitude;
            }
            else
            {
                ret = Identity;
            }
            return ret;
        }

        public Quaternion Conjugate()
        {
            return new Quaternion(-x, -y, -z, w);
        }

        public Quaternion Inverse()
        {
            return Conjugate() / LengthSquared;
        }

        public static Quaternion operator +(Quaternion a, Quaternion b)
        {
            return new Quaternion(a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w);
        }

        public static Quaternion operator -(Quaternion a, Quaternion b)
        {
            return new Quaternion(a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w);
        }

        public static Quaternion operator -(Quaternion a)
        {
            return new Quaternion(-a.x, -a.y, -a.z, -a.w);
        }

        public static Quaternion operator *(Quaternion a, Quaternion b)
        {
            return new Quaternion(
                a.w * b.w - a.x * b.x - a.y * b.y - a.z * b.z,
                a.w * b.x + a.x * b.w + a.y * b.z - a.z * b.y,
                a.w * b.y - a.x * b.z + a.y * b.w + a.z * b.x,
                a.w * b.z + a.x * b.y - a.y * b.x + a.z * b.w
            );
        }

        public static Quaternion operator *(Quaternion a, float value)
        {
            return new Quaternion(a.x * value, a.y * value, a.z * value, a.w * value);
        }

        public static Quaternion operator /(Quaternion a, float value)
        {
            return new Quaternion(a.x / value, a.y / value, a.z / value, a.w / value);
        }

        public static Quaternion operator /(Quaternion a, Quaternion b)
        {
            return a * b.Inverse();
        }

        public static bool operator ==(Quaternion a, Quaternion b)
        {
            return DotProduct(a, b) > 0.999999f;
        }

        public static bool operator !=(Quaternion a, Quaternion b)
        {
            return !(a == b);
        }

        public override bool Equals(object other)
        {
            bool ret;
            if (other == null || !(other is Quaternion))
            {
                ret = false;
            }
            else
            {
                Quaternion quaternion = (Quaternion)other;
                ret = (x == quaternion.x && y == quaternion.y && z == quaternion.z && w == quaternion.w);
            }
            return ret;
        }

        public override int GetHashCode()
        {
            return x.GetHashCode() + y.GetHashCode() + z.GetHashCode() + w.GetHashCode();
        }

        public override string ToString()
        {
            return string.Format("({0:F1}, {1:F1}, {2:F1}, {3:F1})", new object[] { x, y, z, w });
        }
    }
}
