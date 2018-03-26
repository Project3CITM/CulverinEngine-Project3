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

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern Material GetMaterialByName(string name);

        //
        // Resumen:
        //     Get Enabled or Disbaled Behaviours.
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern bool GetEnabled(GameObject parent = null);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void SetEnabled(bool active, GameObject parent = null);

    }
}
