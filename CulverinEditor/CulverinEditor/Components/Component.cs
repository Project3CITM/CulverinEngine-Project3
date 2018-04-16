using System.Runtime.CompilerServices;
using System;
using System.Collections.Generic;
using System.Security;

namespace CulverinEditor
{
    public class Component : Object
    {
        public Component() { }

        public GameObject gameObject
        {
            get
            {
                return GetParentGameObject();
            }
        }

        public string tag
        {
            get
            {
                return gameObject.tag;
            }

            set
            {
                gameObject.tag = value;
            }
        }

        public Transform transform
        {
            get
            {
                return gameObject.GetComponent<Transform>();
            }
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern GameObject GetParentGameObject();

        [Obsolete("Now use CulverinBehaviour::GetEnabled(GameObject parent)", false)]
        public extern bool GetScriptEnabled();

        [Obsolete("Now use CulverinBehaviour::SetEnable(bool active, GameObject parent)", false)]
        public extern void SetScriptEnabled(bool val);

        public bool CompareTag(string tag)
        {
            return gameObject.tag == tag;
        }

        public Component GetComponent(Type type)
        {
            return gameObject.GetComponent(type);
        }

        public T GetComponent<T>() where T : Component
        {
            return gameObject.GetComponent(typeof(T)) as T;
        }
    }
}
