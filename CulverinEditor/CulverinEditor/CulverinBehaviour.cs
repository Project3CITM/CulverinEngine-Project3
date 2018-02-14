using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace CulverinEditor
{
    public abstract class CulverinBehaviour : Component
    {
        public CulverinBehaviour() { }

        public bool enabled
        {
            get
            {
                return enabled;
            }
            set
            {
                enabled = value;
            }
        }


    }
}
