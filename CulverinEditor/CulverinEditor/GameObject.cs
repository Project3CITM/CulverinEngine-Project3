using System.Runtime.CompilerServices;
using System;
using System.Collections.Generic;
using System.Security;

namespace CulverinEditor
{
    public sealed class GameObject : Object
    {
        public GameObject()
        {
            CreateGameObject(this);
        }

        public GameObject gameObject
        {
            get
            {
                return GetOwnGameObject();
            }
        }

        public string tag
        {
            get
            {
                return GetTag();
            }

            set
            {
                SetTag(value);
            }
        }

        public Transform transform
        {
            get
            {
                return GetComponent<Transform>();
            }
        }

        public bool IsEquals(GameObject other)
        {
            if (GetName() == other.GetName())
                return true;

            return false;
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        public  extern string GetTag();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public  extern void SetTag(string tg);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public  extern GameObject Find(string name);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public  extern GameObject FindGameObjectWithTag(string tag);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern bool CompareTag(string tag);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern void CreateGameObject(GameObject go);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public  extern void Destroy(GameObject go);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void SetActive(bool go_active);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern bool IsActive();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern bool IsStatic();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void SetParent(GameObject go_parent);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void SetName(string go_name);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern string GetName();

        [MethodImpl(MethodImplOptions.InternalCall)]
        private  extern GameObject GetOwnGameObject();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern int ChildCount();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern GameObject GetChildByIndex(int index);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern GameObject GetChildByName(string name);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern GameObject GetChildByTagIndex(string tag, int index);


        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern Component AddComponent(Type componentType);

        public T AddComponent<T>() where T : Component
        {
            return AddComponent(typeof(T)) as T;
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern Component GetComponent(Type componentType);

        public T GetComponent<T>() where T : Component
        {
            return GetComponent(typeof(T)) as T;
        }
    }
}
