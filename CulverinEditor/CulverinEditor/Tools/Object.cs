using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Security;
using System.Runtime.CompilerServices;

namespace CulverinEditor
{
    public class Object
    {
        public Object() { }

        public string name
        {
            get
            {
                return name;
            }
            set
            {
                this.name = value;
            }
        }
        
        public static bool operator ==(Object x, Object y)
        {
            return Equals(x, y);
           //return x.Equals(y);
        }
        public static bool operator !=(Object x, Object y)
        {
            return !Equals(x, y);
          // return !(x.Equals(y));
        }

        public override bool Equals(object other)
        {
            bool ret;
            if (other == null || !(other is Object))
            {
                ret = false;
            }
            else
            {
                ret = true;
            }
            return ret;
        }

        //
        //At the moment only use for GameObjects
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void Destroy(GameObject ganeobject, float time = 0.0f);

        //
        //At the moment only use for GameObjects
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern GameObject Instantiate(string prefab);

        //
        //At the moment only use for GameObjects
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern GameObject Instantiate(string prefab, float time);

        public override int GetHashCode()
        {
            return name.GetHashCode();
        }
    }
}
