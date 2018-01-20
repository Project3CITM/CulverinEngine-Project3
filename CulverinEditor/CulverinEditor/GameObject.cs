using System.Runtime.CompilerServices;
using System;

namespace CulverinEditor
{
    public class GameObject
    {
        public GameObject()
        {
            CreateGameObject(this);
        }
        //public GameObject gameObject
        //{
        //    get
        //    {
        //        return GetGameObject();
        //    }
        //}

        public string name
        {
            get
            {
                return GetName();
            }
            set
            {
                SetName(value);
            }
        }

        public static GameObject gameObject
        {
            get
            {
                return GetOwnGameObject();
            }
        }

        //[MethodImpl(MethodImplOptions.InternalCall)]
        //private extern GameObject GetGameObject();

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern void CreateGameObject(GameObject go);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void Destroy(GameObject go);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void SetActive(bool go_active);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern bool IsActive();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void SetParent(GameObject go_parent);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void SetName(string go_name);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern string GetName();

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern GameObject GetOwnGameObject();

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
