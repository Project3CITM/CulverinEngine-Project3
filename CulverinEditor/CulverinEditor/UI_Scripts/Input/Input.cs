using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Runtime.CompilerServices;

namespace CulverinEditor
{
    public class Input
    {
        
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern bool GetPressAnyKey();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern bool GetKeyDown(KeyCode key);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern bool GetKeyUp(KeyCode key);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern bool GetKeyRepeat(KeyCode key);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern bool GetMouseButtonDown(int button);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern bool GetMouseButtonUp(int button);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern bool GetMouseButtonRepeat(int button);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern Vector3 GetMousePosition();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern int GetMouseXAxis();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern int GetMouseYAxis();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern int GetMouseMoutionX();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern int GetMouseMoutionY();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void SetInputManagerActive(string name,bool set);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void SetInputManagerBlock(string name, bool set);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern bool GetInputManagerActive(string name);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern bool GetInputManagerBlock(string name);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern bool GetInput_KeyDown(string name, string set);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern bool GetInput_KeyUp(string name, string input);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern bool GetInput_KeyRepeat(string name, string input);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern bool GetInput_MouseButtonDown(string name, string input);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern bool GetInput_MouseButtonUp(string name, string input);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern float GetInput_ControllerAxis(string name, string input);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern string GetInput_ControllerActionName(string name, string input, int device, bool negative_key);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern string GetInput_ControllerKeyBindingName(string name, string input, int device, bool negative_key);
    
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void SetInputActionToChange(string name, string input, int device, bool negative_key);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern bool GetChangeInputActive();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern int GetChangeInputState();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern int GetActualDeviceCombo();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void SavePlayerAction();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void LoadDefaultPlayerAction();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void RumblePlay(float intensity, int milliseconds);

    }
}
