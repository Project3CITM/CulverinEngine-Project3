using CulverinEditor;
using CulverinEditor.Debug;

public class BarrelMovement : CulverinBehaviour
{

    public GameObject instance;
    BarrelManage manage;
    public GameObject barrel = null;

    float length;
    Transform trans;
    bool test = false;
    CompRigidBody rigid;

    void Start()
    {
        instance = GetLinkedObject("instance");
        manage = instance.GetComponent<BarrelManage>();
        trans = gameObject.GetComponent<Transform>();
        barrel = null;
        rigid = gameObject.GetComponent<CompRigidBody>();
    }
    void Update()
    {
   
      
        Vector3 diff = trans.GetPosition() - new Vector3(0,-4,0);
        if (diff.Length >= manage.length)
        {
           Quaternion quat = rigid.GetColliderQuaternion();
            rigid.MoveKinematic(new Vector3(0, -4, 0), quat);
        
         

        }
        if (!manage.stop)
        {
            Vector3 position = new Vector3(trans.local_position.x + manage.movSpeed * manage.p_dt, trans.local_position.y, trans.local_position.z);
            Quaternion quat = rigid.GetColliderQuaternion();
            rigid.MoveKinematic(position,quat);
          
        }

    }

  
}