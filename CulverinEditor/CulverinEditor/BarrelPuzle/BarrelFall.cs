using CulverinEditor;
using CulverinEditor.Debug;

public class BarrelFall : CulverinBehaviour
{
    BarrelMovement barrel_mov;

    public GameObject barrel_mov_go;
    CompRigidBody rigid_body;
    Vector3 start_pos;
    void Start()
    {
        barrel_mov_go = GetLinkedObject("barrel_mov_go");
        barrel_mov = barrel_mov_go.GetComponent<BarrelMovement>();
        rigid_body = gameObject.GetComponent<CompRigidBody>();
        start_pos = gameObject.GetComponent<Transform>().local_position;
        Debug.Log(start_pos.ToString());
    }

    void Update()
    {
    
        barrel_mov = barrel_mov_go.GetComponent<BarrelMovement>();
        if (barrel_mov.restart)
        {
            //CAL FUNCTION
           // Vector3 pos = gameObject.GetComponent<CompRigidBody>().GetMaxJointPose();
            Quaternion quat = gameObject.GetComponent<CompRigidBody>().GetColliderQuaternion();

            BarrelManage manage = barrel_mov.instance.GetComponent<BarrelManage>();
            Vector3 parent_pos = new Vector3(manage.restart_pos_x, manage.restart_pos_y, manage.restart_pos_z);

            gameObject.GetComponent<CompRigidBody>().MoveKinematic(start_pos*10 + parent_pos, quat);

            gameObject.GetComponent<CompRigidBody>().ResetForce();
            gameObject.GetComponent<CompRigidBody>().ApplyImpulse(new Vector3(1, 0, 0));

            Debug.Log(start_pos.ToString());
        }

    }

    void OnContact()
    {

        CompRigidBody rbody = GetComponent<CompRigidBody>();

        if (rbody != null)
        {
            rbody.RemoveJoint();

        }
    }
}
