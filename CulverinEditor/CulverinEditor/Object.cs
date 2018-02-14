using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Security;

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
            return x.Equals(y);
        }
        public static bool operator !=(Object x, Object y)
        {
            return !(x == y);
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

        public override int GetHashCode()
        {
            return name.GetHashCode();
        }
    }
}
