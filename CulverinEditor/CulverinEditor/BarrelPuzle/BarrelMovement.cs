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
    public bool restart = false;

    void Start()
    {
        instance = GetLinkedObject("instance");
        manage = instance.GetComponent<BarrelManage>();
        trans = gameObject.GetComponent<Transform>();
        barrel = null;
        rigid = gameObject.GetComponent<CompRigidBody>();
        restart = false;
    }
    void Update()
    {

        restart = false;
        Vector3 diff = trans.local_position - new Vector3(manage.restart_pos_x, manage.restart_pos_y, manage.restart_pos_z);
        if (diff.Length >= manage.length)
        {
           Quaternion quat = rigid.GetColliderQuaternion();
            rigid.MoveKinematic(new Vector3(manage.restart_pos_x, manage.restart_pos_y, manage.restart_pos_z),quat);
            restart = true;
         

        }
        if (!manage.stop)
        {
            Vector3 position = new Vector3(trans.local_position.x + manage.movSpeed * 0.016f, trans.local_position.y, trans.local_position.z);
            Quaternion quat = rigid.GetColliderQuaternion();
            rigid.MoveKinematic(position,quat);
          
        }

    }

  
}