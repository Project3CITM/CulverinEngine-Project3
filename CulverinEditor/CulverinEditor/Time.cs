﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Runtime.CompilerServices;

namespace CulverinEditor
{
    public sealed class Time
    {
        public Time() { }

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern float DeltaTime();
    }
}
