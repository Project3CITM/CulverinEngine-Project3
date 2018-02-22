using System.Runtime.CompilerServices;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace CulverinEditor
{
    public class CulverinBehaviour : Component
    {
        public CulverinBehaviour() { }


        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern GameObject GetLinkedObject(string name_variable);



    }


}
