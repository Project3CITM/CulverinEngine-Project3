
namespace CulverinEditor
{
    public class Vector3
    {
        public Vector3()
        {
            x = 0.0f;
            y = 0.0f;
            z = 0.0f;
        }

        public Vector3(float x, float y)
        {
            this.x = x;
            this.y = y;
        }
        public Vector3(float x, float y, float z)
        {
            this.x = x;
            this.y = y;
            this.z = z;
        }

        public Vector3(Vector3 cpy)
        {
            this.x = cpy.x;
            this.y = cpy.y;
            this.z = cpy.z;
        }

        public float x;
        public float y;
        public float z;

        private readonly static Vector3 left = new Vector3(-1, 0, 0);
        private readonly static Vector3 down = new Vector3(0, -1, 0);
        private readonly static Vector3 up = new Vector3(0, 1, 0);
        private readonly static Vector3 forward = new Vector3(0, 0, 1);
        private readonly static Vector3 backward = new Vector3(0, 0, -1);
        private readonly static Vector3 one = new Vector3(1, 1, 1);
        private readonly static Vector3 zero = new Vector3(0, 0, 0);
        private readonly static Vector3 right = new Vector3(1, 0, 0);

        public static Vector3 Left
        {
            get
            {
                return left;
            }
        }
        public static Vector3 Down
        {
            get
            {
                return down;
            }
        }
        public static Vector3 Up
        {
            get
            {
                return up;
            }
        }
        public static Vector3 Forward
        {
            get
            {
                return forward;
            }
        }
        public static Vector3 Backward
        {
            get
            {
                return backward;
            }
        }
        public static Vector3 One
        {
            get
            {
                return one;
            }
        }
        public static Vector3 Zero
        {
            get
            {
                return zero;
            }
        }
        public static Vector3 Right
        {
            get
            {
                return right;
            }
        }

        public float Length
        {
            get
            {
                return Magnitude(this);
            }
        }


        //Math operations ---------------------------------
        public static float Angle(Vector3 from, Vector3 to)
        {
            return Mathf.Atan2(to.y, to.x) - Mathf.Atan2(from.y, from.x);
        }

        public static Vector3 ClampMagnitude(Vector3 vector, float maxLength)
        {
            return vector.Normalized * maxLength;
        }

        public static Vector3 Cross(Vector3 lhs, Vector3 rhs)
        {
            float x, y, z;
            x = lhs.y * rhs.z - rhs.y * lhs.z;
            y = (lhs.x * rhs.z - rhs.x * lhs.z) * -1;
            z = lhs.x * rhs.y - rhs.x * lhs.y;

            Vector3 rtnvector = new Vector3(x, y, z);
            return rtnvector;
        }

        public float Distance(Vector3 v1, Vector3 v2)
        {
            Vector3 temp_vector = new Vector3(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z);
            return Mathf.Sqrt(temp_vector.x * temp_vector.x + temp_vector.y * temp_vector.y + temp_vector.z * temp_vector.z);
        }

        public static float Dot(Vector3 lhs, Vector3 rhs)
        {
            return lhs.x * rhs.x + lhs.y * rhs.y;
        }

        public static Vector3 Lerp(Vector3 a, Vector3 b, float t)
        {
            float l_x = Mathf.Lerp(a.x, b.x, t);
            float l_y = Mathf.Lerp(a.y, b.y, t);
            float l_z = Mathf.Lerp(a.z, b.z, t);
            return new Vector3(l_x, l_y, l_z);
        }

        public static float Magnitude(Vector3 vector)
        {
            return Mathf.Sqrt(vector.x * vector.x + vector.y * vector.y + vector.z * vector.z);
        }

        public static Vector3 Max(Vector3 lhs, Vector3 rhs)
        {
            float x = lhs.x;
            if (rhs.x > x) x = rhs.x;
            float y = lhs.y;
            if (rhs.y > y) y = rhs.y;
            float z = lhs.z;
            if (rhs.z > z) z = rhs.z;
            return new Vector3(x, y, z);
        }

        public static Vector3 Min(Vector3 lhs, Vector3 rhs)
        {
            float x = lhs.x;
            if (rhs.x < x) x = rhs.x;
            float y = lhs.y;
            if (rhs.y < y) y = rhs.y;
            float z = lhs.z;
            if (rhs.z < z) z = rhs.z;
            return new Vector3(x, y, z);
        }

        public static Vector3 MoveTowards(Vector3 current, Vector3 target, float maxDistance)
        {
            Vector3 diff = target - current;
            float magnitude = diff.Length;
            Vector3 ret;
            if (magnitude <= maxDistance || magnitude < Mathf.Epsilon)
            {
                ret = target;
            }
            else
            {
                ret = current + diff / magnitude * maxDistance;
            }
            return ret;
        }

        public Vector3 Normalize(Vector3 vector)
        {
            Vector3 value;
            float magnitude = Magnitude(vector);
            if (magnitude > 1E-05f)
            {
                value = vector / magnitude;
            }
            else
            {
                value = Zero;
            }
            return value;
        }

        public static Vector3 Project(Vector3 vector, Vector3 onNormal)
        {
            return new Vector3(((vector * onNormal) / onNormal.Length) * (onNormal / onNormal.Length));
        }

        public static Vector3 ProjectOnPlane(Vector3 vector, Vector3 planeNormal)
        {
            float distance = -Vector3.Dot(planeNormal.Normalized, vector);
            return vector + planeNormal * distance;
        }
       
        public Vector3 Normalized
        {
            get
            {
                return Normalize(this);
            }
        }

        public void Scale(Vector3 scale)
        {
            this.x *= scale.x;
            this.y *= scale.y;
            this.z *= scale.z;
        }

        public void Add(float x,float y,float z)
        {
            this.x += x;
            this.y += y;
            this.z += z;
        }

        public void Set(float x, float y, float z)
        {
            this.x = x;
            this.y = y;
            this.z = z;
        }

        public override int GetHashCode()
        {
            return x.GetHashCode() + y.GetHashCode() + z.GetHashCode();
        }

        public override string ToString()
        {
            string temp = (x.ToString() + ", " + y.ToString() + ", " + z.ToString());
            return temp;
        }

        // Operators --------------------------------------
        public static Vector3 operator +(Vector3 v1, Vector3 v2)
        {
            return new Vector3(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z);
        }

        public static Vector3 operator -(Vector3 v1, Vector3 v2)
        {
            return new Vector3(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z);
        }

        public static Vector3 operator *(Vector3 v1, Vector3 v2)
        {
            return new Vector3(v1.x * v2.x, v1.y * v2.y, v1.z * v2.z);
        }

        public static Vector3 operator *(float d, Vector3 v1)
        {
            return new Vector3(v1.x * d, v1.y * d, v1.z * d);
        }

        public static Vector3 operator *(Vector3 v1, float d)
        {
            return new Vector3(v1.x * d, v1.y * d, v1.z * d);
        }

        public static Vector3 operator /(Vector3 v1, Vector3 v2)
        {
            return new Vector3(v1.x / v2.x, v1.y / v2.y, v1.z / v2.z);
        }

        public static Vector3 operator /(Vector3 v1, float d)
        {
            return new Vector3(v1.x / d, v1.y / d, v1.z / d);
        }

        public static bool operator ==(Vector3 a, Vector3 b)
        {
            return a.Equals(b);
        }

        public static bool operator !=(Vector3 a, Vector3 b)
        {
            return !(a == b);
        }

        public override bool Equals(object other)
        {
            bool ret;
            if (other == null || !(other is Vector3))
            {
                ret = false;
            }
            else
            {
                Vector3 vector = (Vector3)other;
                ret = (x == vector.x && y == vector.y && z == vector.z);
            }
            return ret;
        }
    }
}
